#ifndef RASTERSONG_CLIP_H
#define RASTERSONG_CLIP_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{

    // Base class representing a video or audio clip with temporal and channel data.
    // Subclasses (VideoClip, AudioClip) extend this with format-specific properties.
    class Clip : public Resource
    {
        GDCLASS(Clip, Resource)

    public:
        Clip() = default;
        ~Clip() override = default;

        // Path to the media file
        void set_path(const String &p_path);
        String get_path() const;

        // Duration in samples (format-dependent interpretation)
        void set_duration_samples(int p_samples);
        int get_duration_samples() const;

        // Offset in samples from the start of the file
        void set_offset_samples(int p_samples);
        int get_offset_samples() const;

        // Number of channels (audio: 1-2, video: RGB channels)
        void set_channels(int p_channels);
        int get_channels() const;

    protected:
        static void _bind_methods();

    protected:
        String _path;
        int _duration_samples = 0;
        int _offset_samples = 0;
        int _channels = 0;
    };

} // namespace godot

#endif // RASTERSONG_CLIP_H
