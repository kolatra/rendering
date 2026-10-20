#pragma once
#include <memory>

namespace Luna {
struct Configuration;

enum class Backend {
    None,
    OpenGL,
    Vulkan,
    Metal
};

class IWindow {
public:
    std::shared_ptr<Configuration> config {};
    virtual ~IWindow() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetNativeHandle() = 0;
    virtual bool ShouldClose() = 0;
    virtual void Show() = 0;

    virtual void* CreateSurface(void* apiContext) = 0;
};
}
