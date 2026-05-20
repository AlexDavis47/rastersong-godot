#include "rastersong/engine.h"

namespace rastersong
{

    Engine::Engine() {}

    Engine::~Engine() {}

    const char *Engine::get_version() const
    {
        return "0.1.0-dev";
    }

} // namespace rastersong
