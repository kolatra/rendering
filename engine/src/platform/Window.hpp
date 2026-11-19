#pragma once
#include <bitset>
#include <memory>
#include <utility>
#include <string>
#include <cstdint>

namespace Luna {
class AppCore;
struct EngineCamera;
struct Configuration;

class Window {
public:
    std::bitset<1024> held_input {};
    uint32_t width, height;
    virtual ~Window() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual double GetTime() = 0;
    virtual std::pair<int, int> GetFrameBufferSize() = 0;
    virtual void *GetNativeHandle() = 0;
    virtual bool ShouldClose() = 0;
    virtual void Close() = 0;
    virtual void Show() = 0;
    virtual void* CreateSurface(void* apiContext) = 0;
    virtual void SetTitle(const std::string& title) = 0;

    AppCore *app;
    double mouseX = 400;
    double mouseY = 300;
    bool mouseMovement = false;
    bool mouseCaptured = true;
    void UpdateMouse(double x, double y);
};

std::unique_ptr<Window> CreateWindow(const std::unique_ptr<AppCore> &app);
}
