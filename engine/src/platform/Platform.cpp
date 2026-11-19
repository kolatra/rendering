#include "platform/Window.hpp"
#include "platform/WindowGLFW.hpp"

namespace Luna {
    std::unique_ptr<Window> CreateWindow(const std::unique_ptr<AppCore> &application) {
        return std::make_unique<WindowGLFW>(application);
    }
}
