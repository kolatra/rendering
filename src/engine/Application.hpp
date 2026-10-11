#ifndef ENGINE_V29090909_APPLICATION_HPP
#define ENGINE_V29090909_APPLICATION_HPP
#include <memory>

#include "graphics/IRenderer.hpp"
#include "platform/IWindow.hpp"

class Application {
    std::unique_ptr<IWindow> window;
    std::unique_ptr<IRenderer> renderer;
    Backend rendering_backend = Backend::None;
public:
    void run(uint32_t width, uint32_t height);
    void setBackend(const Backend choice) { rendering_backend = choice; }
};

#endif //ENGINE_V29090909_APPLICATION_HPP
