#pragma once

#include <memory>

namespace Luna {
struct Configuration;
class IWindow;
class IRenderer;
class GameObject;
struct Floor;

typedef std::shared_ptr<Configuration> HandleConfiguration;
typedef std::shared_ptr<IWindow> HandleWindow;
typedef std::shared_ptr<IRenderer> HandleRenderer;
typedef std::shared_ptr<GameObject> HandleGameObject;

class AppCore {
public:
    HandleConfiguration config;
    HandleWindow window;
    HandleRenderer renderer;

    // The in-editor wiremesh floor, doesn't show on final product
    std::unique_ptr<Floor> floor;

    void Run();

    double currentTime = 0;

    double mouseX = 400;
    double mouseY = 300;
    bool holdingMouse = false;
    void UpdateMouse(double x, double y);

    void RegisterObject(HandleGameObject &obj);
};

std::unique_ptr<AppCore> Initialize();

void AddRenderingLayer(std::unique_ptr<AppCore> &app);

}
