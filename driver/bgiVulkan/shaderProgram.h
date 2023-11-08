#pragma once

#include "core/base.h"

#include "driver/bgiBase/shaderProgram.h"
#include "driver/bgiVulkan/shaderFunction.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkanDevice;

///
/// \class HgiVulkanShaderProgram
///
/// Vulkan implementation of HgiShaderProgram
///
class BgiVulkanShaderProgram final : public BgiShaderProgram
{
public:
    BGIVULKAN_API
    ~BgiVulkanShaderProgram() override = default;

    BGIVULKAN_API
    bool IsValid() const override;

    BGIVULKAN_API
    std::string const& GetCompileErrors() override;

    BGIVULKAN_API
    size_t GetByteSizeOfResource() const override;

    BGIVULKAN_API
    uint64_t GetRawResource() const override;

    /// Returns the shader functions that are part of this program.
    BGIVULKAN_API
    BgiShaderFunctionHandleVector const& GetShaderFunctions() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanShaderProgram(
        BgiVulkanDevice* device,
        BgiShaderProgramDesc const& desc);

private:
    BgiVulkanShaderProgram() = delete;
    BgiVulkanShaderProgram & operator=(const BgiVulkanShaderProgram&) = delete;
    BgiVulkanShaderProgram(const BgiVulkanShaderProgram&) = delete;

    BgiVulkanDevice* _device;
    uint64_t _inflightBits;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
