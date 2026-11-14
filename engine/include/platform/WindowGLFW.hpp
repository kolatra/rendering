#pragma once
#include <utility>
#include "IWindow.hpp"

struct GLFWwindow;

namespace Luna {
class AppCore;

class WindowGLFW : public IWindow {
    GLFWwindow *handle = nullptr;
public:
    WindowGLFW(const std::unique_ptr<AppCore>& app);
    ~WindowGLFW() override;
    void PollEvents() override;
    void SwapBuffers() override;
    double GetTime() override;
    std::pair<int, int> GetFrameBufferSize() override;
    void *GetNativeHandle() override;
    bool ShouldClose() override;
    void Show() override;
    void Close() override;
    void* CreateSurface(void* apiContext) override;
    void SetTitle(const std::string& title) override;
};
}
