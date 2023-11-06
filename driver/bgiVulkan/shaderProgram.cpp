#include "driver/bgiVulkan/shaderProgram.h"
#include "driver/bgiVulkan/shaderFunction.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanShaderProgram::BgiVulkanShaderProgram(
    BgiVulkanDevice* device,
    BgiShaderProgramDesc const& desc)
    : BgiShaderProgram(desc)
    , _device(device)
    , _inflightBits(0)
{
}

bool
BgiVulkanShaderProgram::IsValid() const
{
    return true;
}

std::string const&
BgiVulkanShaderProgram::GetCompileErrors()
{
    static const std::string empty;
    return empty;
}

size_t
BgiVulkanShaderProgram::GetByteSizeOfResource() const
{
    size_t  byteSize = 0;
    for (BgiShaderFunctionHandle const& fn : _descriptor.shaderFunctions) {
        byteSize += fn->GetByteSizeOfResource();
    }
    return byteSize;
}

uint64_t
BgiVulkanShaderProgram::GetRawResource() const
{
    return 0; // No vulkan resource for programs
}

BgiShaderFunctionHandleVector const&
BgiVulkanShaderProgram::GetShaderFunctions() const
{
    return _descriptor.shaderFunctions;
}

BgiVulkanDevice*
BgiVulkanShaderProgram::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanShaderProgram::GetInflightBits()
{
    return _inflightBits;
}

using BgiShaderProgramHandle = BgiHandle<class BgiShaderProgram>;
using BgiShaderProgramHandleVector = std::vector<BgiShaderProgramHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE
