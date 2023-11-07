#pragma once

#include "core/base.h"

#include "driver/bgiBase/enums.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanDevice;

struct BgiVulkanDescriptorSetInfo
{
  uint32_t setNumber;
  VkDescriptorSetLayoutCreateInfo createInfo;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};

using BgiVulkanDescriptorSetInfoVector =
    std::vector<BgiVulkanDescriptorSetInfo>;

using VkDescriptorSetLayoutVector = std::vector<VkDescriptorSetLayout>;


/// Compiles ascii shader code (glsl) into spirv binary code (spirvOut).
/// Returns true if successful. Errors can optionally be captured.
/// numShaderCodes determines how many strings are provided via shaderCodes.
/// 'name' is purely for debugging compile errors. It can be anything.
BGIVULKAN_API
bool BgiVulkanCompileGLSL(
    const char* name,
    const char* shaderCodes[],
    uint8_t numShaderCodes,
    BgiShaderStage stage,
    std::vector<unsigned int>* spirvOUT,
    std::string* errors = nullptr);

/// Uses spirv-reflection to create new descriptor set layout information for
/// the provided spirv.
/// This information can be merged with the info of the other shader stage
/// modules to create the pipeline layout.
/// During Hgi pipeline layout creation we know the shader modules
/// (HgiShaderProgram), but not the HgiResourceBindings so we must use
/// spirv reflection to discover the descriptorSet info for the module.
BGIVULKAN_API
BgiVulkanDescriptorSetInfoVector BgiVulkanGatherDescriptorSetInfo(
    std::vector<unsigned int> const& spirv);

/// Given all of the DescriptorSetInfos of all of the shader modules in a
/// shader program, this function merges them and creates the descriptorSet
/// layouts needed during pipeline layout creation.
/// The caller takes ownership of the returned layouts and must destroy them.
BGIVULKAN_API
VkDescriptorSetLayoutVector BgiVulkanMakeDescriptorSetLayouts(
    BgiVulkanDevice* device,
    std::vector<BgiVulkanDescriptorSetInfoVector> const& infos,
    std::string const& debugName);

GUNGNIR_NAMESPACE_CLOSE_SCOPE
