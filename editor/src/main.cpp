#include <spdlog/spdlog.h>

#include "core/AppCore.hpp"
#include "graphics/IRenderer.hpp"

namespace {
    class Penger : public Luna::GameObject {
    public:
        std::unique_ptr<Luna::IModel> model;

        explicit Penger(const std::shared_ptr<Luna::Configuration> &config) :
            model(Luna::IModel::Create(config.get(), "assets/penger-obj-main/suitger/suitedpenger.obj", false))
        {}

        void Draw(const std::unique_ptr<Luna::IRenderer> &renderer) const override;
    };
}

void Penger::Draw(const std::unique_ptr<Luna::IRenderer> &renderer) const {
    glm::mat4 pos(1.0f);
    pos = glm::translate(pos, glm::vec3(0.0f, 2.0f, 0.0f));

    const auto shader = renderer->shaders[0];
    shader->use();
    shader->setMat4("model", pos);
    shader->setMat4("view", renderer->camera->view);
    shader->setMat4("projection", renderer->camera->projection);

    // enable back-face culling for his render
    renderer->setCulling(true);
    this->model->Draw(*shader);
    renderer->setCulling(false);
}

int main(int argc, char *argv[]) {
    const auto app = Luna::Initialize();

    // create our first game object, engine cleans it up
    const auto penger = new Penger(app->config);
    app->RegisterObject(penger);

    app->Run();

    spdlog::info("Exiting successfully");
    return 0;
}
