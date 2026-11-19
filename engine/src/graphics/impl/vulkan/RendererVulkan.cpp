#include <queue>

#include <spdlog/spdlog.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include <shaderc/shaderc.hpp>

#include "graphics/impl/vulkan/RendererVulkan.hpp"

namespace Luna::Vulkan {
namespace {
    struct FrameResources {
        VkCommandPool cmdPool = nullptr;
        VkCommandBuffer cmdBuffer = nullptr;
        VkSemaphore imageAcquiredSemaphore = nullptr;
    };
}
constexpr static auto MaxFramesInFlight = 3;
constexpr static VkFormat SwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
constexpr static VkFormat DepthFormat = VK_FORMAT_D32_SFLOAT;
static uint64_t frameIndex = 0;
static uint64_t nextSignal = 0;
static std::array<FrameResources, MaxFramesInFlight> frameResources = {};

static VkApplicationInfo appInfo {};
static VkInstance appInstance = nullptr;
static bool initialized = false;
static void CreateInstance(VkDebugUtilsMessengerCreateInfoEXT &debugInfo);

static std::vector<const char *> GetExtensions();

static VkSurfaceKHR surface = nullptr;

static VkPhysicalDevice physicalDevice = nullptr;
static VkDevice logicalDevice = nullptr;
static bool DeviceSuitable(VkPhysicalDevice pd);
static VkPhysicalDevice FindPhysicalDevice();
static void CreateLogicalDevice();

static uint32_t gfxQueueId = 0;
static VkQueue gfxQueue = nullptr;
static bool SelectQueue();

static VmaAllocator allocator = nullptr;
static void CreateAllocator();

static VkSwapchainKHR swapchain = nullptr;
static std::vector<VkImage> swapchainImages;
static std::vector<VkImageView> swapchainImageViews;
static std::vector<VkSemaphore> renderCompleteSemaphores;
static bool requireRecreate = false;
static uint32_t swapchainWidth = 0;
static uint32_t swapchainHeight = 0;
static VkImage depthImage = nullptr;
static VkImageView depthImageView = nullptr;
static VmaAllocation depthImageAllocation = nullptr;
static void CheckSwapchain();
static void CreateSwapchain(uint32_t w, uint32_t h);
static void DestroySwapchain();

static VkShaderModule createShaderModule(const std::string &filename, shaderc_shader_kind kind);

static VkPipeline pipeline = nullptr;
static VkPipelineLayout pipelineLayout = nullptr;
static void CreatePipeline(VShader &shader);

static VkSemaphore timelineSemaphore = nullptr;

static void SetupSync();
static void CreateCommandBuffers();

static VkDebugUtilsMessengerEXT debugMessenger = nullptr;
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

static void check_error(VkResult result, const std::string &msg) {
    if (result != VK_SUCCESS) {
        spdlog::error(msg);
        throw;
    }
}

Implementation::~Implementation() {

}

void Implementation::init(Window &window) {
    spdlog::info("Checking for Vulkan support");
    if (!glfwVulkanSupported()) {
        spdlog::error("glfw and Vulkan said no");
        throw std::runtime_error("");
    }
    check_error(
        volkInitialize(),
        "Failed to initialize Vulkan loader"
    );

    spdlog::trace("instance");
    VkDebugUtilsMessengerCreateInfoEXT debugInfo {};
    CreateInstance(debugInfo);
    volkLoadInstance(appInstance);
    check_error(
        vkCreateDebugUtilsMessengerEXT(appInstance, &debugInfo, nullptr, &debugMessenger),
        "Failed to create debug messenger!"
    );

    spdlog::trace("surface");
    surface = static_cast<VkSurfaceKHR>(window.CreateSurface(appInstance));

    spdlog::trace("physical device");
    FindPhysicalDevice();

    spdlog::trace("queue");
    SelectQueue();

    spdlog::trace("logical device");
    CreateLogicalDevice();
    volkLoadDevice(logicalDevice);

    spdlog::trace("allocator");
    CreateAllocator();

    spdlog::trace("swapchain");
    CreateSwapchain(window.width, window.height);

    // TODO: write a pipeline helper as we'll want many, not just one
    spdlog::trace("pipeline");
    // CreatePipeline();

    spdlog::trace("setting up sync objects");
    SetupSync();

    spdlog::trace("command buffers");
    CreateCommandBuffers();

    spdlog::error("No real error, but we got here without anything breaking. :D");
}

void Implementation::beginFrame() {
    // wait on semaphores and reset the command pool
    CheckSwapchain();
    const uint64_t frameResIndex = frameIndex++ % MaxFramesInFlight;
    const uint64_t signal = nextSignal++;
    const uint64_t waitValue = signal - MaxFramesInFlight;

    VkSemaphoreWaitInfo waitInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &timelineSemaphore,
        .pValues = &waitValue
    };
    vkWaitSemaphores(logicalDevice, &waitInfo, UINT64_MAX);

