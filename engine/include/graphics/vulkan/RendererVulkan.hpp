#pragma once
#include "../Renderer.hpp"
#include <vulkan/vulkan.h>

namespace Luna::Vulkan {
    inline VkApplicationInfo appInfo {};
    inline VkInstance appInstance {};
    inline bool initialized = false;
    void CreateInstance();
    std::vector<const char *> GetExtensions();

    inline VkSurfaceKHR surface;

    inline VkDevice device;
    inline VkPhysicalDevice physicalDevice;
    bool DeviceSuitable(VkPhysicalDevice pd);
    VkPhysicalDevice FindPhysicalDevice();
    void CreateLogicalDevice();

    class Implementation : public Renderer {
    public:
        explicit Implementation() {}
        ~Implementation() override;
        void init(IWindow &window) override;
        void beginFrame() override;
        void endFrame() override ;
        void clear(const glm::vec4& colour) override;
        void setCulling(bool flag) override;
    };
}
