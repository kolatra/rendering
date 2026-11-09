#include "Penguin.h"
#include "graphics/IRenderer.hpp"

void Penger::Draw(const std::unique_ptr<Luna::IRenderer> &renderer) const {
    glm::mat4 pos(1.0f);
    // oh boy he high up there
    pos = glm::translate(pos, glm::vec3(0.0f, 2.0f, 0.0f));

    const auto shader = renderer->shaders[0];
    shader->use();
    shader->setMat4("model", pos);
    shader->setMat4("view", renderer->camera->view);
    shader->setMat4("projection", renderer->camera->projection);

    // enable back-face culling for his render
    renderer->setCulling(true);
    this->model->Draw(*shader);
}
