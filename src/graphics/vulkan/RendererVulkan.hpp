#pragma once
#include "../IRenderer.hpp"
#include <vulkan/vulkan.h>

namespace Luna::Vulkan {
    class Implementation : public IRenderer {
        VkApplicationInfo appInfo {};
        VkInstance appInstance {};
        VkResult CreateInstance();
    public:
        explicit Implementation() {}
        ~Implementation() override;
        void init(IWindow *window, Configuration *config) override;
        void beginFrame() override;
        void endFrame() override ;
        void clear(const glm::vec4& colour) override;
        void setCulling(bool flag) override;
    };
}
