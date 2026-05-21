#include "clip.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot
{

    // ---------------------------------------------------------------------------
    // GDScript bindings
    // ---------------------------------------------------------------------------

    void Clip::_bind_methods()
    {
        // Path
        ClassDB::bind_method(D_METHOD("set_path", "path"), &Clip::set_path);
        ClassDB::bind_method(D_METHOD("get_path"), &Clip::get_path);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "path"), "set_path", "get_path");

        // Duration
        ClassDB::bind_method(D_METHOD("set_duration_samples", "samples"), &Clip::set_duration_samples);
        ClassDB::bind_method(D_METHOD("get_duration_samples"), &Clip::get_duration_samples);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "duration_samples"), "set_duration_samples", "get_duration_samples");

        // Offset
        ClassDB::bind_method(D_METHOD("set_offset_samples", "samples"), &Clip::set_offset_samples);
        ClassDB::bind_method(D_METHOD("get_offset_samples"), &Clip::get_offset_samples);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "offset_samples"), "set_offset_samples", "get_offset_samples");

        // Channels
        ClassDB::bind_method(D_METHOD("set_channels", "channels"), &Clip::set_channels);
        ClassDB::bind_method(D_METHOD("get_channels"), &Clip::get_channels);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "channels"), "set_channels", "get_channels");
    }

    // ---------------------------------------------------------------------------
    // Public API
    // ---------------------------------------------------------------------------

    void Clip::set_path(const String &p_path)
    {
        _path = p_path;
    }

    String Clip::get_path() const
    {
        return _path;
    }

    void Clip::set_duration_samples(int p_samples)
    {
        _duration_samples = p_samples;
    }

    int Clip::get_duration_samples() const
    {
        return _duration_samples;
    }

    void Clip::set_offset_samples(int p_samples)
    {
        _offset_samples = p_samples;
    }

    int Clip::get_offset_samples() const
    {
        return _offset_samples;
    }

    void Clip::set_channels(int p_channels)
    {
        _channels = p_channels;
    }

    int Clip::get_channels() const
    {
        return _channels;
    }

} // namespace godot
