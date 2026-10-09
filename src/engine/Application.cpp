#include "Application.hpp"

#include <iostream>

#include "graphics/opengl/RendererOpenGL.hpp"
#include "platform/WindowGLFW.hpp"

void Application::run(uint32_t width, uint32_t height) {
    window = std::make_unique<WindowGLFW>(rendering_backend, width, height);
    std::cout << "Window created.\n";

    switch (rendering_backend) {
        case OpenGL:
            renderer = std::make_unique<RendererOpenGL>();
            break;
        case Vulkan:
            throw std::exception("Vulkan is not implemented!");
        case Metal:
            throw std::exception("Metal is not implemented!");
        default:
            throw std::exception("Invalid state!");
    }

    renderer->init(window.get(), width, height);
    std::cout << "Renderer created.\n";

    while (!window->ShouldClose()) {
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        window->SwapBuffers();
        window->PollEvents();
    }
}
