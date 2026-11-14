#include "platform/IWindow.hpp"
#include "../../include/platform/WindowGLFW.hpp"

namespace Luna {
    std::unique_ptr<IWindow> CreateWindow(const std::unique_ptr<AppCore> &application) {
        return std::make_unique<WindowGLFW>(application);
    }
}
