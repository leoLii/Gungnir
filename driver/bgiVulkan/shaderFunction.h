#pragma once

#include "core/base.h"

#include "driver/bgiBase/shaderFunction.h"
#include "driver/bgiVulkan/shaderCompiler.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class Bgi;
class BgiVulkan;
class BgiVulkanDevice;

///
/// \class HgiVulkanShaderFunction
///
/// Vulkan implementation of HgiShaderFunction
///
class BgiVulkanShaderFunction final : public BgiShaderFunction
{
public:
    BGIVULKAN_API
    ~BgiVulkanShaderFunction() override;

    BGIVULKAN_API
    bool IsValid() const override;

    BGIVULKAN_API
    std::string const& GetCompileErrors() override;

    BGIVULKAN_API
    size_t GetByteSizeOfResource() const override;

    BGIVULKAN_API
    uint64_t GetRawResource() const override;

    /// Returns the shader stage this function operates in.
    BGIVULKAN_API
    VkShaderStageFlagBits GetShaderStage() const;

    /// Returns the binary shader module of the shader function.
    BGIVULKAN_API
    VkShaderModule GetShaderModule() const;

    /// Returns the shader entry function name (usually "main").
    BGIVULKAN_API
    const char* GetShaderFunctionName() const;

    /// Returns the descriptor set layout information that describe the
    /// resource bindings for this module. The returned info would usually be
    /// merged with info of other shader modules to create a VkPipelineLayout.
    BGIVULKAN_API
    BgiVulkanDescriptorSetInfoVector const& GetDescriptorSetInfo() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanShaderFunction(
        BgiVulkanDevice* device,
        Bgi const* bgi,
        BgiShaderFunctionDesc const& desc,
        int shaderVersion);

private:
    BgiVulkanShaderFunction() = delete;
    BgiVulkanShaderFunction& operator=(const BgiVulkanShaderFunction&) = delete;
    BgiVulkanShaderFunction(const BgiVulkanShaderFunction&) = delete;

    BgiVulkanDevice* _device;
    std::string _errors;
    size_t _spirvByteSize;
    VkShaderModule _vkShaderModule;
    BgiVulkanDescriptorSetInfoVector _descriptorSetInfo;
    uint64_t _inflightBits;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE