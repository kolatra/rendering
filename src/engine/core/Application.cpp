#include <engine/core/Application.hpp>
#include <engine/platform/WindowGLFW.hpp>
#include <engine/graphics/opengl/RendererOpenGL.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

#ifdef __APPLE__
#include <engine/graphics/metal/MetalRenderer.hpp>
#endif

namespace Luna {
    Application::Application() {
        config = std::make_shared<Configuration>();
        config->Load();

        window = std::make_unique<WindowGLFW>(config, this);
        std::cout << "Window created.\n";

        switch (config->rendering_api) {
            case Backend::OpenGL:
                renderer = std::make_unique<OpenGL::Implementation>(config.get());
                break;
            case Backend::Vulkan:
                throw std::runtime_error("Vulkan is not implemented");
            case Backend::Metal:
#ifdef __APPLE__
                renderer = std::make_unique<Metal::Implementation>();
                break;
#else
                spdlog::error("Metal is not available on this device");
                exit(1);
#endif
            default:
                throw std::runtime_error("Invalid state!");
        }

        renderer->init(window.get(), config.get());
        std::cout << "Renderer created.\n";
    }
}


int main() {
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
