#include <glad/gl.h>
#include "../../../include/engine/core/Application.hpp"
#include "engine/platform/WindowGLFW.hpp"

#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>

namespace Luna {
static void keys(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

static void resize(GLFWwindow *window, int32_t w, int32_t h) {
    glViewport(0, 0, w, h);
}

static void cursor(GLFWwindow* window, double x, double y) {

}

static void mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) {

}

static void errors(int32_t error, const char *description) {
    std::fprintf(stderr, "Code: %d\nError: %s\n", error, description);
}

WindowGLFW::WindowGLFW(const std::shared_ptr<Configuration> &config, Application *app) {
    this->config = config;
    const auto width = config->width;
    const auto height = config->height;

    if (!glfwInit()) {
        std::cout << "Couldn't load glfw!\n";
        throw std::exception();
    }

    if (config->rendering_api == Backend::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    } else if (config->rendering_api == Backend::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        throw std::runtime_error("not implemented");
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, config->name, nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        throw std::exception();
    }

    if (config->rendering_api == Backend::OpenGL) {
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(0);
        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cout << "Couldn't load OpenGL with glad!\n";
            glfwTerminate();
            throw std::exception();
        }
    }

    glfwSetErrorCallback(errors);
    glfwSetFramebufferSizeCallback(handle, resize);
    glfwSetKeyCallback(handle, keys);
    glfwSetCursorPosCallback(handle, cursor);
    glfwSetMouseButtonCallback(handle, mouse);
    glfwSetWindowUserPointer(handle, app);
}

WindowGLFW::~WindowGLFW() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void *WindowGLFW::CreateSurface(void *apiContext) {
    if (config->rendering_api == Backend::OpenGL) {
        // glfw already handles this when the window is created for OpenGL contexts
        return nullptr;
    }

    if (config->rendering_api == Backend::Vulkan) {
        // create surface and other shit and return
    }

    return nullptr;
}

void WindowGLFW::Show() {
    glfwShowWindow(handle);
}

void WindowGLFW::PollEvents() {
    glfwPollEvents();
}

void WindowGLFW::SwapBuffers() {
    glfwSwapBuffers(handle);
}

void *WindowGLFW::GetNativeHandle() {
    return handle;
}

bool WindowGLFW::ShouldClose() {
    return glfwWindowShouldClose(handle);
}

double WindowGLFW::GetTime() {
    return glfwGetTime();
}
}
