#include "RendererVulkan.hpp"
#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Luna::Vulkan {
    static inline VkApplicationInfo gAppInfo {};
    static inline VkInstance gInstance {};

    VkBool32 debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageSeverityFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
    ) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            spdlog::error("Validation layer: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    void Startup() {
        spdlog::info("Checking for proper support in Vulkan");
        if (!glfwVulkanSupported()) {
            spdlog::error("glfw and Vulkan said no");
            throw std::runtime_error("");
        }

        gAppInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Luna Engine",
            .apiVersion = VK_API_VERSION_1_4
        };

        uint32_t extCount = 0;
        const auto extensions = glfwGetRequiredInstanceExtensions(&extCount);
        std::vector requestedExtensions { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
        spdlog::info("Found {} instance extensions", extCount);
        for (auto i = 0; i < extCount; i++)
            requestedExtensions.push_back(extensions[i]);

        std::vector requestedLayers { "VK_LAYER_KHRONOS_VALIDATION" };
        VkDebugUtilsMessengerCreateInfoEXT debugInfo {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
             .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
             .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
             .pfnUserCallback = debugCallback
        };

        VkInstanceCreateInfo ci {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = &debugInfo,
            .pApplicationInfo = &gAppInfo,
            .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
            .ppEnabledLayerNames = requestedLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
            .ppEnabledExtensionNames = requestedExtensions.data()
        };
        if (vkCreateInstance(&ci, nullptr, &gInstance) != VK_SUCCESS) {
            spdlog::error("Could not create Vulkan instance!");
            throw std::runtime_error("");
        }
    }
}
