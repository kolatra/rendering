#include "IWindow.hpp"
#include "glfw/WindowGLFW.hpp"

namespace Luna {
    std::unique_ptr<IWindow> CreateAppWindow(const std::shared_ptr<Configuration> &config, AppCore *application) {
        return std::make_unique<WindowGLFW>(config, application);
    }
}
