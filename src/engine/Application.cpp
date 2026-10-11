#include "Application.hpp"

#include <iostream>

#include "graphics/Model.hpp"
#include "graphics/opengl/RendererOpenGL.hpp"
#include "platform/WindowGLFW.hpp"

Application::Application(uint32_t width, uint32_t height, std::string &title) {
    // TODO: Default for now, configurable somehow later.
    rendering_backend = Backend::OpenGL;

    window = std::make_unique<WindowGLFW>(rendering_backend, width, height, title);
    std::cout << "Window created.\n";

    switch (rendering_backend) {
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

void Application::run() {
    GLShader shader("shaders/triangle.vsh", "shaders/triangle.fsh");
    Model penger("penger-obj-main/penger/penger.obj", false);

    while (!window->ShouldClose()) {
        renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

        penger.Draw(shader);

        window->SwapBuffers();
        window->PollEvents();
    }
}
