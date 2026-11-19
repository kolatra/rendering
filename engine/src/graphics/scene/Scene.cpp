#include "graphics/Renderer.hpp"
#include "graphics/scene/Scene.hpp"
#include "core/AppCore.hpp"
#include "graphics/Text.hpp"
#include "spdlog/spdlog.h"

namespace Luna {
    Scene::Scene(AppCore &app, bool controls) :
    controlsEnabled(controls),
    renderer(*app.renderer),
    config(app.config) {
        camera = std::make_unique<EngineCamera>(config, app.window.get());
        // the default shader included in every scene for basic triangle rendering with textures
        shaders.push_back(Shader::Create("triangle"));
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
}
