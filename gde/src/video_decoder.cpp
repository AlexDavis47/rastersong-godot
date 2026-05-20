#include "video_decoder.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

extern "C"
{
#include <libavutil/imgutils.h>
}

using namespace godot;

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

VideoDecoder::VideoDecoder() = default;

VideoDecoder::~VideoDecoder()
{
    _cleanup();
}

// ---------------------------------------------------------------------------
// GDScript bindings
// ---------------------------------------------------------------------------

void VideoDecoder::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("open", "path"), &VideoDecoder::open);
    ClassDB::bind_method(D_METHOD("close"), &VideoDecoder::close);
    ClassDB::bind_method(D_METHOD("decode_frame", "index"), &VideoDecoder::decode_frame);
    ClassDB::bind_method(D_METHOD("get_width"), &VideoDecoder::get_width);
    ClassDB::bind_method(D_METHOD("get_height"), &VideoDecoder::get_height);
    ClassDB::bind_method(D_METHOD("is_open"), &VideoDecoder::is_open);
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void VideoDecoder::_cleanup()
{
    if (_packet)
    {
        av_packet_free(&_packet);
    }
    if (_frame_rgba)
    {
        av_frame_free(&_frame_rgba);
    }
    if (_frame)
    {
        av_frame_free(&_frame);
    }
    if (_sws_ctx)
    {
        sws_freeContext(_sws_ctx);
        _sws_ctx = nullptr;
    }
    if (_codec_ctx)
    {
        avcodec_free_context(&_codec_ctx);
    }
    if (_fmt_ctx)
    {
        avformat_close_input(&_fmt_ctx);
    }

    _video_stream_idx = -1;
    _frame_width = 0;
    _frame_height = 0;
    _current_frame_index = -1;
    _frame_data = PackedByteArray();
}

bool VideoDecoder::_setup_sws()
{
    if (_sws_ctx)
    {
        sws_freeContext(_sws_ctx);
        _sws_ctx = nullptr;
    }

    _sws_ctx = sws_getContext(
        _frame_width, _frame_height, _codec_ctx->pix_fmt,
        _frame_width, _frame_height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!_sws_ctx)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to create swscale context");
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool VideoDecoder::open(const String &p_path)
{
    _cleanup();

    const CharString path_utf8 = p_path.utf8();

    if (avformat_open_input(&_fmt_ctx, path_utf8.get_data(), nullptr, nullptr) < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to open file: ", p_path);
        return false;
    }

    if (avformat_find_stream_info(_fmt_ctx, nullptr) < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to read stream info");
        _cleanup();
        return false;
    }

    // Find the best video stream and its decoder.
    const AVCodec *codec = nullptr;
    _video_stream_idx = av_find_best_stream(_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    if (_video_stream_idx < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: no video stream found");
        _cleanup();
        return false;
    }

    _codec_ctx = avcodec_alloc_context3(codec);
    if (!_codec_ctx)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to allocate codec context");
        _cleanup();
        return false;
    }

    if (avcodec_parameters_to_context(_codec_ctx, _fmt_ctx->streams[_video_stream_idx]->codecpar) < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to copy codec parameters");
        _cleanup();
        return false;
    }

    if (avcodec_open2(_codec_ctx, codec, nullptr) < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to open codec");
        _cleanup();
        return false;
    }

    _frame_width = _codec_ctx->width;
    _frame_height = _codec_ctx->height;

    // Allocate reusable objects.
    _frame = av_frame_alloc();
    _frame_rgba = av_frame_alloc();
    _packet = av_packet_alloc();
    if (!_frame || !_frame_rgba || !_packet)
    {
        UtilityFunctions::push_error("VideoDecoder: allocation failure");
        _cleanup();
        return false;
    }

    // Pre-allocate the RGBA frame buffer.
    _frame_rgba->format = AV_PIX_FMT_RGBA;
    _frame_rgba->width = _frame_width;
    _frame_rgba->height = _frame_height;
    if (av_frame_get_buffer(_frame_rgba, 0) < 0)
    {
        UtilityFunctions::push_error("VideoDecoder: failed to allocate RGBA frame buffer");
        _cleanup();
        return false;
    }

    _frame_data.resize(_frame_width * _frame_height * 4);

    if (!_setup_sws())
    {
        _cleanup();
        return false;
    }

    return true;
}

void VideoDecoder::close()
{
    _cleanup();
}

PackedByteArray VideoDecoder::decode_frame(int p_index)
{
    if (!_fmt_ctx || _video_stream_idx < 0)
        return PackedByteArray();

    AVStream *stream = _fmt_ctx->streams[_video_stream_idx];

    // If we haven't decoded any frames yet, initialize position to 0
    if (_current_frame_index == -1)
    {
        _current_frame_index = 0;
        // Seek to the beginning
        if (av_seek_frame(_fmt_ctx, _video_stream_idx, 0, AVSEEK_FLAG_BACKWARD) < 0)
        {
            UtilityFunctions::push_error("VideoDecoder: failed to seek to start");
            return PackedByteArray();
        }
        avcodec_flush_buffers(_codec_ctx);
    }

    // If requesting a frame before current position, seek back
    if (p_index < _current_frame_index)
    {
        // Convert frame index to PTS (presentation timestamp)
        const int64_t ts = static_cast<int64_t>(p_index * av_q2d(av_inv_q(stream->r_frame_rate)));
        if (av_seek_frame(_fmt_ctx, _video_stream_idx, ts, AVSEEK_FLAG_BACKWARD) < 0)
        {
            UtilityFunctions::push_error("VideoDecoder: failed to seek to frame ", p_index);
            return PackedByteArray();
        }
        avcodec_flush_buffers(_codec_ctx);
        _current_frame_index = p_index;
    }

    // Decode frames until we reach the requested frame
    while (_current_frame_index <= p_index)
    {
        int ret = avcodec_receive_frame(_codec_ctx, _frame);

        if (ret == 0)
        {
            // Convert to RGBA
            sws_scale(_sws_ctx,
                      _frame->data, _frame->linesize, 0, _frame_height,
                      _frame_rgba->data, _frame_rgba->linesize);

            // Copy row-by-row to account for any linesize padding
            uint8_t *dst = _frame_data.ptrw();
            const int row_bytes = _frame_width * 4;
            for (int y = 0; y < _frame_height; ++y)
            {
                memcpy(dst + y * row_bytes,
                       _frame_rgba->data[0] + y * _frame_rgba->linesize[0],
                       row_bytes);
            }

            av_frame_unref(_frame);

            if (_current_frame_index == p_index)
            {
                // We got the frame we wanted
                _current_frame_index++;
                return _frame_data;
            }

            _current_frame_index++;
            continue;
        }

        if (ret != AVERROR(EAGAIN))
            return PackedByteArray(); // EOF or error

        // Read the next packet to feed more data to decoder
        if (av_read_frame(_fmt_ctx, _packet) >= 0)
        {
            if (_packet->stream_index == _video_stream_idx)
            {
                avcodec_send_packet(_codec_ctx, _packet);
            }
            av_packet_unref(_packet);
        }
        else
        {
            // EOF reached
            return PackedByteArray();
        }
    }

    return PackedByteArray();
}

int VideoDecoder::get_width() const { return _frame_width; }
int VideoDecoder::get_height() const { return _frame_height; }

bool VideoDecoder::is_open() const
{
    return _fmt_ctx != nullptr;
}
