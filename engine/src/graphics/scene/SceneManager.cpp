#include "core/AppCore.hpp"
#include "Scene.hpp"

namespace Luna::SceneManager {
    static std::vector<std::unique_ptr<Scene>> scenes;
    static std::unique_ptr<Scene> currentScene;

    void Startup() {

    }

    void Shutdown() {
        currentScene.reset();
        scenes.clear();
    }

    bool SelectScene(int i) {
        if (i >= 0 && i < scenes.size()) {
            currentScene = std::move(scenes[i]);
            return true;
        } else {
            return false;
        }
    }

    std::vector<std::unique_ptr<Scene>> &Scenes() {
        return scenes;
    }

    EngineCamera &Camera() {
        if (!currentScene) {
            throw std::runtime_error("Camera Ref: No scene loaded");
        }
        return *currentScene->camera;
    }

    Scene& CurrentScene() {
        return *currentScene;
    }

    Scene &CreateScene(AppCore &app, bool controls) {
        auto scene = std::make_unique<Scene>(app, controls);
        scene->camera = std::make_unique<EngineCamera>(app.config, app.window.get());
        app.window->mouseMovement = scene->controlsEnabled;

        auto &ref = *scene;
        scenes.push_back(std::move(scene));
        if (!currentScene) {
            currentScene = std::move(scene);
        }
        return ref;
    }

    void Update(float dt) {
        currentScene->Update(dt);
    }

    void Render() {
        currentScene->Render();
    }
}
