#include <engine/core/Application.hpp>
#include <engine/platform/WindowGLFW.hpp>
#include <engine/graphics/opengl/RendererOpenGL.hpp>
#include <engine/core/Config.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

namespace Luna {
    Application::Application() {
        config = std::make_shared<Configuration>();
        config->Load();

        switch (config->rendering_api) {
            case Backend::OpenGL:
                window = std::make_unique<WindowGLFW>(config, this);
                renderer = std::make_unique<OpenGL::Implementation>(config.get());
                break;
            case Backend::Vulkan:
                spdlog::error("Vulkan is not implemented");
            case Backend::Metal:
                spdlog::error("Metal is not implemented");
            default:
                throw std::runtime_error("Invalid state!");
        }

        renderer->init(window.get(), config.get());
        spdlog::info("Renderer initialized.");
    }
}

int main() {
    spdlog::set_level(spdlog::level::trace);

    auto *app = new Luna::Application();
    const auto &window = app->window;
    const auto &renderer = app->renderer;
    app->Setup();

    auto *floor = new Luna::OpenGL::Floor();
    floor->Setup();
    auto last_time = window->GetTime();

    window->Show();
    while (!window->ShouldClose()) {
        app->currentTime = window->GetTime();
        renderer->deltaTime = static_cast<float>(app->currentTime - last_time);
        renderer->camera->Update(*window, renderer->deltaTime);

        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        app->Loop();

        // disable back-face culling for the floor render
        renderer->setCulling(false);
        floor->Render(*renderer->camera);

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
        last_time = window->GetTime();
    }
    delete app;
    spdlog::info("Exited successfully!");
    return 0;
}
