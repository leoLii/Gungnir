#include "capabilities.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanCapabilities::BgiVulkanCapabilities(BgiVulkanDevice* device)
    : supportsTimeStamps(false)
{
    VkPhysicalDevice physicalDevice = device->GetVulkanPhysicalDevice();

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, 0);
    std::vector<VkQueueFamilyProperties> queues(queueCount);

    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueCount,
        queues.data());

    // Grab the properties of all queues up until the (gfx) queue we are using.
    uint32_t gfxQueueIndex = device->GetGfxQueueFamilyIndex();

    // The last queue we grabbed the properties of is our gfx queue.
    if (TF_VERIFY(gfxQueueIndex < queues.size())) {
        VkQueueFamilyProperties const& gfxQueue = queues[gfxQueueIndex];
        supportsTimeStamps = gfxQueue.timestampValidBits > 0;
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &vkDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &vkDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vkMemoryProperties);

    // Vertex attribute divisor properties ext
    vkVertexAttributeDivisorProperties.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;
    vkVertexAttributeDivisorProperties.pNext = nullptr;
        
    vkDeviceProperties2.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkDeviceProperties2.properties = vkDeviceProperties;
    vkDeviceProperties2.pNext = &vkVertexAttributeDivisorProperties;
    vkGetPhysicalDeviceProperties2(physicalDevice, &vkDeviceProperties2);

    // Vertex attribute divisor features ext
    vkVertexAttributeDivisorFeatures.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;
    vkVertexAttributeDivisorFeatures.pNext = nullptr;

    // Indexing features ext for resource bindings
    vkIndexingFeatures.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    vkIndexingFeatures.pNext = &vkVertexAttributeDivisorFeatures;

    // Vulkan 1.1 features
    vkVulkan11Features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    vkVulkan11Features.pNext = &vkIndexingFeatures;

    // Query device features
    vkDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkDeviceFeatures2.pNext = &vkVulkan11Features;
    vkGetPhysicalDeviceFeatures2(physicalDevice, &vkDeviceFeatures2);

    // Verify we meet feature and extension requirements

    // Storm with HgiVulkan needs gl_BaseInstance/gl_BaseInstanceARB in shader.
    TF_VERIFY(
        vkVulkan11Features.shaderDrawParameters);

    #if !defined(VK_USE_PLATFORM_MACOS_MVK)
        TF_VERIFY(
            vkIndexingFeatures.shaderSampledImageArrayNonUniformIndexing &&
            vkIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing);
    #endif

    TF_VERIFY(
        vkVertexAttributeDivisorFeatures.vertexAttributeInstanceRateDivisor);

    if (BgiVulkanIsDebugEnabled()) {
        TF_WARN("Selected GPU %s", vkDeviceProperties.deviceName);
    }

    _maxClipDistances = vkDeviceProperties.limits.maxClipDistances;
    _maxUniformBlockSize = vkDeviceProperties.limits.maxUniformBufferRange;
    _maxShaderStorageBlockSize =
        vkDeviceProperties.limits.maxStorageBufferRange;
    _uniformBufferOffsetAlignment =
        vkDeviceProperties.limits.minUniformBufferOffsetAlignment;

    const bool conservativeRasterEnabled = (device->IsSupportedExtension(
        VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME));
    const bool hasBuiltinBarycentrics = (device->IsSupportedExtension(
        VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME));
    const bool shaderDrawParametersEnabled =
        vkVulkan11Features.shaderDrawParameters;

    _SetFlag(HgiDeviceCapabilitiesBitsDepthRangeMinusOnetoOne, false);
    _SetFlag(HgiDeviceCapabilitiesBitsStencilReadback, true);
    _SetFlag(HgiDeviceCapabilitiesBitsMultiDrawIndirect, true);
    _SetFlag(HgiDeviceCapabilitiesBitsShaderDoublePrecision, true);
    _SetFlag(HgiDeviceCapabilitiesBitsConservativeRaster, 
        conservativeRasterEnabled);
    _SetFlag(HgiDeviceCapabilitiesBitsBuiltinBarycentrics, 
        hasBuiltinBarycentrics);
    _SetFlag(HgiDeviceCapabilitiesBitsShaderDrawParameters, 
        shaderDrawParametersEnabled);
}

BgiVulkanCapabilities::~BgiVulkanCapabilities() = default;

int
BgiVulkanCapabilities::GetAPIVersion() const
{
    return vkDeviceProperties.apiVersion;
}

int
BgiVulkanCapabilities::GetShaderVersion() const
{
    // Note: This is not the Vulkan Shader Language version. It is provided for
    // compatibility with code that is asking for the GLSL version.
    return 450;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
