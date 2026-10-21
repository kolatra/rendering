#include "engine/core/Application.hpp"
#include "engine/graphics/Model.hpp"
#include "engine/graphics/opengl/RendererOpenGL.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../engine/objects/Penger.hpp"

using namespace Luna;

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
    // General application setup goes here
    setup_view(renderer.get(), config.get());

    // Resource initialization
    auto *penger = new Penger;
    renderer->pengers.push_back(penger);
}

// Called every frame by the engine.
void Application::Loop() const {
    const auto shader = renderer->shaders[0];
    const auto penger = renderer->pengers[0];
    penger->Draw(*shader);
}
