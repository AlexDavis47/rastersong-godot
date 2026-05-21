#ifndef RASTERSONG_VIDEO_CLIP_H
#define RASTERSONG_VIDEO_CLIP_H

#include "clip.hpp"

namespace godot
{

    // Video-specific clip data extending the base Clip class.
    // Provides frame rate and resolution information, plus convenience methods.
    class VideoClip : public Clip
    {
        GDCLASS(VideoClip, Clip)

    public:
        VideoClip() = default;
        ~VideoClip() override = default;

        // Frame rate in frames per second
        void set_frame_rate(float p_frame_rate);
        float get_frame_rate() const;

        // Video width in pixels
        void set_width(int p_width);
        int get_width() const;

        // Video height in pixels
        void set_height(int p_height);
        int get_height() const;

        // Convenience method: returns width * height * channels
        int get_samples_per_frame() const;

    protected:
        static void _bind_methods();

    private:
        float _frame_rate = 0.0f;
        int _width = 0;
        int _height = 0;
    };

} // namespace godot

#endif // RASTERSONG_VIDEO_CLIP_H
