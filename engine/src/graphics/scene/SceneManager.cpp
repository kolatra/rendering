#include "Scene.hpp"

namespace Luna {
    void SceneManager::AddScene(std::unique_ptr<Scene> scene) {
        scenes.push_back(std::move(scene));
    }

    std::vector<std::unique_ptr<Scene> > &SceneManager::Scenes() {
        return scenes;
    }

    Scene& SceneManager::SelectScene(int index) {
        return *scenes[index];
    }

    Scene &SceneManager::CreateScene(AppCore &app, bool controls) {
        auto scene = std::make_unique<Scene>(app, controls);
        auto &ref = *scene;
        scenes.push_back(std::move(scene));
        return ref;
    }
}