    auto &frameResource = frameResources[frameResIndex];
    vkResetCommandPool(logicalDevice, frameResource.cmdPool, 0);
}

void Implementation::endFrame() {
    // present the submitted work from the queue
}

void Implementation::clear(const glm::vec4 &colour) {
}

void Implementation::setCulling(bool flag) {

}

VShader::VShader() {
    for (auto i = 0; i < 10; i++)
        spdlog::error("VULKAN SHADERRS NOT READY FOR USE");
    vertexModule = createShaderModule("", shaderc_vertex_shader);
    fragmentModule = createShaderModule("", shaderc_fragment_shader);
}

VShader::~VShader() {

}

void VShader::use() {

}

void VShader::setBool(const std::string &name, bool value) {

}

void VShader::setInt(const std::string &name, int value) {

}

void VShader::setFloat(const std::string &name, float value) {

}

void VShader::setMat4(const std::string &name, const glm::mat4 &value) {

}

void CreateInstance(VkDebugUtilsMessengerCreateInfoEXT &debugInfo) {
    if (initialized) {
        return;
    }

    appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Luna Engine",
        .apiVersion = VK_API_VERSION_1_4
    };

    auto requestedExtensions = GetExtensions();

    std::vector requestedLayers { "VK_LAYER_KHRONOS_validation" };
    debugInfo = {
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

    check_error(
        vkCreateInstance(&ci, nullptr, &appInstance),
        "Failed to create Vulkan instance!"
    );

    initialized = true;
}

std::vector<const char *> GetExtensions() {
    uint32_t extCount = 0;
    const auto extensions = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector requestedExtensions { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
    spdlog::trace("Found {} instance extensions", extCount);
    for (auto i = 0; i < extCount; i++)
        requestedExtensions.push_back(extensions[i]);
    return requestedExtensions;
}

bool DeviceSuitable(const VkPhysicalDevice pd) {
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formatCount, formats.data());

    auto supported = false;
    for (const auto &[format, colorSpace] : formats) {
        if (format == SwapchainFormat) {
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
    VkPhysicalDeviceVulkan14Features supportedFeatures14{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = nullptr
    };
    VkPhysicalDeviceVulkan13Features supportedFeatures13{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &supportedFeatures14
    };
    VkPhysicalDeviceVulkan12Features supportedFeatures12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &supportedFeatures13
    };
    VkPhysicalDeviceFeatures2 supportedFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &supportedFeatures12
    };
    vkGetPhysicalDeviceFeatures2(physicalDevice, &supportedFeatures);

    if (!supportedFeatures13.dynamicRendering || !supportedFeatures13.
        synchronization2 ||
        !supportedFeatures12.timelineSemaphore) {
        spdlog::error("Physical device doesn't meet the feature requirements");
        throw;
    }

    // Allows changing polygon mode at runtime instead of it being static
    // in the pipeline.
    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dr_features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT,
        .pNext = nullptr,
        .extendedDynamicState3PolygonMode = VK_TRUE,
    };

    VkPhysicalDeviceVulkan14Features features14 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &dr_features
    };

    VkPhysicalDeviceVulkan13Features features13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &features14,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
    };

    VkPhysicalDeviceVulkan12Features features12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &features13,
        .scalarBlockLayout = VK_TRUE,
        .timelineSemaphore = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE
    };

    VkPhysicalDeviceFeatures2 features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features12,
        .features{.shaderInt64 = VK_TRUE}
    };

    std::vector queuePriorities{1.0f};
    VkDeviceQueueCreateInfo gfxQueueInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = gfxQueueId,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities.data()
    };

    const std::vector deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_EXT_extended_dynamic_state3" };

    VkDeviceCreateInfo devCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &gfxQueueInfo,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = nullptr // features struct chain is set in pNext
    };

    check_error(
        vkCreateDevice(physicalDevice, &devCreateInfo, nullptr, &logicalDevice),
        "Failed to create logical device!"
    );

    vkGetDeviceQueue(logicalDevice, gfxQueueId, 0, &gfxQueue);
    if (!gfxQueue) {
        spdlog::error("Failed to get graphics queue info!");
        throw;
    }
}

