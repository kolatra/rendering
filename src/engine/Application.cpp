#include "Application.hpp"
#include "graphics/Model.hpp"
#include "graphics/opengl/RendererOpenGL.hpp"
#include "platform/glfw/WindowGLFW.hpp"
#include <iostream>

#include "objects/Penger.hpp"

bool Configuration::Load() {
    return false;
}

Application::Application(uint32_t width, uint32_t height, std::string &title) {
    config = std::make_unique<Configuration>();
    config->window_size.width = width;
    config->window_size.height = height;
    config->rendering_api = Backend::OpenGL;
    config->Load();

    window = std::make_unique<WindowGLFW>(*config, title, this);
    std::cout << "Window created.\n";

    switch (config->rendering_api) {
        case Backend::OpenGL:
            renderer = std::make_unique<RendererOpenGL>();
            break;
        case Backend::Vulkan:
            throw std::exception("Vulkan is not implemented!");
        case Backend::Metal:
            throw std::exception("Metal is not implemented!");
        default:
            throw std::exception("Invalid state!");
    }

    renderer->init(window.get(), width, height);
    std::cout << "Renderer created.\n";
}

void Application::Run() {
    GLShader shader("shaders/triangle.vsh", "shaders/triangle.fsh");
    Penger penger;

    window->Show();
    while (!window->ShouldClose()) {
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        penger.Draw(shader);

        window->SwapBuffers();
        window->PollEvents();
    }
}
