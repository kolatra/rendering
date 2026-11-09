#pragma once
#include "../IWindow.hpp"
#include "../../core/AppCore.hpp"

struct GLFWwindow;

namespace Luna {
class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
public:
    WindowGLFW(const std::shared_ptr<Configuration> &config, AppCore *app);
    ~WindowGLFW() override;
    void PollEvents() override;
    void SwapBuffers() override;
    double GetTime() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;
    void HandleInput(EngineCamera *camera, float delta) override;
    void* CreateSurface(void* apiContext) override;
    void SetTitle(const std::string& title) override;
};
}