bool SelectQueue() {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, properties.data());

    for (uint32_t i = 0; i < count; i++) {
        if ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            gfxQueueId = i;
            return true;
        }
    }

    return false;
}

void CreateAllocator() {
    VmaVulkanFunctions funcInfo{};
    VmaAllocatorCreateInfo ci {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = physicalDevice,
        .device = logicalDevice,
        .pVulkanFunctions = &funcInfo,
        .instance = appInstance,
        .vulkanApiVersion = VK_API_VERSION_1_4
    };

    vmaImportVulkanFunctionsFromVolk(&ci, &funcInfo);
    check_error(
        vmaCreateAllocator(&ci, &allocator),
        "Could not create allocator!"
    );
}

void CheckSwapchain() {
    if (requireRecreate) {
        vkDeviceWaitIdle(logicalDevice);
        spdlog::trace("recreating swapchain");
        DestroySwapchain();
        // CreateSwapchain(); // TODO: this needs current window
        requireRecreate = false;
    }
}

void CreateSwapchain(uint32_t w, uint32_t h) {
    swapchainWidth = w;
    swapchainHeight = h;

    VkSurfaceCapabilitiesKHR surfaceCaps{};
    check_error(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps),
        "Could not get surface capabilities!"
    );

    /*
     * get the larger value between 2 (the amount of images that we want) and the minimum dictated by our capabilities.
     *
     * if Vulkan states that we can ask for a minimum of 3 images, we want to not be under that threshold as the swapchain
     * will fail to create.
     *
     * unlike the minimum, the maximum can be set to 0, which means we can ask for an unlimited number of them.
     * physical resources aren't unlimited though (thanks AI), and if there is a maximum we want to ensure we don't go over it.
     */
    uint32_t requestedImageCount = std::max(2u, surfaceCaps.minImageCount);
    if (surfaceCaps.maxImageCount > 0) {
        requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = requestedImageCount,
        .imageFormat = SwapchainFormat,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent{.width = swapchainWidth, .height = swapchainHeight},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        /* This presentMode creates a FIFO queue where images are pushed after being rendered, and then popped
         * from the queue for presentation.
         *
         * During a vblank the presentation engine pops and displays the front of the queue.
         *
         * Because of the vblank format, this mode also enables vertical sync for us.
         *
         * If for whatever reason the queue fills up and has no available slots, we can pause on the cpu side and wait
         * for a new slot to come available before continuing to queue render images on the gpu
         *
         * There are additional types that could be useful:
         * VK_PRESENT_MODE_MAILBOX_KHR      - like FIFO, but will replace an image if the queue is full
         * VK_PRESENT_MODE_IMMEDIATE_KHR    - images are presented immediately when sent for presentation.
         *                                    no v-sync. potential for screen tearing as it doesn't wait for a vblank
         * VK_PRESENT_MODE_FIFO_RELAXED_KHR - FIFO by default, becomes IMMEDIATE when renderer lags
         */
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
    };

    check_error(
        vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain),
        "Could not create swapchain!"
    );

    // fill our app's swapchain image buffer with what we have available
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());
    swapchainImageViews.resize(imageCount);

    // image views
    for (auto i = 0; i < swapchainImages.size(); ++i) {
        VkImageViewCreateInfo imgViewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = SwapchainFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        check_error(
            vkCreateImageView(logicalDevice, &imgViewInfo, nullptr, &swapchainImageViews[i]),
            "Error creating swapchain image view"
        );
    }

    renderCompleteSemaphores.resize(swapchainImages.size());
    for (auto &semaphore: renderCompleteSemaphores) {
        VkSemaphoreCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        check_error(
            vkCreateSemaphore(logicalDevice, &info, nullptr, &semaphore),
            "Could not create semaphore!"
        );
    }

    VkImageCreateInfo depthCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = DepthFormat,
        .extent {
            .width = swapchainWidth, .height = swapchainHeight, .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocInfo{
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    check_error(
        vmaCreateImage(allocator, &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr),
        "Could not allocate depth image!"
    );

    VkImageViewCreateInfo depthImgViewInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = depthImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = DepthFormat,
        .subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    check_error(
        vkCreateImageView(logicalDevice, &depthImgViewInfo, nullptr, &depthImageView),
        "Could not create depth image view!"
    );
}

void DestroySwapchain() {
    for (const auto imageView : swapchainImageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    swapchainImageViews.clear();

    for (const auto &semaphore : renderCompleteSemaphores) {
        vkDestroySemaphore(logicalDevice, semaphore, nullptr);
    }
    renderCompleteSemaphores.clear();

    if (swapchain) {
        vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
        swapchain = nullptr;
    }

    if (depthImageView) {
        vkDestroyImageView(logicalDevice, depthImageView, nullptr);
        depthImageView = nullptr;
    }
}

void CreatePipeline(VShader &shader) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };

    check_error(
        vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout),
        "Could not create pipeline layout!"
    );

    constexpr auto entryPoint = "main";
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shader.vertexModule,
            .pName = entryPoint
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader.fragmentModule,
            .pName = entryPoint
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS
    };

    VkPipelineViewportStateCreateInfo viewportInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        // .polygonMode = VK_POLYGON_MODE_LINE,
        // switching between wireframe and normal
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f
    };

    // Disable multisampling
    VkPipelineMultisampleStateCreateInfo multiInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkPipelineColorBlendAttachmentState attachState {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendStateCreateInfo blendInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachState
    };

    std::vector dynamicState {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicStateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicState.size()),
        .pDynamicStates = dynamicState.data()
    };

    VkPipelineRenderingCreateInfo renderInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &SwapchainFormat,
        .depthAttachmentFormat = DepthFormat
    };

    VkGraphicsPipelineCreateInfo pipelineInfo {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterInfo,
        .pMultisampleState = &multiInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &blendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = pipelineLayout,
        .renderPass = VK_NULL_HANDLE,
    };

    check_error(
        vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &pipeline),
        "Could not create graphics pipeline!"
    );
}

