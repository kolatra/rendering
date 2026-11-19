#pragma once

#include <shaderc/shaderc.hpp>

namespace Luna::SpirVCompiler {
    void Init();
    void Destroy();
    shaderc::CompilationResult<uint32_t> Compile(const std::string &filename, shaderc_shader_kind kind);
}
