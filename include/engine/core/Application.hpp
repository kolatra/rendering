#pragma once

#include <memory>

#include "../graphics/IRenderer.hpp"
#include "../platform/IWindow.hpp"

namespace Luna {
    struct Configuration {
        Backend rendering_api = Backend::None;
        const char *name = "Luna Application";
        uint32_t width;
        uint32_t height;

        bool Load();
    };

    class Application {
    public:
        std::shared_ptr<Configuration> config;
        std::unique_ptr<IWindow> window;
        std::unique_ptr<IRenderer> renderer;
        // Implemented in your main.cpp to run the app
        void Run() const;
    };

    Application* CreateApp(uint32_t width, uint32_t height);
}

