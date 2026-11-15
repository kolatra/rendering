#include <stdexcept>

#include "spdlog/spdlog.h"

#include "graphics/Renderer.hpp"
#include "scene/Scene.hpp"
#include "graphics/opengl/GlModel.hpp"
#include "graphics/opengl/RendererOpenGL.hpp"
#include "graphics/vulkan/RendererVulkan.hpp"
#include "LunaEngine.hpp"
#include "Utilities.hpp"

namespace Luna {
Renderer::Renderer() = default;
Renderer::~Renderer() = default;

std::unique_ptr<Renderer> StartBackend(Configuration &config) {
    spdlog::info("Starting backend...");
    switch (config.rendering_api) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::Implementation>();
        case Backend::Vulkan:
            return std::make_unique<Vulkan::Implementation>();
        case Backend::Metal:
            spdlog::error("Metal is not implemented!");
            exit(422);
        case Backend::None:
        default:
            throw std::runtime_error("StartBackend: This should never happen.");
    }
}

std::shared_ptr<Shader> Shader::Create(const AppCore &app, const std::unique_ptr<Scene> &scene, const std::string &name) {
    std::shared_ptr<Shader> out;
    switch (app.config->rendering_api) {
        case Backend::OpenGL: {
            out = OpenGL::GLShader::FromFile(name);
            break;
        }
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan shaders are not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal shaders not implemented!");
        case Backend::None:
        default:
            throw std::runtime_error("Invalid state when creating shader!");
    }

    scene->shaders.push_back(out);
    return out;
}

std::unique_ptr<Model> Model::Create(Configuration &config, std::string path, bool gamma) {
    switch (config.rendering_api) {
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

std::unique_ptr<Floor> Floor::Create(Configuration &config) {
    std::unique_ptr<Floor> out;
    switch (config.rendering_api) {
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

void GameObject::Draw(Renderer *renderer) const {
    const auto shader = renderer->currentScene->shaders[0];
    shader->use();
    shader->setMat4("model", position);
    shader->setMat4("view", renderer->currentScene->camera->view);
    shader->setMat4("projection", renderer->currentScene->camera->projection);

    if (culling)
        renderer->setCulling(true);
    this->model->Draw(*shader);
}

void Renderer::LoadScene(std::unique_ptr<Scene> scene) {
    // i don't know yet how i want to even store scenes so this is a big WIP
    currentScene = std::move(scene);
}

std::unique_ptr<Scene> Renderer::UnloadScene() {
    auto scene = std::move(currentScene);
    currentScene = nullptr;
    return scene;
}
}
