#pragma once

#include <memory>

namespace Luna {
    struct Configuration;
    class IWindow;
    class IRenderer;

class EngineHeart {
public:
    std::shared_ptr<Configuration> config;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;

    double currentTime = 0;

    explicit EngineHeart();
};

int EngineMain(int argc, char **argv);
}

