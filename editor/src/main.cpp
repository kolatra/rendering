#include <spdlog/spdlog.h>

#include "core/AppCore.hpp"
#include "graphics/IRenderer.hpp"

namespace {
class BusinessMan : public Luna::GameObject {
public:
    std::unique_ptr<Luna::IModel> model;
    glm::mat4 position {1.0f};

    explicit BusinessMan(const std::shared_ptr<Luna::Configuration> &config) :
        model(
            Luna::IModel::Create(
                config.get(),
                "assets/penger-obj-main/suitger/suitedpenger.obj"
            )
        ) {}

    void Draw(const std::unique_ptr<Luna::IRenderer> &renderer) const override {
        const auto shader = renderer->shaders[0];
        shader->use();
        shader->setMat4("model", position);
        shader->setMat4("view", renderer->camera->view);
        shader->setMat4("projection", renderer->camera->projection);

        // enable back-face culling for his render
        renderer->setCulling(true);
        this->model->Draw(*shader);
        renderer->setCulling(false);
    }
};
}

int main(int argc, char *argv[]) {
    const auto app = Luna::Initialize();

    // create our first game object, engine cleans it up
    const auto importantGuy = new BusinessMan(app->config);
    app->RegisterObject(importantGuy);

    app->Run();

    spdlog::info("Exiting successfully");
    return 0;
}
