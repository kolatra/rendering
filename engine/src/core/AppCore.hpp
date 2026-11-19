#pragma once

#include <memory>

#include "graphics/Renderer.hpp"
#include "Config.hpp"

namespace Luna {
class AppCore;
struct Configuration;
class Window;
struct Floor;

std::unique_ptr<AppCore> Initialize(int argc, char **argv);

// Set after config is loaded
inline auto ACTIVE_BACKEND = Backend::None;

class AppCore {
public:
    Configuration config;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;

    // The in-editor wiremesh floor, doesn't show on final product
    std::unique_ptr<Floor> floor;

    int Run();

    double currentTime = 0;
    double lastTime = 0;

    Scene& CreateScene(bool controls);
    void PresentScene(std::unique_ptr<Scene> scene);
    std::unique_ptr<Scene> ClearActiveScene();
};
}
