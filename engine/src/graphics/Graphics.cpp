#include "../core/Config.hpp"
#include "IRenderer.hpp"
#include "opengl/RendererOpenGL.hpp"

#include <stdexcept>

#include "opengl/GlModel.hpp"
#include "spdlog/spdlog.h"
#include "vulkan/RendererVulkan.hpp"
#include "../core/AppCore.hpp"

namespace Luna {
std::unique_ptr<IRenderer> StartBackend(Configuration *config) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::Implementation>();
        case Backend::Vulkan:
            return std::make_unique<Vulkan::Implementation>();
        case Backend::Metal:
            spdlog::error("Metal is not implemented!");
            exit(422);
        case Backend::None:
            break;
    }

    throw std::runtime_error("This should never happen.");
}

IShader *IShader::Create(AppCore *app, const std::string &name) {
    IShader *out;
    switch (app->config->rendering_api) {
        case Backend::OpenGL:
            out = new OpenGL::GLShader(name);
            break;
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan shaders are not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal shaders not implemented!");
        case Backend::None:
        default:
            throw std::runtime_error("Invalid state when creating shader!");
    }

    app->renderer->shaders.push_back(out);
    return out;
}

IModel *IModel::Create(Configuration *config, std::string path, bool gamma) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return new OpenGL::GlModel(path, gamma);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan models are not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal models are not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("This should never happen.");
}

std::unique_ptr<Floor> Floor::Create(Configuration *config) {
    std::unique_ptr<Floor> out;
    switch (config->rendering_api) {
        case Backend::OpenGL:
            out = std::make_unique<OpenGL::GlFloor>();
            break;
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    out->Setup();
    return out;
}
}
