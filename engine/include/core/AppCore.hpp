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
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<Floor> floor;

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool holdingMouse = false;
    void UpdateMouse(double x, double y);

    void RegisterObject(GameObject *obj) const;
};

std::tuple<
    std::unique_ptr<AppCore>&,
    std::shared_ptr<Configuration>&,
    std::unique_ptr<IWindow>&,
    std::unique_ptr<IRenderer>&
> Initialize();

void AddRenderingLayer(std::unique_ptr<AppCore> &app);

}
