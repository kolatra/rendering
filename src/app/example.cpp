#include "engine/core/Application.hpp"
#include "engine/graphics/Model.hpp"
#include "engine/graphics/opengl/RendererOpenGL.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Luna;

class Penger {
public:
    IModel* model;

    Penger() : model(new OpenGL::Model("penger-obj-main/penger/penger.obj", false)) {}

    ~Penger() {}

    void Draw(IShader &shader) {
        model->Draw(shader);
    }
};

static Penger *penger = nullptr;

static void setup_view(IRenderer *renderer, Configuration *config) {
    const auto shader = renderer->shaders[0];
    constexpr glm::mat4 model(1.0f);
    const auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(config->width) / static_cast<float>(config->height), 0.1f, 100.0f);
    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

void Application::Setup() const {
    setup_view(renderer.get(), config.get());

    penger = new Penger();
}

void Application::Loop() const {
    const auto shader = renderer->shaders[0];

    penger->Draw(*shader);
}
