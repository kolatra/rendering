#pragma once

namespace Luna {
enum class Backend {
    None,
    OpenGL,
    Vulkan,
    Metal
};

class IWindow {
    Backend backend = Backend::None;
public:
    virtual ~IWindow() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetNativeHandle() = 0;
    virtual bool ShouldClose() = 0;
    virtual void Show() = 0;

    virtual void* CreateSurface(void* apiContext) = 0;
    Backend GetBackend() const { return backend; }
    void SetBackend(Backend b) { this->backend = b; }
};
}
