#pragma once

#include <memory>

#include "graphics/Renderer.hpp"
#include "Config.hpp"

namespace Luna {
struct Configuration;
class IWindow;
struct Floor;

// Set after config is loaded
inline auto ACTIVE_BACKEND = Backend::None;

class AppCore {
public:
    Configuration config;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<Renderer> renderer;

    // The in-editor wiremesh floor, doesn't show on final product
    std::unique_ptr<Floor> floor;

    int Run();

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool mouseMovement = false;
    bool mouseCaptured = true;
    void UpdateMouse(double x, double y);
    void LoadScene(std::unique_ptr<Scene> scene);
    std::unique_ptr<Scene> UnloadScene();
};

std::unique_ptr<AppCore> Initialize(int argc, char **argv);
Scene& CreateScene(const std::unique_ptr<AppCore> &app, bool controls);
}
