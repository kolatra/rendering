#pragma once

#include <engine/platform/IWindow.hpp>
#include <tomlplusplus/toml.hpp>

namespace Luna {
    struct Configuration {
        Backend rendering_api = Backend::OpenGL;
        const char *name = "Luna Application";
        uint32_t width = 1280;
        uint32_t height = 720;

        bool Load();
    private:
        toml::parse_result config;
    };
}
