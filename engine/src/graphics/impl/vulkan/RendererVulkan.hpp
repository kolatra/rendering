#pragma once
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include <vma/vk_mem_alloc.h>

#include "graphics/Renderer.hpp"
#include "graphics/types/Shader.hpp"

#define VK_CALLBACKS VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, \
            VkDebugUtilsMessageSeverityFlagsEXT messageType, \
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, \
            void *pUserData

namespace Luna::Vulkan {
    class Implementation : public Renderer {
    public:
        explicit Implementation() {}
        ~Implementation() override;
        void init(Window *window) override;
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
        explicit VShader(const shaderc::CompilationResult<uint32_t> &vSpirv, const shaderc::CompilationResult<uint32_t> &fSpirv);
        ~VShader() override;

        void use() override;
        void setBool(const std::string &name, bool value) override;
        void setInt(const std::string &name, int value) override;
        void setFloat(const std::string &name, float value) override;
        void setMat4(const std::string &name, const glm::mat4 &value) override;

        VkShaderModule vertexModule = nullptr;
        VkShaderModule fragmentModule = nullptr;

        static VkShaderModule createModule(const shaderc::CompilationResult<uint32_t> &code);
    };

    struct Instance {
        VkInstance handle = nullptr;
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
        void Create(Window &window);
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
        void Create(std::vector<VShader*> &shaders);
    };

    struct Allocator {
        VmaAllocator handle = nullptr;
        void Init();
    };

    struct DebugMessenger {
        VkDebugUtilsMessengerEXT handle = nullptr;
        static VkBool32 callback(VK_CALLBACKS);
    };

    struct SyncManager {
        VkSemaphore timelineSemaphore = nullptr;
        std::array<FrameResources, MaxFramesInFlight> frameResources = {};
        uint64_t frameIndex = 0;
        uint64_t nextSignal = 0;
        void Setup();
        // Updates our frame resource indices and returns them.
        std::tuple<uint64_t, uint64_t, uint64_t> Values();
        static void CreateCommandBuffers();
    };
}
