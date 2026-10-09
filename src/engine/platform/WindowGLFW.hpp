#pragma once
#include "IWindow.hpp"

#include <GLFW/glfw3.h>

using LoadFunctionPointer = void(*)();

class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
public:
    WindowGLFW(Backend backend, int width, int height);
    ~WindowGLFW() override;
    void PollEvents() override { glfwPollEvents(); }
    void SwapBuffers() override { glfwSwapBuffers(handle); }
    void *GetNativeHandle() override { return handle; }
    bool ShouldClose() override { return glfwWindowShouldClose(handle); }
    void SetUserPointer(void *t) override { glfwSetWindowUserPointer(handle, t); }
    void SetMouseGrab(bool flag) override;
};
