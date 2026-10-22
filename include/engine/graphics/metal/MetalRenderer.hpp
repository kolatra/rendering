#pragma once

#include <engine/graphics/IRenderer.hpp>
#include <Metal/Metal.hpp>

namespace Luna::Metal {
    struct Implementation : IRenderer {
        ~Implementation() override;
        void init(IWindow *window, Configuration *config) override;
        void beginFrame() override;
        void endFrame() override;
        void clear(const glm::vec4 &colour) override;
    private:
        MTL::Device* _pDevice = nullptr;
        MTL::CommandQueue* _pCommandQueue = nullptr;
    };
}
