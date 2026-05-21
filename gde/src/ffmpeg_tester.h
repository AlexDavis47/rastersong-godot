#ifndef RASTERSONG_FFMPEG_TESTER_H
#define RASTERSONG_FFMPEG_TESTER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{

    class FFmpegTester : public RefCounted
    {
        GDCLASS(FFmpegTester, RefCounted)

    public:
        FFmpegTester() = default;
        ~FFmpegTester() override = default;

        String get_ffmpeg_version() const;
        bool is_ffmpeg_available() const;
        String get_libavutil_version() const;
        String get_libavcodec_version() const;
        String get_libavformat_version() const;

    protected:
        static void _bind_methods();
    };

} // namespace godot

#endif // RASTERSONG_FFMPEG_TESTER_H
