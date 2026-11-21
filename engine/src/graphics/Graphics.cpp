#include <stdexcept>

#include <fmt/core.h>

#include "graphics/Renderer.hpp"
#include "graphics/impl/opengl/GlModel.hpp"
#include "graphics/impl/opengl/RendererOpenGL.hpp"
#include "graphics/impl/vulkan/RendererVulkan.hpp"
#include "LunaEngine.hpp"
#include "impl/SpirVCompiler.hpp"

namespace Luna {
Renderer::Renderer() = default;
Renderer::~Renderer() = default;

std::unique_ptr<Renderer> StartGraphics() {
    fmt::println("Starting backend...");
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL: return std::make_unique<OpenGL::Implementation>();
        case Backend::Vulkan: return std::make_unique<Vulkan::Implementation>();
        case Backend::Metal:  throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
        default:              throw std::runtime_error("StartGraphics: This should never happen.");
    }
}

std::unique_ptr<Shader> Shader::Create(const std::string &name) {
    std::string v = std::format("shaders/{}.vert", name);
    std::string f = std::format("shaders/{}.frag", name);

    SpirVCompiler::Init();
    auto vertexRes = SpirVCompiler::Compile(v, shaderc_glsl_vertex_shader);
    auto fragmentRes = SpirVCompiler::Compile(f, shaderc_glsl_fragment_shader);

    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL: return std::make_unique<OpenGL::GLShader>(vertexRes, fragmentRes);
        case Backend::Vulkan: return std::make_unique<Vulkan::VShader>(vertexRes, fragmentRes);
        case Backend::Metal:  throw std::runtime_error("Metal shaders not implemented!");
        case Backend::None:
        default:              throw std::runtime_error("Invalid state when creating shader!");
    }
}

std::unique_ptr<Model> Model::Create(std::string path, bool gamma) {
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL: return std::make_unique<OpenGL::GlModel>(path, gamma);
        case Backend::Vulkan: throw std::runtime_error("Vulkan models are not implemented!");
        case Backend::Metal:  throw std::runtime_error("Metal models are not implemented!");
        case Backend::None:
        default:              throw std::runtime_error("Model: Backend was cleared!");
    }
}

std::unique_ptr<Floor> Floor::Create() {
    std::unique_ptr<Floor> out;
    switch (ACTIVE_BACKEND) {
        case Backend::OpenGL: out = std::make_unique<OpenGL::GlFloor>(); break;
        case Backend::Vulkan: throw std::runtime_error("Vulkan is not implemented!");
        case Backend::Metal:  throw std::runtime_error("Metal is not implemented!");
        case Backend::None:
        default:              throw std::runtime_error("Invalid state!");
    }

    out->Setup();
    return out;
}

}
