#include "core/AppCore.hpp"
#include "core/Config.hpp"
#include "platform/WindowGLFW.hpp"

#include <glad/gl.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "spdlog/spdlog.h"

namespace Luna {
static void keys(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (const auto app = static_cast<AppCore*>(glfwGetWindowUserPointer(window))) {
        switch (action) {
            case GLFW_PRESS:
                app->window->held_input[key] = true;
                if (key == GLFW_KEY_ESCAPE) {
                    spdlog::debug("releasing mouse");
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    app->mouseCaptured = false;
                }
                break;
            case GLFW_RELEASE:
                app->window->held_input[key] = false;
            default:
                break;
        }
    }
}

static void resize(GLFWwindow *window, int32_t w, int32_t h) {
    glViewport(0, 0, w, h);
}

static void cursor(GLFWwindow* window, double x, double y) {
    if (const auto app = static_cast<AppCore*>(glfwGetWindowUserPointer(window))) {
        app->UpdateMouse(x, y);
    }
}

static void mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods) {
    if (const auto app = static_cast<AppCore*>(glfwGetWindowUserPointer(window))) {
        if (!app->mouseCaptured) {
            spdlog::debug("capturing mouse");
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            app->mouseCaptured = true;
        }
    }
}

static void errors(int32_t error, const char *description) {
    std::fprintf(stderr, "Code: %d\nError: %s\n", error, description);
}

WindowGLFW::WindowGLFW(const std::unique_ptr<AppCore> &app) {
    const auto width = app->config.startingWidth;
    const auto height = app->config.startingHeight;

    spdlog::debug("Loading glfw");
    if (!glfwInit()) {
        throw std::runtime_error("Couldn't load glfw!");
    }

    if (ACTIVE_BACKEND == Backend::OpenGL) {
        std::string debugOutput;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    #ifndef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        debugOutput = "OpenGL v4.6";
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        debugOutput = "OpenGL v4.1";
    #endif
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        spdlog::debug(debugOutput);
    } else if (ACTIVE_BACKEND == Backend::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    handle = glfwCreateWindow(width, height, app->config.name, nullptr, nullptr);
    if (!handle) {
        const char *text = nullptr;
        glfwGetError(&text);
        glfwTerminate();
        spdlog::error("Could not create glfw window! Error: {}", text);
        throw std::runtime_error("");
    }

    if (ACTIVE_BACKEND == Backend::OpenGL) {
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(0);
        if (!gladLoadGL(glfwGetProcAddress)) {
            glfwDestroyWindow(handle);
            glfwTerminate();
            throw std::runtime_error("Couldn't load OpenGL with glad!");
        }
        spdlog::trace("glad load successful");
    }

    glfwSetErrorCallback(errors);
    glfwSetFramebufferSizeCallback(handle, resize);
    glfwSetKeyCallback(handle, keys);
    glfwSetCursorPosCallback(handle, cursor);
    glfwSetMouseButtonCallback(handle, mouse);
    glfwSetWindowUserPointer(handle, app.get());

    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    spdlog::trace("glfw window constructor ending");
}

WindowGLFW::~WindowGLFW() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void *WindowGLFW::CreateSurface(void *apiContext) {
    if (ACTIVE_BACKEND == Backend::OpenGL) {
        // glfw already handles this when the window is created for OpenGL contexts
        return nullptr;
    }

    if (ACTIVE_BACKEND == Backend::Vulkan) {
        VkSurfaceKHR surface_khr;
        if (glfwCreateWindowSurface(static_cast<VkInstance>(apiContext), handle, nullptr, &surface_khr) != VK_SUCCESS) {
            spdlog::error("Couldn't create surface!");
            throw;
        }
        return surface_khr;
    }

    return nullptr;
}

std::pair<int, int> WindowGLFW::GetFrameBufferSize() {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(handle, &width, &height);
    return {width, height};
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

void WindowGLFW::Close() {
    glfwSetWindowShouldClose(handle, true);
}

double WindowGLFW::GetTime() {
    return glfwGetTime();
}

void WindowGLFW::SetTitle(const std::string &title) {
    glfwSetWindowTitle(handle, title.c_str());
}
}
