#pragma once

#include <memory>
#include <graphics/IRenderer.hpp>

namespace Luna {
struct Configuration;
class IWindow;
class GameObject;
struct Floor;

class AppCore {
public:
    std::shared_ptr<Configuration> config;
    std::shared_ptr<IWindow> window;
    std::shared_ptr<IRenderer> renderer;

    // The in-editor wiremesh floor, doesn't show on final product
    std::unique_ptr<Floor> floor;

    int Run();

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool holdingMouse = false;
    void UpdateMouse(double x, double y);

    template <typename T>
    void RegisterObject() const {
        const std::shared_ptr<GameObject> newObject = std::make_shared<T>(this->config);
        this->renderer->objects.push_back(newObject);
    }
};

std::unique_ptr<AppCore> Initialize(int argc, char **argv);
}
