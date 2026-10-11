#pragma once

#include <memory>

#include "graphics/IRenderer.hpp"
#include "platform/IWindow.hpp"

class Application {
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    Backend rendering_backend = Backend::None;
public:
    Application(uint32_t width, uint32_t height, std::string& title);
    void run();
    void setBackend(const Backend choice) { rendering_backend = choice; }
};

