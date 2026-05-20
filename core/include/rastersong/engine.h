#pragma once

namespace rastersong
{

    class Engine
    {
    public:
        Engine();
        ~Engine();

        const char *get_version() const;
    };

} // namespace rastersong
