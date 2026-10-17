#pragma once
#include "IWindow.hpp"

class Application;
struct GLFWwindow;

namespace Luna {
class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
public:
    WindowGLFW(std::shared_ptr<Configuration> config, Application *app);
    ~WindowGLFW() override;
    void PollEvents() override;
    void SwapBuffers() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;

    void* CreateSurface(void* apiContext) override;
};
}
