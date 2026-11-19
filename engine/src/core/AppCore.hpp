#pragma once

#include <memory>

#include "graphics/Renderer.hpp"
#include "graphics/scene/Scene.hpp"
#include "Config.hpp"

namespace Luna {
class AppCore;
struct Configuration;
struct Window;
struct SceneManager;

std::unique_ptr<AppCore> Initialize(int argc, char **argv);

// Set after config is loaded
inline auto ACTIVE_BACKEND = Backend::None;

class AppCore {
public:
    Configuration config;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SceneManager> sceneManager;
    Scene *currentScene;

    int Run();

    double currentTime = 0;
    double lastTime = 0;
    void UpdateTime();

    void PresentScene(const std::unique_ptr<Scene> &scene);
    Scene *ClearActiveScene();
};
}
