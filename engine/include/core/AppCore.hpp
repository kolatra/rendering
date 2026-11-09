#pragma once

#include <memory>

namespace Luna {
struct Configuration;
class IWindow;
class IRenderer;
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

    void RegisterObject(const std::shared_ptr<GameObject> &obj) const;
};

template <typename T>
static void RegisterObject(const std::unique_ptr<AppCore> &app) {
    const std::shared_ptr<GameObject> newObject = std::make_shared<T>(app->config);
    app->RegisterObject(newObject);
}
std::unique_ptr<AppCore> Initialize();

void AddRenderingLayer(std::unique_ptr<AppCore> &app);

}
