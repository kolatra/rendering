#pragma once

#include <memory>

namespace Luna {
    struct Configuration;
    class IWindow;
    class IRenderer;

class AppCore {
public:
    std::shared_ptr<Configuration> config;
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<Floor> floor;

    double currentTime = 0;
};

inline std::tuple<std::shared_ptr<Configuration> &, std::unique_ptr<IWindow> &, std::unique_ptr<IRenderer> &>
GetHandles(std::unique_ptr<AppCore> &app) {
    auto cfg = std::ref(app->config);
    auto win = std::ref(app->window);
    auto ren = std::ref(app->renderer);
    return std::make_tuple(cfg, win, ren);
}

std::unique_ptr<AppCore> Initialize();
}
