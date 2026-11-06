#include "../core/Config.hpp"
#include "IRenderer.hpp"
#include "opengl/RendererOpenGL.hpp"

#include <stdexcept>

#include "opengl/GlModel.hpp"
#include "spdlog/spdlog.h"
#include "vulkan/RendererVulkan.hpp"

namespace Luna {
std::unique_ptr<IRenderer> StartBackend(Configuration *config) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return std::make_unique<OpenGL::Implementation>(config);
        case Backend::Vulkan:
            return std::make_unique<Vulkan::Implementation>(config);
        case Backend::Metal:
            spdlog::error("Metal is not implemented!");
            exit(422);
        case Backend::None:
            break;
    }

    throw std::runtime_error("This should never happen.");
}

IShader *IShader::Create(Configuration *config, const std::string &name) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return new OpenGL::GLShader(name);
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("This should never happen.");
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

    throw std::runtime_error("This should never happen.");
}

Floor* Floor::Create(Configuration *config) {
    switch (config->rendering_api) {
        case Backend::OpenGL:
            return new OpenGL::GlFloor();
        case Backend::Vulkan:
            throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
            break;
    }

    throw std::runtime_error("This should never happen.");
}
}
