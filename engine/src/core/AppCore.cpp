#include "spdlog/spdlog.h"
#include "platform/Window.hpp"
#include "graphics/Renderer.hpp"
#include "core/AppCore.hpp"
#include "graphics/scene/Scene.hpp"

#include "graphics/Text.hpp"

namespace Luna {
    std::unique_ptr<AppCore> Initialize(int argc, char **argv) {
        bool storeDefaultConfig = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--save-config") == 0) {
                storeDefaultConfig = true;
            }
        }
        auto config = Configuration(storeDefaultConfig);
        ACTIVE_BACKEND = config.rendering_api;

        spdlog::trace("creating app core");
        auto app = std::make_unique<AppCore>();
        app->config = std::move(config);

        spdlog::trace("creating window");
        app->window = CreateWindow(app);
        app->window->app = app.get();

        spdlog::trace("initializing rendering");
        app->renderer = StartGraphics();
        app->renderer->init(app->window.get());
        FontRenderer::Init();
        app->floor = Floor::Create();

        app->window->Show();
        // At this point we could have a loading screen scene of some kind
        // just with a texture to start but maybe text later and in a dream a progress bar

        return app;
    }

    Scene &AppCore::CreateScene(bool controls) {
        auto scene = std::make_unique<Scene>(*this, controls);
        auto &ref = *scene;
        PresentScene(std::move(scene));
        return ref;
    }

    int AppCore::Run() {
        if (!renderer->currentScene) {
            spdlog::error("Current scene not set! Scenes were not loaded correctly!");
            throw std::runtime_error("Scene not loaded correctly!");
        }
        renderer->currentScene->camera = std::make_unique<EngineCamera>(config, window.get());
        window->mouseMovement = renderer->currentScene->controlsEnabled;

        while (!window->ShouldClose()) {
            currentTime = window->GetTime();
            renderer->deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;

            renderer->currentScene->Update(renderer->deltaTime);

            auto pos = renderer->currentScene->camera->position;
            window->SetTitle(std::format("x: {} y: {} z: {}", pos.x, pos.y, pos.z));

            {
                renderer->beginFrame();

                renderer->currentScene->Render(renderer->deltaTime);

                if (config.showFloor) {
                    renderer->setCulling(false);
                    floor->Render(*renderer->currentScene->camera);
                }

                renderer->endFrame();
            }

            window->SwapBuffers();
            window->PollEvents();
        }
        return 0;
    }

    void Window::UpdateMouse(double x, double y) {
        if (!mouseMovement) return;
        app->renderer->currentScene->camera->UpdateMouse(app, x, y);
    }

    void AppCore::PresentScene(std::unique_ptr<Scene> scene) {
        // i don't know yet how i want to even store scenes so this is a big WIP
        renderer->currentScene = std::move(scene);
    }

    std::unique_ptr<Scene> AppCore::ClearActiveScene() {
        auto scene = std::move(renderer->currentScene);
        renderer->currentScene = nullptr;
        return scene;
    }

}
