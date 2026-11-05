#include "../core/Config.hpp"
#include "IRenderer.hpp"
#include "opengl/RendererOpenGL.hpp"

#include <stdexcept>

#include "opengl/GlModel.hpp"

namespace Luna {
std::unique_ptr<IRenderer> StartBackend(Configuration *config) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::Implementation>(config);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("No renderer backend was selected.");
}

IShader *IShader::Create(Configuration *config, const std::string &vertex, const std::string &fragment) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return new OpenGL::GLShader(vertex, fragment);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("No renderer backend was selected.");
}

IModel *IModel::Create(Configuration *config, std::string path, bool gamma) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return new OpenGL::GlModel(path, gamma);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("No renderer backend was selected.");
}
}
