#pragma once

#include <memory>

#include "graphics/Renderer.hpp"
#include "graphics/scene/Scene.hpp"
#include "Config.hpp"

namespace Luna {
class AppCore;
struct Configuration;
struct Window;

std::unique_ptr<AppCore> Initialize(int argc, char **argv);

// Set after config is loaded
inline auto ACTIVE_BACKEND = Backend::None;

class AppCore {
public:
    Configuration config;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;

    ~AppCore();

    void Run();

    double currentTime = 0;
    double lastTime = 0;
    void UpdateTime();

};
}
