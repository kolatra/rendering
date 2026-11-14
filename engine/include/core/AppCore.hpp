#pragma once

#include <memory>
#include <graphics/Renderer.hpp>

namespace Luna {
struct Configuration;
class IWindow;
struct Floor;

class AppCore {
public:
    std::shared_ptr<Configuration> config;
    std::shared_ptr<IWindow> window;
    std::shared_ptr<Renderer> renderer;

    // The in-editor wiremesh floor, doesn't show on final product
    std::unique_ptr<Floor> floor;

    int Run();

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool holdingMouse = false;
    void UpdateMouse(double x, double y);
};

std::unique_ptr<AppCore> Initialize(int argc, char **argv);
}
