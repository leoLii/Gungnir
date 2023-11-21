#include "common/utils/diagnostic.h"

#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/capabilities.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/instance.h"
#include "driver/bgiVulkan/pipelineCache.h"

#define VMA_IMPLEMENTATION
    #include <vk_mem_alloc.h>
#undef VMA_IMPLEMENTATION

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

static uint32_t
_GetGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, 0);

    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueCount,
        queues.data());

    for (uint32_t i = 0; i < queueCount; i++) {
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }

    return VK_QUEUE_FAMILY_IGNORED;
}

static bool
_SupportsPresentation(
    VkPhysicalDevice physicalDevice,
    uint32_t familyIndex)
{
    #if defined(VK_USE_PLATFORM_WIN32_KHR)
        return vkGetPhysicalDeviceWin32PresentationSupportKHR(
                    physicalDevice, familyIndex);
    #elif defined(VK_USE_PLATFORM_XLIB_KHR)
        Display* dsp = XOpenDisplay(nullptr);
        VisualID visualID = XVisualIDFromVisual(
            DefaultVisual(dsp, DefaultScreen(dsp)));
        return vkGetPhysicalDeviceXlibPresentationSupportKHR(
                    physicalDevice, familyIndex, dsp, visualID);
    #elif defined(VK_USE_PLATFORM_MACOS_MVK)
        // Presentation currently always supported on Metal / MoltenVk
        return true;
    #else
        #error Unsupported Platform
        return true;
    #endif
}

