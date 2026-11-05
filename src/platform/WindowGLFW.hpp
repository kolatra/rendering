#pragma once
#include "IWindow.hpp"
#include "../core/EngineHeart.hpp"

struct GLFWwindow;

namespace Luna {
class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
public:
    WindowGLFW(const std::shared_ptr<Configuration> &config, EngineHeart *app);
    ~WindowGLFW() override;
    void PollEvents() override;
    void SwapBuffers() override;
    double GetTime() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;
    void HandleInput(EngineCamera *camera, float delta) override;

    void* CreateSurface(void* apiContext) override;
};
}
