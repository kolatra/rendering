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
                renderer = std::make_unique<OpenGL::Implementation>();
                break;
            case Backend::Vulkan:
                throw std::runtime_error("Vulkan is not implemented");
            case Backend::Metal:
#ifndef __APPLE__
                spdlog::error("Metal is not available on this device");
#else
                renderer = std::make_unique<Metal::Implementation>();
                break;
#endif
            default:
                throw std::runtime_error("Invalid state!");
        }

        renderer->init(window.get(), config.get());
        std::cout << "Renderer created.\n";
    }
}

#ifndef __APPLE__ // MetalApp.m
int main() {
    auto *app = new Luna::Application();
    const auto &window = app->window;
    const auto &renderer = app->renderer;
    app->Setup();

    auto *floor = new Luna::OpenGL::Floor();
    floor->Setup();

    window->Show();
    while (!window->ShouldClose()) {
        app->currentTime = window->GetTime();

        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        app->Loop();
        floor->Render();

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }
    delete app;
    return 0;
}
#endif