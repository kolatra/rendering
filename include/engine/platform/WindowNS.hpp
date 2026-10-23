#pragma once
#include <engine/platform/IWindow.hpp>

namespace Luna {
class MacOSWindow : public IWindow {
public:
    MacOSWindow(Configuration *config);
    ~MacOSWindow() override;
    void PollEvents() override;
    void SwapBuffers() override;
    double GetTime() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;
    void HandleInput(EngineCamera *camera, float delta) override;
    void *CreateSurface(void* apiContext) override;
private:
    void* nsWindow = nullptr;
};
}
