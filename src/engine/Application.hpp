#pragma once

#include <memory>

#include "graphics/IRenderer.hpp"
#include "platform/IWindow.hpp"

namespace Luna {
struct Configuration {
    Backend rendering_api = Backend::None;

    struct {
        uint32_t width;
        uint32_t height;
    } window_size;

    bool Load();
};

class Application {
    std::unique_ptr<Configuration> config;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
public:
    Application(uint32_t width, uint32_t height, std::string& title);
    void Run() const;
};
}

