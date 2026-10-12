#include <glad/gl.h>
#include "../../Application.hpp"
#include "../glfw/WindowGLFW.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <exception>
#include <iostream>

class Application;

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

WindowGLFW::WindowGLFW(Backend backend, int width, int height, std::string& title, Application *app) {
    this->backend = backend;
    if (!glfwInit()) {
        std::cout << "Couldn't load glfw!\n";
        throw std::exception();
    }

    if (backend == Backend::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    } else if (backend == Backend::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        throw std::exception("not implemented");
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!handle) {
        glfwTerminate();
        throw std::exception();
    }
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(0);

    if (backend == Backend::OpenGL) {
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
    if (backend == Backend::OpenGL) {
        // glfw already handles this when the window is created for OpenGL contexts
        return nullptr;
    }

    if (backend == Backend::Vulkan) {
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
