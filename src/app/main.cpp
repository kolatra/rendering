#include "engine/Application.hpp"
#include "engine/graphics/Model.hpp"
#include "engine/graphics/opengl/RendererOpenGL.hpp"
#include "engine/platform/WindowGLFW.hpp"
#include <iostream>

#include "../engine/objects/Penger.hpp"

namespace Luna {
bool Configuration::Load() {
    // TODO: implement loading config from a file on disk
    return false;
}

Application *CreateApp(uint32_t width, uint32_t height) {
    const auto config = std::make_shared<Configuration>();
    config->width = width;
    config->height = height;
    config->rendering_api = Backend::OpenGL;
    config->Load();

    auto *app = new Application();
    app->config = config;

    app->window = std::make_unique<WindowGLFW>(config, app);
    std::cout << "Window created.\n";

    switch (config->rendering_api) {
        case Backend::OpenGL:
            app->renderer = std::make_unique<RendererOpenGL>();
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

void Application::Run() const {
    GLShader shader("shaders/triangle.vsh", "shaders/triangle.fsh");
    Penger penger;

    window->Show();
    while (!window->ShouldClose()) {
        renderer->beginFrame();
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        penger.Draw(shader);

        window->SwapBuffers();
        window->PollEvents();
        renderer->endFrame();
    }
}
}

int main() {
    const auto *app = Luna::CreateApp(1280, 720);
    app->Run();

    return 0;
}