void SetupSync() {
    VkSemaphoreTypeCreateInfo semaphoreTypeInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = MaxFramesInFlight
    };

    VkSemaphoreCreateInfo semaphoreInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaphoreTypeInfo
    };

    check_error(
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &timelineSemaphore),
        "Couldn't create timeline semaphore!"
    );

    for (auto &resource : frameResources) {
        VkSemaphoreCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        check_error(
            vkCreateSemaphore(logicalDevice, &createInfo, nullptr, &resource.imageAcquiredSemaphore),
            "Unable to create per-frame image-acquired semaphore!"
        );
    }
}

VkShaderModule createShaderModule(const std::string &filename, shaderc_shader_kind kind) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    options.SetTargetSpirv(shaderc_spirv_version_1_6);
    options.SetOptimizationLevel(shaderc_optimization_level_zero);

    // TODO
    std::string src = "todo, need to get the shader code here somehow, either from on disk or in static memory";

    shaderc::CompilationResult result = compiler.CompileGlslToSpv(
        src,
        kind,
        filename.c_str(),
        options
    );

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        spdlog::error("Shader compilation error: {}", result.GetErrorMessage());
        throw;
    }

    size_t shaderSize = (result.cend() - result.cbegin()) * sizeof(uint32_t);
    VkShaderModuleCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderSize,
        .pCode = result.cbegin(),
    };

    VkShaderModule shaderModule;
    check_error(
        vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule),
        "Could not create shader module!"
    );

    return shaderModule;
}

void CreateCommandBuffers() {
    for (auto &resource : frameResources) {
        VkCommandPoolCreateInfo poolInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = gfxQueueId
        };

        check_error(
            vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &resource.cmdPool),
            "Could not create Command Pool!"
        );

        VkCommandBufferAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = resource.cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        check_error(
            vkAllocateCommandBuffers(logicalDevice, &allocInfo, &resource.cmdBuffer),
            "Could not allocate command buffer!"
        );
    }
}
}
