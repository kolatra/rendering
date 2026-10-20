#include "../../include/engine/core/Application.hpp"
#include "engine/graphics/Model.hpp"
#include "engine/graphics/opengl/RendererOpenGL.hpp"

#include "../engine/objects/Penger.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Luna;

void Application::Run() const {
    Penger penger;
    const auto shader = renderer->shaders[0];
    constexpr glm::mat4 model(1.0f);
    const auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(config->width) / static_cast<float>(config->height), 0.1f, 100.0f);
    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    window->Show();
    while (!window->ShouldClose()) {
        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        penger.Draw(*shader);

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }
}

int main() {
    const auto *app = CreateApp();
    app->Run();

    delete app;
    return 0;
}
