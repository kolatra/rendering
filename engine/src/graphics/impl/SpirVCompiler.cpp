#include <spdlog/spdlog.h>

#include "SpirVCompiler.hpp"
#include "Utilities.hpp"
#include "core/AppCore.hpp"

namespace Luna::SpirVCompiler {
    static shaderc::Compiler compiler {};
    static shaderc::CompileOptions options {};
    static bool initialized = false;

    void Init() {
        if (initialized) return;
        if (ACTIVE_BACKEND == Backend::Vulkan) {
            options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
            options.SetTargetSpirv(shaderc_spirv_version_1_6);
            spdlog::debug("SpirV version 1.6 initialized");
        } else if (ACTIVE_BACKEND == Backend::OpenGL) {
            options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
            options.SetTargetSpirv(shaderc_spirv_version_1_0);
            spdlog::debug("SpirV version 1.0 initialized");
        } else {
            throw std::runtime_error("Unknown Backend");
        }
        options.SetOptimizationLevel(shaderc_optimization_level_zero);
        initialized = true;
    }

    void Destroy() {
        if (!initialized) return;
        initialized = false;
    }

    std::vector<uint32_t> Compile(const std::string &filename, shaderc_shader_kind kind) {
        if (!initialized) {
            spdlog::error("Trying to compile a SpirV shader without starting the compiler!");
            throw std::runtime_error("");
        }
        std::string src = Util::ReadFileToString(filename);
        shaderc::CompilationResult result = compiler.CompileGlslToSpv(src, kind, filename.c_str(), options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            spdlog::error("Shader compilation error: {}", result.GetErrorMessage());
            throw;
        }
        std::vector compiled(result.begin(), result.end());
        return compiled;
    }
}
