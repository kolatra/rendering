#pragma once

#include <filesystem>

#include <tomlplusplus/toml.hpp>
#include <spdlog/fmt/fmt.h>

#include "Utilities.hpp"

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

template<>
struct fmt::formatter<Backend> : formatter<std::string> {
    auto format(Backend backend, format_context &ctx) const -> decltype(ctx.out()) {
        switch (backend) {
            case Backend::OpenGL:
                return format_to(ctx.out(), "OpenGL");
            case Backend::Vulkan:
                return format_to(ctx.out(), "Vulkan");
            case Backend::Metal:
                return format_to(ctx.out(), "Metal");
            default:
                return format_to(ctx.out(), "None");
        }
    }
};

namespace Luna {
struct Configuration {
    Backend rendering_api = Backend::OpenGL;
    const char *name = "beep boop";
    uint32_t startingWidth = 1280;
    uint32_t startingHeight = 720;

    explicit Configuration(bool storeDefaults);
    Configuration() = default;

    bool Load();
    bool Save(std::filesystem::path &path) const;
    void Print();
private:
    toml::table config;
    [[nodiscard]] toml::table to_toml() const;
};
}
