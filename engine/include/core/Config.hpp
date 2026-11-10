#pragma once

#include <tomlplusplus/toml.hpp>
#include <spdlog/spdlog.h>
#include <filesystem>

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

inline std::string level_to_string(spdlog::level::level_enum level) {
    switch (level) {
        case spdlog::level::trace:
            return "trace";
        case spdlog::level::debug:
            return "debug";
        case spdlog::level::info:
            return "info";
        case spdlog::level::warn:
            return "warn";
        case spdlog::level::err:
            return "err";
        case spdlog::level::critical:
            return "critical";
        default:
            return "None";
    }
}

struct Configuration {
    Backend rendering_api = Backend::OpenGL;
    const char *name = "beep boop";
    uint32_t startingWidth = 1280;
    uint32_t startingHeight = 720;
    bool showFloor = true;
    bool alwaysLog = true;
    spdlog::level::level_enum logLevel = spdlog::level::trace;
    explicit Configuration(bool storeDefaults);

    bool Load();
    bool Save(std::filesystem::path &path) const;
    void Print();
private:
    toml::table config;
    [[nodiscard]] toml::table to_toml() const;
};
}
