#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

using namespace godot;

// Opens a media file and decodes video frames on demand.
// All decoded frames are returned as raw RGBA pixel data via get_frame_data().
class VideoDecoder : public RefCounted
{
    GDCLASS(VideoDecoder, RefCounted)

protected:
    static void _bind_methods();

public:
    VideoDecoder();
    ~VideoDecoder() override;

    // Open a file. Returns false on failure.
    bool open(const String &p_path);

    // Release all resources.
    void close();

    // Decode a specific frame by index.
    // Returns raw RGBA8888 pixel data, or empty array on failure.
    PackedByteArray decode_frame(int p_index);

    // Video dimensions.
    int get_width() const;
    int get_height() const;

    // Whether a file is currently open.
    bool is_open() const;

private:
    AVFormatContext *_fmt_ctx = nullptr;
    AVCodecContext *_codec_ctx = nullptr;
    SwsContext *_sws_ctx = nullptr;
    AVFrame *_frame = nullptr;      // decoded frame (native pixel format)
    AVFrame *_frame_rgba = nullptr; // converted RGBA frame
    AVPacket *_packet = nullptr;

    int _video_stream_idx = -1;
    int _frame_width = 0;
    int _frame_height = 0;
    int _current_frame_index = -1; // Track current position in stream

    PackedByteArray _frame_data;

    void _cleanup();
    bool _setup_sws();
};