BgiVulkanDevice::BgiVulkanDevice(BgiVulkanInstance* instance)
    : _vkPhysicalDevice(nullptr)
    , _vkDevice(nullptr)
    , _vmaAllocator(nullptr)
    , _commandQueue(nullptr)
    , _capabilities(nullptr)
{
    //
    // Determine physical device
    //

    const uint32_t maxDevices = 64;
    VkPhysicalDevice physicalDevices[maxDevices];
    uint32_t physicalDeviceCount = maxDevices;
    UTILS_VERIFY(
        vkEnumeratePhysicalDevices(
            instance->GetVulkanInstance(),
            &physicalDeviceCount,
            physicalDevices) == VK_SUCCESS
    );

    for (uint32_t i = 0; i < physicalDeviceCount; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

        uint32_t familyIndex =
            _GetGraphicsQueueFamilyIndex(physicalDevices[i]);

        if (familyIndex == VK_QUEUE_FAMILY_IGNORED) continue;

        // Assume we always want a presentation capable device for now.
        if (!_SupportsPresentation(physicalDevices[i], familyIndex)) {
            continue;
        }

        if (props.apiVersion < VK_API_VERSION_1_0) continue;

        // Try to find a discrete device. Until we find a discrete device,
        // store the first non-discrete device as fallback in case we never
        // find a discrete device at all.
        if (props.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            _vkPhysicalDevice = physicalDevices[i];
            _vkGfxsQueueFamilyIndex = familyIndex;
            break;
        } else if (!_vkPhysicalDevice) {
            _vkPhysicalDevice = physicalDevices[i];
            _vkGfxsQueueFamilyIndex = familyIndex;
        }
    }

    if (!_vkPhysicalDevice) {
        UTILS_CODING_ERROR("VULKAN_ERROR: Unable to determine physical device");
        return;
    }

    //
    // Query supported extensions for device
    //

    uint32_t extensionCount = 0;
    UTILS_VERIFY(
        vkEnumerateDeviceExtensionProperties(
            _vkPhysicalDevice,
            nullptr,
            &extensionCount,
            nullptr) == VK_SUCCESS
    );

    _vkExtensions.resize(extensionCount);

    UTILS_VERIFY(
        vkEnumerateDeviceExtensionProperties(
            _vkPhysicalDevice,
            nullptr,
            &extensionCount,
            _vkExtensions.data()) == VK_SUCCESS
    );

    //
    // Create Device
    //
    _capabilities = new BgiVulkanCapabilities(this);

    VkDeviceQueueCreateInfo queueInfo =
        {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    float queuePriorities[] = {1.0f};
    queueInfo.queueFamilyIndex = _vkGfxsQueueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    std::vector<const char*> extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Allow certain buffers/images to have dedicated memory allocations to
    // improve performance on some GPUs.
    bool dedicatedAllocations = false;
    if (IsSupportedExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME)
        && IsSupportedExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
    {
        dedicatedAllocations = true;
        extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    }

    // Allow OpenGL interop - Note requires two extensions in HgiVulkanInstance.
    if (IsSupportedExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME) &&
        IsSupportedExtension(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME))
    {
        extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
    }

    // Memory budget query extension
    bool supportsMemExtension = false;
    if (IsSupportedExtension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
        supportsMemExtension = true;
        extensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
    }

    // Resolve depth during render pass resolve extension
    if (IsSupportedExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        extensions.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
        extensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
    }

    // Allows the same layout in structs between c++ and glsl (share structs).
    // This means instead of 'std430' you can now use 'scalar'.
    if (IsSupportedExtension(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME)) {
        extensions.push_back(VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME);
    } else {
        UTILS_WARN("Unsupported VK_EXT_scalar_block_layout."
                "Update gfx driver?");
    }

    // Allow conservative rasterization.
    if (IsSupportedExtension(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME)) {
        extensions.push_back(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);
    }

    // Allow use of built-in shader barycentrics.
    if (IsSupportedExtension(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)) {
        extensions.push_back(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME);
    }

    // Allow use of shader draw parameters.
    if (IsSupportedExtension(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME);
    }

    // Allow use of vertex attribute divisors.
    if (IsSupportedExtension(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
        extensions.push_back(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME);
    }

    // This extension is needed to allow the viewport to be flipped in Y so that
    // shaders and vertex data can remain the same between opengl and vulkan.
    extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

    // Enabling certain features may incure a performance hit
    // (e.g. robustBufferAccess), so only enable the features we will use.
    VkPhysicalDeviceVulkan11Features vulkan11Features =
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
    vulkan11Features.pNext = _capabilities->vkVulkan11Features.pNext;
    vulkan11Features.shaderDrawParameters =
        _capabilities->vkVulkan11Features.shaderDrawParameters;

    VkPhysicalDeviceVulkan12Features vulkan12Features =
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    vulkan12Features.pNext = _capabilities->vkVulkan12Features.pNext;

    VkPhysicalDeviceVulkan12Features vulkan13Features =
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    vulkan13Features.pNext = _capabilities->vkVulkan13Features.pNext;

    VkPhysicalDeviceFeatures2 features2 =
        {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    features2.pNext = &vulkan13Features;

    features2.features.multiDrawIndirect =
        _capabilities->vkDeviceFeatures.multiDrawIndirect;
    features2.features.samplerAnisotropy =
        _capabilities->vkDeviceFeatures.samplerAnisotropy;
    features2.features.shaderSampledImageArrayDynamicIndexing =
        _capabilities->vkDeviceFeatures.shaderSampledImageArrayDynamicIndexing;
    features2.features.shaderStorageImageArrayDynamicIndexing =
        _capabilities->vkDeviceFeatures.shaderStorageImageArrayDynamicIndexing;
    features2.features.sampleRateShading =
        _capabilities->vkDeviceFeatures.sampleRateShading;
    features2.features.shaderClipDistance =
        _capabilities->vkDeviceFeatures.shaderClipDistance;
    features2.features.tessellationShader =
        _capabilities->vkDeviceFeatures.tessellationShader;
    features2.features.depthClamp =
        _capabilities->vkDeviceFeatures.depthClamp;
    features2.features.shaderFloat64 =
        _capabilities->vkDeviceFeatures.shaderFloat64;

    // Needed to write to storage buffers from vertex shader (eg. GPU culling).
    features2.features.vertexPipelineStoresAndAtomics =
        _capabilities->vkDeviceFeatures.vertexPipelineStoresAndAtomics;
    // Needed to write to storage buffers from fragment shader (eg. OIT).
    features2.features.fragmentStoresAndAtomics =
        _capabilities->vkDeviceFeatures.fragmentStoresAndAtomics;

    #if !defined(VK_USE_PLATFORM_MACOS_MVK)
        // Needed for buffer address feature
        features2.features.shaderInt64 =
            _capabilities->vkDeviceFeatures.shaderInt64;
        // Needed for gl_primtiveID
        features2.features.geometryShader =
            _capabilities->vkDeviceFeatures.geometryShader;
    #endif

    VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();
    createInfo.pNext = &features2;

    UTILS_VERIFY(
        vkCreateDevice(
            _vkPhysicalDevice,
            &createInfo,
            BgiVulkanAllocator(),
            &_vkDevice) == VK_SUCCESS
    );

    BgiVulkanSetupDeviceDebug(instance, this);

    //
    // Extension function pointers
    //

    vkCreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR)
    vkGetDeviceProcAddr(_vkDevice, "vkCreateRenderPass2KHR");

    //
    // Memory allocator
    //

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.instance = instance->GetVulkanInstance();
    allocatorInfo.physicalDevice = _vkPhysicalDevice;
    allocatorInfo.device = _vkDevice;
    if (dedicatedAllocations) {
        allocatorInfo.flags |=VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (supportsMemExtension) {
        allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    UTILS_VERIFY(
        vmaCreateAllocator(&allocatorInfo, &_vmaAllocator) == VK_SUCCESS
    );

    //
    // Command Queue
    //

    _commandQueue = new BgiVulkanCommandQueue(this);

    //
    // Pipeline cache
    //

    _pipelineCache = new BgiVulkanPipelineCache(this);
}

BgiVulkanDevice::~BgiVulkanDevice()
{
    // Make sure device is idle before destroying objects.
    UTILS_VERIFY(vkDeviceWaitIdle(_vkDevice) == VK_SUCCESS);

    delete _pipelineCache;
    delete _commandQueue;
    delete _capabilities;
    vmaDestroyAllocator(_vmaAllocator);
    vkDestroyDevice(_vkDevice, BgiVulkanAllocator());
}

VkDevice
BgiVulkanDevice::GetVulkanDevice() const
{
    return _vkDevice;
}

VmaAllocator
BgiVulkanDevice::GetVulkanMemoryAllocator() const
{
    return _vmaAllocator;
}

BgiVulkanCommandQueue*
BgiVulkanDevice::GetCommandQueue() const
{
    return _commandQueue;
}

BgiVulkanCapabilities const&
BgiVulkanDevice::GetDeviceCapabilities() const
{
    return *_capabilities;
}

uint32_t
BgiVulkanDevice::GetGfxQueueFamilyIndex() const
{
    return _vkGfxsQueueFamilyIndex;
}

VkPhysicalDevice
BgiVulkanDevice::GetVulkanPhysicalDevice() const
{
    return _vkPhysicalDevice;
}

BgiVulkanPipelineCache*
BgiVulkanDevice::GetPipelineCache() const
{
    return _pipelineCache;
}

void
BgiVulkanDevice::WaitForIdle()
{
    UTILS_VERIFY(
        vkDeviceWaitIdle(_vkDevice) == VK_SUCCESS
    );
}

bool
BgiVulkanDevice::IsSupportedExtension(const char* extensionName) const
{
    for (VkExtensionProperties const& ext : _vkExtensions) {
        if (!strcmp(extensionName, ext.extensionName)) {
            return true;
        }
    }

    return false;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
