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

    void Run();

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool holdingMouse = false;
    void UpdateMouse(double x, double y);

    void RegisterObject(GameObject *obj) const;
};

std::unique_ptr<AppCore> Initialize();

void AddRenderingLayer(std::unique_ptr<AppCore> &app);

}
