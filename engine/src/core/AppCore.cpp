#include <fmt/core.h>
#include "platform/Window.hpp"
#include "graphics/Renderer.hpp"
#include "core/AppCore.hpp"
#include "graphics/scene/Scene.hpp"

#include "graphics/Text.hpp"

namespace Luna {
    AppCore::~AppCore() {
        SceneManager::Shutdown();
        FontRenderer::Shutdown();
    }

    std::unique_ptr<AppCore> Initialize(int argc, char **argv) {
        bool storeDefaultConfig = false;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--save-config") == 0) {
                storeDefaultConfig = true;
            }
        }
        auto config = Configuration(storeDefaultConfig);
        ACTIVE_BACKEND = config.rendering_api;

        auto app = std::make_unique<AppCore>();
        app->config = std::move(config);

        app->window = CreateWindow(app);
        app->window->app = app.get();

        app->renderer = StartGraphics();
        app->renderer->init(app->window.get());
        FontRenderer::Init();

        auto controls = true;
        SceneManager::Startup();
        SceneManager::CreateScene(*app, controls);
        SceneManager::SelectScene(0);

        app->window->Show();

        return app;
    }

    void AppCore::Run() {
        auto currentTime = window->GetTime();
        auto dt = 0.01;
        auto frameCount = 0;
        auto lastFpsUpdate = currentTime;

        while (!window->ShouldClose()) {
            auto newTime = window->GetTime();
            auto frameTime = newTime - currentTime;
            currentTime = newTime;

            if (currentTime - lastFpsUpdate >= 1) {
                window->SetTitle(std::format("FPS: {}", frameCount));
                lastFpsUpdate = currentTime;
                frameCount = 0;
            }

            while (frameTime > 0.0) {
                float deltaTime = std::min(frameTime, dt);
                SceneManager::Update(deltaTime);
                frameTime -= deltaTime;
            }

            renderer->clear({0.4f, 0.7f, 1.0f, 1.0f});
            renderer->beginFrame();
            SceneManager::Render();
            renderer->endFrame();

            window->SwapBuffers();
            window->PollEvents();
            frameCount++;
        }
    }

    void Window::UpdateMouse(double x, double y) {
        if (!mouseMovement) return;
        SceneManager::Camera().UpdateMouse(app, x, y);
    }
}
