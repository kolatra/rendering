#pragma once
#include "IWindow.hpp"

#include <GLFW/glfw3.h>

class Application;

class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
    Backend backend = Backend::None;
public:
    WindowGLFW(Backend backend, int width, int height, std::string& title, Application *app);
    ~WindowGLFW() override;
    void PollEvents() override { glfwPollEvents(); }
    void SwapBuffers() override { glfwSwapBuffers(handle); }
    void *GetNativeHandle() override { return handle; }
    bool ShouldClose() override { return glfwWindowShouldClose(handle); }

    void* CreateSurface(void* apiContext) override;
    Backend GetBackend() const override { return backend; }
};
