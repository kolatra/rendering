#include <stdexcept>

#include "spdlog/spdlog.h"

#include "graphics/Renderer.hpp"
#include "graphics/scene/Scene.hpp"
#include "graphics/impl/opengl/GlModel.hpp"
#include "graphics/impl/opengl/RendererOpenGL.hpp"
#include "graphics/impl/vulkan/RendererVulkan.hpp"
#include "LunaEngine.hpp"
#include "Utilities.hpp"

namespace Luna {
Renderer::Renderer() = default;
Renderer::~Renderer() = default;

std::unique_ptr<Renderer> StartGraphics() {
    spdlog::debug("Starting backend...");
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::Implementation>();
        case Backend::Vulkan:
            return std::make_unique<Vulkan::Implementation>();
        case Backend::Metal:
            spdlog::error("Metal is not implemented!");
            exit(422);
        case Backend::None:
        default:
            throw std::runtime_error("StartGraphics: This should never happen.");
    }
}

std::unique_ptr<Shader> Shader::Create(const std::string &name) {
    std::unique_ptr<Shader> out;
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL:
            out = OpenGL::GLShader::FromFile(name);
            break;
        case Backend::Vulkan:
            out = std::make_unique<Vulkan::VShader>(name);
            break;
        case Backend::Metal:
            throw std::runtime_error("Metal shaders not implemented!");
        case Backend::None:
        default:
            throw std::runtime_error("Invalid state when creating shader!");
    }
    return out;
}

std::unique_ptr<Model> Model::Create(std::string path, bool gamma) {
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::GlModel>(path, gamma);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan models are not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal models are not implemented!");
        case Backend::None:
        default:
            throw std::runtime_error("Model: Backend was cleared!");
    }
}

std::unique_ptr<Floor> Floor::Create() {
    std::unique_ptr<Floor> out;
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL:
            out = std::make_unique<OpenGL::GlFloor>();
            break;
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
        default:
            throw std::runtime_error("Invalid state!");
    }

    out->Setup();
    return out;
}

void GameObject::Draw(Renderer &renderer) const {
    shader->use();
    shader->setMat4("model", position);
    shader->setMat4("view", renderer.currentScene->camera->view);
    shader->setMat4("projection", renderer.currentScene->camera->projection);

    if (culling)
        renderer.setCulling(true);
    this->model->Draw(*shader);
}

}
