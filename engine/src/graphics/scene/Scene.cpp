#include "graphics/Renderer.hpp"
#include "graphics/types/Shader.hpp"
#include "graphics/scene/Scene.hpp"
#include "core/AppCore.hpp"
#include "spdlog/spdlog.h"

namespace Luna {
    Scene::Scene(AppCore &app, bool controls) :
    controlsEnabled(controls),
    renderer(*app.renderer),
    config(app.config) {
        camera = std::make_unique<EngineCamera>(config, app.window.get());
        // the default shader included in every scene for basic triangle rendering with textures
        auto sh = Shader::Create("triangle");
        shaders.push_back(std::move(sh));
        spdlog::info("Scene loaded");
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

        camera->Update(dt, controlsEnabled);
    }

    void Scene::Render(float dt) {
        for (const auto &object : objects) {
            object->PreRender(dt);
            object->Draw(renderer);
        }
    }

    void GameObject::Draw(Renderer &renderer) const {
        shader->use();
        shader->setMat4("model", position);
        shader->setMat4("view", SceneManager::CurrentScene().camera->view);
        shader->setMat4("projection", SceneManager::CurrentScene().camera->projection);

        if (culling)
            renderer.setCulling(true);
        this->model->Draw(*shader);
    }
}
