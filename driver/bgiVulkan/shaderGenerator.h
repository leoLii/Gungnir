#ifndef GUNGNIR_DRIVER_VULKAN_SHADER_GENERATOR_H
#define GUNGNIR_DRIVER_VULKAN_SHADER_GENERATOR_H

#include "core/base.h"

#include "driver/bgiBase/shaderGenerator.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/shaderSection.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class Bgi;

using BgiVulkanShaderSectionUniquePtrVector =
    std::vector<std::unique_ptr<BgiVulkanShaderSection>>;

/// \class HgiVulkanShaderGenerator
///
/// Takes in a descriptor and spits out GLSL code through it's execute function.
///
class BgiVulkanShaderGenerator final: public BgiShaderGenerator
{
public:
    BGIVULKAN_API
    explicit BgiVulkanShaderGenerator(
        Bgi const *bgi,
        const BgiShaderFunctionDesc &descriptor);

    //This is not commonly consumed by the end user, but is available.
    BGIVULKAN_API
    BgiVulkanShaderSectionUniquePtrVector* GetShaderSections();

    template<typename SectionType, typename ...T>
    SectionType *CreateShaderSection(T && ...t);

protected:
    BGIVULKAN_API
    void _Execute(std::ostream &ss) override;

private:
    BgiVulkanShaderGenerator() = delete;
    BgiVulkanShaderGenerator & operator=(const BgiVulkanShaderGenerator&) = delete;
    BgiVulkanShaderGenerator(const BgiVulkanShaderGenerator&) = delete;

    void _WriteVersion(std::ostream &ss);

    void _WriteExtensions(std::ostream &ss);
    
    void _WriteMacros(std::ostream &ss);

    void _WriteConstantParams(
        const BgiShaderFunctionParamDescVector &parameters);

    void _WriteTextures(const BgiShaderFunctionTextureDescVector& textures);
	
    void _WriteBuffers(const BgiShaderFunctionBufferDescVector &buffers);

    //For writing shader inputs and outputs who are very similarly written
    void _WriteInOuts(
        const BgiShaderFunctionParamDescVector &parameters,
        const std::string &qualifier);
    void _WriteInOutBlocks(
        const BgiShaderFunctionParamBlockDescVector &parameterBlocks,
        const std::string &qualifier);
    
    BgiVulkanShaderSectionUniquePtrVector _shaderSections;
    Bgi const *_bgi;
    uint32_t _textureBindIndexStart;
    uint32_t _inLocationIndex;
    uint32_t _outLocationIndex;
    std::vector<std::string> _shaderLayoutAttributes;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_SHADER_GENERATOR_H