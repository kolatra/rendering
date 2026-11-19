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

static Instance instance;
static Device device;
static Swapchain swapchain;
static Allocator allocator;
static std::vector<Pipeline> pipelines;
static DebugMessenger debugMessenger;
static Sync sync;

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
    instance.Create(debugInfo);
    volkLoadInstance(instance.handle);
    check_error(
        vkCreateDebugUtilsMessengerEXT(instance.handle, &debugInfo, nullptr, &debugMessenger.handle),
        "Failed to create debug messenger!"
    );

    spdlog::trace("surface");
    device.surface = static_cast<VkSurfaceKHR>(window.CreateSurface(instance.handle));

    spdlog::trace("physical device");
    device.FindPhysicalDevice();

    spdlog::trace("logical device");
    device.CreateLogicalDevice();
    volkLoadDevice(device.logical);

    spdlog::trace("allocator");
    allocator.Init();

    spdlog::trace("swapchain");
    swapchain.Create(window.width, window.height);

    // TODO: write a pipeline helper as we'll want many, not just one
    spdlog::trace("pipeline");
    pipelines.push_back(Pipeline());
    pipelines[0].Create();

    spdlog::trace("setting up sync objects");
    sync.Setup();

    spdlog::trace("command buffers");
    sync.CreateCommandBuffers();

    spdlog::error("No real error, but we got here without anything breaking. :D");
    throw std::runtime_error("");
}

void Implementation::beginFrame() {
    // wait on semaphores and reset the command pool
    swapchain.Check();
    const uint64_t frameResIndex = sync.frameIndex++ % MaxFramesInFlight;
    const uint64_t signal = sync.nextSignal++;
    const uint64_t waitValue = signal - MaxFramesInFlight;

    VkSemaphoreWaitInfo waitInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &sync.timelineSemaphore,
        .pValues = &waitValue
    };
    vkWaitSemaphores(device.logical, &waitInfo, UINT64_MAX);

    auto &frameResource = sync.frameResources[frameResIndex];
    vkResetCommandPool(device.logical, frameResource.cmdPool, 0);
}

void Implementation::endFrame() {
    // present the submitted work from the queue
}

void Implementation::clear(const glm::vec4 &colour) {

}

void Implementation::setCulling(bool flag) {

}

VShader::VShader() {
    vertexModule = createModule("", shaderc_vertex_shader);
    fragmentModule = createModule("", shaderc_fragment_shader);
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

void Instance::Create(VkDebugUtilsMessengerCreateInfoEXT &debugInfo) {
    if (initialized) {
        return;
    }

    VkApplicationInfo appInfo = {
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
        .pfnUserCallback = debugMessenger.callback
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
        vkCreateInstance(&ci, nullptr, &handle),
        "Failed to create Vulkan instance!"
    );

    initialized = true;
}

std::vector<const char *> Instance::GetExtensions() {
    uint32_t extCount = 0;
    const auto extensions = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector requestedExtensions { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
    spdlog::trace("Found {} instance extensions", extCount);
    for (auto i = 0; i < extCount; i++)
        requestedExtensions.push_back(extensions[i]);
    return requestedExtensions;
}

bool Device::Suitable(const VkPhysicalDevice pd) {
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd, device.surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(pd, device.surface, &formatCount, formats.data());

    auto supported = false;
    for (const auto &[format, colorSpace] : formats) {
        if (format == SwapchainFormat) {
            supported = true;
            break;
        }
    }

    return supported;
}

VkPhysicalDevice Device::FindPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.handle, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.handle, &deviceCount, devices.data());

    for (auto i = 0; i < deviceCount; i++) {
        physical = devices[i];
        if (!Suitable(physical)) {
            continue;
        }
        VkPhysicalDeviceProperties props {};
        vkGetPhysicalDeviceProperties(physical, &props);
        spdlog::info("Device: {}", props.deviceName);
        SelectQueue();
        return physical;
    }

    spdlog::info("No physical device found!");
    throw std::runtime_error("No physical device found!");
}

void Device::CreateLogicalDevice() {
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
    vkGetPhysicalDeviceFeatures2(physical, &supportedFeatures);

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
        vkCreateDevice(physical, &devCreateInfo, nullptr, &logical),
        "Failed to create logical device!"
    );

    vkGetDeviceQueue(logical, gfxQueueId, 0, &gfxQueue);
    if (!gfxQueue) {
        spdlog::error("Failed to get graphics queue info!");
        throw;
    }
}

bool Device::SelectQueue() {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &count, properties.data());

    for (uint32_t i = 0; i < count; i++) {
        if ((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            gfxQueueId = i;
            return true;
        }
    }

    return false;
}

