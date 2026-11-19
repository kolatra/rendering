#pragma once
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "graphics/Renderer.hpp"

#define VK_CALLBACKS VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, \
            VkDebugUtilsMessageSeverityFlagsEXT messageType, \
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, \
            void *pUserData

namespace Luna::Vulkan {
    class Implementation : public Renderer {
    public:
        explicit Implementation() {}
        ~Implementation() override;
        void init(Window &window) override;
        void beginFrame() override;
        void endFrame() override ;
        void clear(const glm::vec4& colour) override;
        void setCulling(bool flag) override;
    };

    constexpr static auto MaxFramesInFlight = 3;
    constexpr static VkFormat SwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
    constexpr static VkFormat DepthFormat = VK_FORMAT_D32_SFLOAT;

    class VShader : public Shader {
    public:
        explicit VShader();
        ~VShader() override;

        void use() override;
        void setBool(const std::string &name, bool value) override;
        void setInt(const std::string &name, int value) override;
        void setFloat(const std::string &name, float value) override;
        void setMat4(const std::string &name, const glm::mat4 &value) override;

        VkShaderModule vertexModule = nullptr;
        VkShaderModule fragmentModule = nullptr;

        static VkShaderModule createModule(const std::string &filename, shaderc_shader_kind kind);
    };

    struct Instance {
        VkInstance handle = nullptr;
        bool initialized = false;
        void Create(VkDebugUtilsMessengerCreateInfoEXT &debugInfo);

        static std::vector<const char *> GetExtensions();
    };

    struct FrameResources {
        VkCommandPool cmdPool = nullptr;
        VkCommandBuffer cmdBuffer = nullptr;
        VkSemaphore imageAcquiredSemaphore = nullptr;
    };

    struct Swapchain {
        VkSwapchainKHR handle = nullptr;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkSemaphore> renderCompleteSemaphores;
        bool requireRecreate = false;
        uint32_t swapchainWidth = 0;
        uint32_t swapchainHeight = 0;
        VkImage depthImage = nullptr;
        VkImageView depthImageView = nullptr;
        VmaAllocation depthImageAllocation = nullptr;
        void Check();
        void Create(uint32_t w, uint32_t h);
        void Destroy();
    };

    struct Device {
        VkPhysicalDevice physical = nullptr;
        VkDevice logical = nullptr;
        VkPhysicalDevice FindPhysicalDevice();
        void CreateLogicalDevice();

        uint32_t gfxQueueId = 0;
        VkQueue gfxQueue = nullptr;
        VkSurfaceKHR surface = nullptr;
    private:
        static bool Suitable(VkPhysicalDevice pd);
        bool SelectQueue();
    };

    struct Pipeline {
        VkPipeline handle = nullptr;
        VkPipelineLayout layout = nullptr;
        void Create();
    };

    struct Allocator {
        VmaAllocator handle = nullptr;
        void Init();
    };

    struct DebugMessenger {
        VkDebugUtilsMessengerEXT handle = nullptr;
        static VkBool32 callback(VK_CALLBACKS);
    };

    struct Sync {
        VkSemaphore timelineSemaphore = nullptr;
        std::array<FrameResources, MaxFramesInFlight> frameResources = {};
        uint64_t frameIndex = 0;
        uint64_t nextSignal = 0;
        void Setup();
        static void CreateCommandBuffers();
    };
}
