#ifndef RASTERSONG_AUDIO_CLIP_H
#define RASTERSONG_AUDIO_CLIP_H

#include "clip.hpp"

namespace godot
{

    // Audio-specific clip data extending the base Clip class.
    // Inherits all base properties; no additional format-specific data required.
    class AudioClip : public Clip
    {
        GDCLASS(AudioClip, Clip)

    public:
        AudioClip() = default;
        ~AudioClip() override = default;

    protected:
        static void _bind_methods();
    };

} // namespace godot

#endif // RASTERSONG_AUDIO_CLIP_H
