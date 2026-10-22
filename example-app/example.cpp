#include "engine/core/Application.hpp"
#include "engine/graphics/opengl/GlModel.hpp"

using namespace Luna;

namespace {
    class Penger {
    public:
        IModel* model;
        Penger() : model(new OpenGL::GlModel("assets/penger-obj-main/penger/penger.obj", false)) {}
        ~Penger() {}
    };
}

static Penger *penger = nullptr;

void Application::Setup() const {
    // Compile our shader
    auto *shader = new OpenGL::GLShader("shaders/triangle.vsh", "shaders/triangle.fsh");

    // Give the shader to the engine so it's cleaned up for us afterwards and we
    // can use it everywhere.
    shader->name = "Triangles";
    renderer->shaders.push_back(shader);

    // Initialize objects
    penger = new Penger();
}

void Application::Loop() const {
    const auto shader = renderer->shaders[0];
    constexpr glm::mat4 model(1.0f);
    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", renderer->camera->view);
    shader->setMat4("projection", renderer->camera->projection);

    // enable back-face culling for the penguin render
    renderer->setCulling(true);
    penger->model->Draw(*shader);
}
