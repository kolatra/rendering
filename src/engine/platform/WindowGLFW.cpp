#include "WindowGLFW.hpp"

#include <exception>
#include <print>

#include "../Application.hpp"
#include "glad/gl.h"

static GLFWwindow *create_vulkan_window(int width, int height) {
    if (!glfwVulkanSupported()) {
        throw std::exception("Vulkan is not supported on this system!");
    }

    const auto handle = glfwCreateWindow(width, height, "GLFW Vulkan Window", nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        throw std::exception();
    }

    return handle;
}

static GLFWwindow *create_opengl_window(int width, int height) {
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::exception("Could not load GL with glad!");
    }

    const auto handle = glfwCreateWindow(width, height, "GLFW Window", nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        throw std::exception();
    }

    glfwMakeContextCurrent(handle);
    glfwSwapInterval(0);
    return handle;
}

WindowGLFW::WindowGLFW(Backend backend, int width, int height) {
    glfwInit(); // TODO: error check

    if (backend == OpenGL) {
        handle = create_opengl_window(width, height);
    } else if (backend == Vulkan) {
        handle = create_vulkan_window(width, height);
    } else {
        throw std::exception("Unsupported backend!");
    }
}

WindowGLFW::~WindowGLFW() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void WindowGLFW::SetMouseGrab(const bool flag) {
    glfwSetInputMode(handle, GLFW_CURSOR, flag ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
