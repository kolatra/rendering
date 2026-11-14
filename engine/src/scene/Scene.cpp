#include "graphics/Renderer.hpp"
#include "scene/Scene.hpp"

namespace Luna {
    Scene::Scene(std::shared_ptr<Renderer> renderer) {
        this->renderer = renderer;
    }

    Scene::~Scene() {
        for (const auto &object : objects) {
            object->model.reset();
        }
    }

    void Scene::Update(float dt) {
        for (const auto &object : objects) {
            object->Tick();
        }
        camera->Update(dt);
    }

    void Scene::Render(float dt) {
        for (const auto &object : objects) {
            object->Draw(renderer);
        }
    }

    void Renderer::LoadScene(Scene *scene) {
        // i don't know yet how i want to even store scenes so this is a big WIP
        // for now i'll just make it 1:1 with the original demo scene
        this->scenes.push_back(scene);
        this->currentScene = scene;
    }
}
