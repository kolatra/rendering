#pragma once

#include <tomlplusplus/toml.hpp>
#include <fmt/core.h>
#include <filesystem>

#include "Utilities.hpp"

namespace Luna {
enum class Backend {
    None,
    OpenGL,
    Vulkan,
    Metal
};

inline Backend be_from_string(const std::string& in) {
    if (Util::ToLower(in) == "opengl") {
        return Backend::OpenGL;
    }

    if (Util::ToLower(in) == "vulkan") {
        return Backend::Vulkan;
    }

    if (Util::ToLower(in) == "metal") {
        return Backend::Metal;
    }

    // Default
    return Backend::OpenGL;
}

inline std::string be_to_string(const Backend backend) {
    switch (backend) {
        case Backend::OpenGL:
            return "OpenGL";
        case Backend::Vulkan:
            return "Vulkan";
        case Backend::Metal:
            return "Metal";
        default:
            return "None";
    }
}

struct Configuration {
    Backend rendering_api = Backend::OpenGL;
    const char *name = "beep boop";
    uint32_t startingWidth = 1280;
    uint32_t startingHeight = 720;
    bool alwaysLog = true;

    explicit Configuration(bool storeDefaults);
    Configuration() = default;

    bool Load();
    bool Save(std::filesystem::path &path) const;
private:
    toml::table config;
    [[nodiscard]] toml::table to_toml() const;
};
}
