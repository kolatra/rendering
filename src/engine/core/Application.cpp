#include <engine/core/Application.hpp>
#include <engine/platform/WindowGLFW.hpp>
#include <engine/graphics/opengl/RendererOpenGL.hpp>
#include <iostream>

namespace Luna {
    Application *CreateApp(uint32_t width, uint32_t height) {
        const auto config = std::make_shared<Configuration>();
        config->width = width;
        config->height = height;
        config->Load();

        auto *app = new Application();
        app->config = config;

        app->window = std::make_unique<WindowGLFW>(config, app);
        std::cout << "Window created.\n";

        switch (config->rendering_api) {
            case Backend::OpenGL:
                app->renderer = std::make_unique<OpenGL::Implementation>();
                break;
            case Backend::Vulkan:
                throw std::exception("Vulkan is not implemented!");
            case Backend::Metal:
                throw std::exception("Metal is not implemented!");
            default:
                throw std::exception("Invalid state!");
        }

        app->renderer->init(app->window.get(), width, height);
        std::cout << "Renderer created.\n";
        return app;
    }
}

int main() {
    const auto *app = Luna::CreateApp();
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
