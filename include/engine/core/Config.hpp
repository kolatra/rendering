#pragma once

#include <tomlplusplus/toml.hpp>

namespace Luna {
enum class Backend {
    None,
    OpenGL,
    Vulkan,
    Metal
};

inline Backend be_from_string(const std::string& in) {
    if (in == "OpenGL") {
        return Backend::OpenGL;
    }

    if (in == "Vulkan") {
        return Backend::Vulkan;
    }

    if (in == "Metal") {
        return Backend::Metal;
    }

    // Default
    return Backend::OpenGL;
}

struct Configuration {
    Backend rendering_api;
    const char *name = "Luna Application";
    uint32_t width = 1280;
    uint32_t height = 720;

    bool Load();
private:
    toml::parse_result config;
};
}
