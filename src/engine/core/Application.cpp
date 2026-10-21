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
    const auto *app = new Luna::Application();
    const auto &window = app->window;
    const auto &renderer = app->renderer;
    app->Setup();

    std::cout << "Showing window now with " << renderer->pengers.size() << " render objects.\n";
    window->Show();
    while (!window->ShouldClose()) {
        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        app->Loop();

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }
    return 0;
}
