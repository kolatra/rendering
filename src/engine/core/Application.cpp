#include <engine/core/Application.hpp>
#include <engine/platform/WindowGLFW.hpp>
#include <engine/graphics/opengl/RendererOpenGL.hpp>
#include <iostream>

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
                throw std::exception("Vulkan is not implemented!");
            case Backend::Metal:
                throw std::exception("Metal is not implemented!");
            default:
                throw std::exception("Invalid state!");
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
