#include "engine/core/Application.hpp"
#include "engine/graphics/opengl/GlModel.hpp"

namespace {
    class Penger {
    public:
        Luna::IModel* model;
        Penger() : model(new Luna::OpenGL::GlModel("assets/penger-obj-main/suitger/suitedpenger.obj", false)) {}
        ~Penger() {}
    };

    Penger *penger = nullptr;
}

void Luna::Application::Setup() const {
    // Compile our shader
    auto *shader = new OpenGL::GLShader("shaders/triangle.vsh", "shaders/triangle.fsh");

    // Give the shader to the engine so it's cleaned up for us afterwards and we
    // can use it everywhere.
    shader->name = "Triangles";
    renderer->shaders.push_back(shader);

    // Initialize objects
    penger = new Penger();
}

void Luna::Application::Loop() const {
    glm::mat4 model(1.0f);
    // oh boy he high up there
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));

    const auto shader = renderer->shaders[0];
    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", renderer->camera->view);
    shader->setMat4("projection", renderer->camera->projection);

    // enable back-face culling for his render
    renderer->setCulling(true);
    penger->model->Draw(*shader);
}
