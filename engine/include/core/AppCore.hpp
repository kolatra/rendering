#pragma once

#include <memory>
#include <graphics/Renderer.hpp>

namespace Luna {
struct Configuration;
class IWindow;
struct Floor;

class GameObject {
public:
    uint32_t renderPriority = 0;
    glm::mat4 position;
    std::shared_ptr<Model> model;
    std::string modelPath;
    bool culling = false;

    virtual ~GameObject() { model.reset(); }
    virtual void Tick() = 0;
    virtual void Draw(const std::shared_ptr<Renderer> &renderer) const;

    uint32_t GetPriority() const { return renderPriority; }
};

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

    template <typename T>
    void RegisterObject() const {
        const std::shared_ptr<GameObject> newObject = std::make_shared<T>();
        newObject->model = Model::Create(config, newObject->modelPath);
        this->renderer->objects.push_back(newObject);
    }
};

std::unique_ptr<AppCore> Initialize(int argc, char **argv);
}
