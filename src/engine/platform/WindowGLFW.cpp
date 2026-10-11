#include <glad/gl.h>
#include <exception>
#include "../Application.hpp"
#include "WindowGLFW.hpp"
#include <iostream>

namespace {
    void keys(GLFWwindow* window, int key, int scancode, int action, int mods) {

    }

    void resize(GLFWwindow *window, int32_t w, int32_t h) {
        glViewport(0, 0, w, h);
    }

    void cursor(GLFWwindow* window, double x, double y) {

    }

    void mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) {

    }

    void errors(int32_t error, const char *description) {
        std::fprintf(stderr, "Code: %d\nError: %s\n", error, description);
    }
}

WindowGLFW::WindowGLFW(Backend backend, int width, int height, std::string& title) {
    this->rendering_backend = backend;
    if (!glfwInit()) {
        std::cout << "Couldn't load glfw!\n";
        throw std::exception();
    }

    if (backend == Backend::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!handle) {
            glfwTerminate();
            throw std::exception();
        }

        glfwSetErrorCallback(errors);
        glfwSetFramebufferSizeCallback(handle, resize);
        glfwSetKeyCallback(handle, keys);
        glfwSetCursorPosCallback(handle, cursor);
        glfwSetMouseButtonCallback(handle, mouse);

        glfwMakeContextCurrent(handle);
        glfwSwapInterval(0);

        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cout << "Couldn't load OpenGL with glad!\n";
            throw std::exception();
        }
    } else {
        // Vulkan/Metal etc.
        throw std::exception("Unsupported backend!");
    }
}

WindowGLFW::~WindowGLFW() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void *WindowGLFW::CreateSurface(void *apiContext) {
    if (backend == Backend::OpenGL) {
        // glfw already handles this when the window is created for OpenGL contexts
        return nullptr;
    }

    return nullptr;
}
