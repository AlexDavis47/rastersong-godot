#include "ffmpeg_tester.h"

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace godot
{

    void FFmpegTester::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_ffmpeg_version"), &FFmpegTester::get_ffmpeg_version);
        ClassDB::bind_method(D_METHOD("is_ffmpeg_available"), &FFmpegTester::is_ffmpeg_available);
        ClassDB::bind_method(D_METHOD("get_libavutil_version"), &FFmpegTester::get_libavutil_version);
        ClassDB::bind_method(D_METHOD("get_libavcodec_version"), &FFmpegTester::get_libavcodec_version);
        ClassDB::bind_method(D_METHOD("get_libavformat_version"), &FFmpegTester::get_libavformat_version);
    }

    String FFmpegTester::get_ffmpeg_version() const
    {
        return String("FFmpeg libraries linked successfully");
    }

    bool FFmpegTester::is_ffmpeg_available() const
    {
        return true;
    }

    String FFmpegTester::get_libavutil_version() const
    {
        unsigned int version = avutil_version();
        int major = (version >> 16) & 0xFF;
        int minor = (version >> 8) & 0xFF;
        int patch = version & 0xFF;
        return vformat("libavutil %d.%d.%d", major, minor, patch);
    }

    String FFmpegTester::get_libavcodec_version() const
    {
        unsigned int version = LIBAVCODEC_VERSION_INT;
        int major = (version >> 16) & 0xFF;
        int minor = (version >> 8) & 0xFF;
        int patch = version & 0xFF;
        return vformat("libavcodec %d.%d.%d", major, minor, patch);
    }

    String FFmpegTester::get_libavformat_version() const
    {
        unsigned int version = LIBAVFORMAT_VERSION_INT;
        int major = (version >> 16) & 0xFF;
        int minor = (version >> 8) & 0xFF;
        int patch = version & 0xFF;
        return vformat("libavformat %d.%d.%d", major, minor, patch);
    }

}