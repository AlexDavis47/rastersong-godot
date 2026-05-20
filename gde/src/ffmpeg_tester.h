#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

class FFmpegTester : public RefCounted
{
    GDCLASS(FFmpegTester, RefCounted)

protected:
    static void _bind_methods();

public:
    FFmpegTester() = default;
    ~FFmpegTester() override = default;

    String get_ffmpeg_version() const;
    bool is_ffmpeg_available() const;
    String get_libavutil_version() const;
    String get_libavcodec_version() const;
    String get_libavformat_version() const;
};
