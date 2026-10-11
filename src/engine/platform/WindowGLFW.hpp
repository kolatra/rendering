#pragma once
#include "IWindow.hpp"

class Application;
struct GLFWwindow;

class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
    Backend backend = Backend::None;
public:
    WindowGLFW(Backend backend, int width, int height, std::string& title, Application *app);
    ~WindowGLFW() override;
    void PollEvents() override;
    void SwapBuffers() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;

    void* CreateSurface(void* apiContext) override;
    Backend GetBackend() const override { return backend; }
};
