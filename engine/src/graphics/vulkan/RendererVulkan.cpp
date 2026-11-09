#include <spdlog/spdlog.h>
#include "graphics/vulkan/RendererVulkan.hpp"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace Luna::Vulkan {
    static VkBool32 debugCallback(
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

    Implementation::~Implementation() {

    }

    void Implementation::init(IWindow *window, Configuration *config) {
        spdlog::info("Checking for proper support in Vulkan");
        if (!glfwVulkanSupported()) {
            spdlog::error("glfw and Vulkan said no");
            throw std::runtime_error("");
        }

        CreateInstance();
        surface = static_cast<VkSurfaceKHR>(window->CreateSurface(appInstance));
        FindPhysicalDevice();
        CreateLogicalDevice();

        spdlog::info("Vulkan instance created successfully!");
        exit(420);
    }

    void Implementation::beginFrame() {
    }

    void Implementation::endFrame() {
    }

    void Implementation::clear(const glm::vec4 &colour) {
    }

    void Implementation::setCulling(bool flag) {
    }

    void CreateInstance() {
        if (initialized) {
            return;
        }

        appInfo = {
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

        std::vector requestedLayers { "VK_LAYER_KHRONOS_validation" };
        VkDebugUtilsMessengerCreateInfoEXT debugInfo {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback
        };

        const VkInstanceCreateInfo ci {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = &debugInfo,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
            .ppEnabledLayerNames = requestedLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
            .ppEnabledExtensionNames = requestedExtensions.data()
        };

        if (vkCreateInstance(&ci, nullptr, &appInstance) != VK_SUCCESS) {
            spdlog::error("Failed to create Vulkan instance!");
            throw std::runtime_error("Failed to create Vulkan instance!");
        }

        initialized = true;
    }

    bool DeviceSuitable(const VkPhysicalDevice pd) {
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formatCount, formats.data());

        auto supported = false;
        for (const auto &[format, colorSpace] : formats) {
            if (format == VK_FORMAT_B8G8R8A8_SRGB) {
                supported = true;
                break;
            }
        }

        return supported;
    }

    VkPhysicalDevice FindPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(appInstance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(appInstance, &deviceCount, devices.data());

        for (auto i = 0; i < deviceCount; i++) {
            physicalDevice = devices[i];
            if (!DeviceSuitable(physicalDevice)) {
                continue;
            }
            VkPhysicalDeviceProperties props {};
            vkGetPhysicalDeviceProperties(physicalDevice, &props);
            spdlog::info("Device: {}", props.deviceName);
            return physicalDevice;
        }

        spdlog::info("No physical device found!");
        throw std::runtime_error("No physical device found!");
    }

    void CreateLogicalDevice() {

    }
}