void Allocator::Init() {
    VmaVulkanFunctions funcInfo{};
    VmaAllocatorCreateInfo ci {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = device.physical,
        .device = device.logical,
        .pVulkanFunctions = &funcInfo,
        .instance = instance.handle,
        .vulkanApiVersion = VK_API_VERSION_1_4
    };

    vmaImportVulkanFunctionsFromVolk(&ci, &funcInfo);
    check_error(
        vmaCreateAllocator(&ci, &handle),
        "Could not create allocator!"
    );
}

void Swapchain::Check() {
    if (requireRecreate) {
        vkDeviceWaitIdle(device.logical);
        spdlog::trace("recreating swapchain");
        Destroy();
        // CreateSwapchain(); // TODO: this needs current window
        requireRecreate = false;
    }
}

void Swapchain::Create(uint32_t w, uint32_t h) {
    swapchainWidth = w;
    swapchainHeight = h;

    VkSurfaceCapabilitiesKHR surfaceCaps{};
    check_error(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical, device.surface, &surfaceCaps),
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
        .surface = device.surface,
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
        vkCreateSwapchainKHR(device.logical, &swapchainCreateInfo, nullptr, &handle),
        "Could not create swapchain!"
    );

    // fill our app's swapchain image buffer with what we have available
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device.logical, handle, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.logical, handle, &imageCount, swapchainImages.data());
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
            vkCreateImageView(device.logical, &imgViewInfo, nullptr, &swapchainImageViews[i]),
            "Error creating swapchain image view"
        );
    }

    renderCompleteSemaphores.resize(swapchainImages.size());
    for (auto &semaphore: renderCompleteSemaphores) {
        VkSemaphoreCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        check_error(
            vkCreateSemaphore(device.logical, &info, nullptr, &semaphore),
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
        vmaCreateImage(allocator.handle, &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr),
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
        vkCreateImageView(device.logical, &depthImgViewInfo, nullptr, &depthImageView),
        "Could not create depth image view!"
    );
}

void Swapchain::Destroy() {
    for (const auto imageView : swapchainImageViews) {
        vkDestroyImageView(device.logical, imageView, nullptr);
    }
    swapchainImageViews.clear();

    for (const auto &semaphore : renderCompleteSemaphores) {
        vkDestroySemaphore(device.logical, semaphore, nullptr);
    }
    renderCompleteSemaphores.clear();

    if (handle) {
        vkDestroySwapchainKHR(device.logical, handle, nullptr);
        handle = nullptr;
    }

    if (depthImageView) {
        vkDestroyImageView(device.logical, depthImageView, nullptr);
        depthImageView = nullptr;
    }
}

void Pipeline::Create() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };

    check_error(
        vkCreatePipelineLayout(device.logical, &pipelineLayoutInfo, nullptr, &layout),
        "Could not create pipeline layout!"
    );

    constexpr auto entryPoint = "main";
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = static_cast<VkShaderModule>(nullptr),
            .pName = entryPoint
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = static_cast<VkShaderModule>(nullptr),
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
        .layout = layout,
        .renderPass = VK_NULL_HANDLE,
    };

    check_error(
        vkCreateGraphicsPipelines(device.logical, nullptr, 1, &pipelineInfo, nullptr, &handle),
        "Could not create graphics pipeline!"
    );
}

void Sync::Setup() {
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
        vkCreateSemaphore(device.logical, &semaphoreInfo, nullptr, &timelineSemaphore),
        "Couldn't create timeline semaphore!"
    );

    for (auto &resource : frameResources) {
        VkSemaphoreCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        check_error(
            vkCreateSemaphore(device.logical, &createInfo, nullptr, &resource.imageAcquiredSemaphore),
            "Unable to create per-frame image-acquired semaphore!"
        );
    }
}

VkShaderModule VShader::createModule(const std::string &filename, shaderc_shader_kind kind) {
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
        vkCreateShaderModule(device.logical, &createInfo, nullptr, &shaderModule),
        "Could not create shader module!"
    );

    return shaderModule;
}

void Sync::CreateCommandBuffers() {
    for (auto &resource : sync.frameResources) {
        VkCommandPoolCreateInfo poolInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = device.gfxQueueId
        };

        check_error(
            vkCreateCommandPool(device.logical, &poolInfo, nullptr, &resource.cmdPool),
            "Could not create Command Pool!"
        );

        VkCommandBufferAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = resource.cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        check_error(
            vkAllocateCommandBuffers(device.logical, &allocInfo, &resource.cmdBuffer),
            "Could not allocate command buffer!"
        );
    }
}

VkBool32 DebugMessenger::callback(VK_CALLBACKS) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::error("Validation layer: {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}
}
