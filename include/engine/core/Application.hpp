#pragma once

#include <memory>

#include "../graphics/IRenderer.hpp"
#include "../platform/IWindow.hpp"

namespace Luna {
    struct Configuration {
        Backend rendering_api = Backend::OpenGL;
        const char *name = "Luna Application";
        uint32_t width = 1280;
        uint32_t height = 720;

        bool Load();
    };

    class Application {
    public:
        std::shared_ptr<Configuration> config;
        std::unique_ptr<IWindow> window;
        std::unique_ptr<IRenderer> renderer;

        explicit Application();

        // Initialize needed objects for runtime and setup app etc.
        void Setup() const;

        // Called every frame by the engine.
        void Loop() const;
    };
}

