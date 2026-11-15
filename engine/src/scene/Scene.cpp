#include "graphics/Renderer.hpp"
#include "scene/Scene.hpp"
#include "core/AppCore.hpp"
#include "graphics/Text.hpp"
#include "spdlog/spdlog.h"

namespace Luna {
    Scene::Scene(const std::unique_ptr<AppCore> &app, bool controls) :
    controlsEnabled(controls),
    renderer(app->renderer.get()),
    config(app->config.get()) {
        camera = std::make_unique<EngineCamera>(*config, app->window.get());
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
