#include "video_clip.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot
{

    // ---------------------------------------------------------------------------
    // GDScript bindings
    // ---------------------------------------------------------------------------

    void VideoClip::_bind_methods()
    {
        // Frame rate
        ClassDB::bind_method(D_METHOD("set_frame_rate", "frame_rate"), &VideoClip::set_frame_rate);
        ClassDB::bind_method(D_METHOD("get_frame_rate"), &VideoClip::get_frame_rate);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frame_rate"), "set_frame_rate", "get_frame_rate");

        // Width
        ClassDB::bind_method(D_METHOD("set_width", "width"), &VideoClip::set_width);
        ClassDB::bind_method(D_METHOD("get_width"), &VideoClip::get_width);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "width"), "set_width", "get_width");

        // Height
        ClassDB::bind_method(D_METHOD("set_height", "height"), &VideoClip::set_height);
        ClassDB::bind_method(D_METHOD("get_height"), &VideoClip::get_height);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "height"), "set_height", "get_height");

        // Convenience method
        ClassDB::bind_method(D_METHOD("get_samples_per_frame"), &VideoClip::get_samples_per_frame);
    }

    // ---------------------------------------------------------------------------
    // Public API
    // ---------------------------------------------------------------------------

    void VideoClip::set_frame_rate(float p_frame_rate)
    {
        _frame_rate = p_frame_rate;
    }

    float VideoClip::get_frame_rate() const
    {
        return _frame_rate;
    }

    void VideoClip::set_width(int p_width)
    {
        _width = p_width;
    }

    int VideoClip::get_width() const
    {
        return _width;
    }

    void VideoClip::set_height(int p_height)
    {
        _height = p_height;
    }

    int VideoClip::get_height() const
    {
        return _height;
    }

    int VideoClip::get_samples_per_frame() const
    {
        return _width * _height * _channels;
    }

} // namespace godot
