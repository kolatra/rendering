#pragma once
#include <bitset>
#include <memory>
#include <string>

namespace Luna {
    struct EngineCamera;
    struct Configuration;

enum class Backend {
    None,
    OpenGL,
    Vulkan,
    Metal
};

inline Backend be_from_string(const std::string& in) {
    if (in == "OpenGL") {
        return Backend::OpenGL;
    }

    if (in == "Vulkan") {
        return Backend::Vulkan;
    }

    if (in == "Metal") {
        return Backend::Metal;
    }

    // Default
    return Backend::OpenGL;
}

class IWindow {
public:
    std::shared_ptr<Configuration> config {};
    std::bitset<1024> held_input {};
    virtual ~IWindow() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual double GetTime() = 0;
    virtual void *GetNativeHandle() = 0;
    virtual bool ShouldClose() = 0;
    virtual void Show() = 0;
    virtual void HandleInput(EngineCamera *camera, float delta) = 0;
    virtual void* CreateSurface(void* apiContext) = 0;
};
}
