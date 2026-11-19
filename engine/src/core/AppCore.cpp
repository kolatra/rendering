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

        app->sceneManager = std::make_unique<SceneManager>();
        auto controls = true;
        app->sceneManager->CreateScene(*app, controls);

        app->window->Show();
        // TODO: ew
        app->currentScene = app->sceneManager->Scenes()[0].get();
        app->renderer->currentScene = app->currentScene;

        return app;
    }

    int AppCore::Run() {
        currentScene->camera = std::make_unique<EngineCamera>(config, window.get());
        window->mouseMovement = currentScene->controlsEnabled;

        while (!window->ShouldClose()) {
            UpdateTime();
            currentScene->Update(renderer->deltaTime);

            auto pos = currentScene->camera->position;
            window->SetTitle(std::format("x: {} y: {} z: {}", pos.x, pos.y, pos.z));

            renderer->beginFrame();
            currentScene->Render(renderer->deltaTime);
            renderer->endFrame();

            window->SwapBuffers();
            window->PollEvents();
        }
        return 0;
    }

    void AppCore::UpdateTime() {
        currentTime = window->GetTime();
        renderer->deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
    }

    void Window::UpdateMouse(double x, double y) {
        if (!mouseMovement) return;
        app->currentScene->camera->UpdateMouse(app, x, y);
    }

    void AppCore::PresentScene(const std::unique_ptr<Scene> &scene) {
        // i don't know yet how i want to even store scenes so this is a big WIP
        currentScene = scene.get();
    }

    Scene *AppCore::ClearActiveScene() {
        auto scene = std::move(currentScene);
        currentScene = nullptr;
        return scene;
    }

}
