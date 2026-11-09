#pragma once
#include "../IRenderer.hpp"
#include <vulkan/vulkan.h>

namespace Luna::Vulkan {
    inline VkApplicationInfo appInfo {};
    inline VkInstance appInstance {};
    inline bool initialized = false;
    void CreateInstance();

    inline VkSurfaceKHR surface;

    inline VkDevice device;
    inline VkPhysicalDevice physicalDevice;
    bool DeviceSuitable(VkPhysicalDevice pd);
    VkPhysicalDevice FindPhysicalDevice();
    void CreateLogicalDevice();

    class Implementation : public IRenderer {
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
