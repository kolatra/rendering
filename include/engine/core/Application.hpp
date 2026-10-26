#pragma once

#include <memory>

namespace Luna {
    struct Configuration;
    class IWindow;
    class IRenderer;

class Application {
public:
    std::shared_ptr<Configuration> config;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;

    double currentTime = 0;

    explicit Application();

    // Initialize needed objects for runtime, setup app, etc.
    void Setup() const;

    // Called every frame by the engine.
    void Loop() const;
};
}

