#pragma once

#include "common/base.h"
#include "common/math/math.h"

#include "driver/bgiBase/computeCmds.h"
#include "driver/bgiBase/computePipeline.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

using namespace math;

namespace driver {

struct BgiComputeCmdsDesc;
class BgiVulkan;
class BgiVulkanCommandBuffer;

/// \class HgiVulkanComputeCmds
///
/// OpenGL implementation of HgiComputeCmds.
///
class BgiVulkanComputeCmds final : public BgiComputeCmds
{
public:
    BGIVULKAN_API
    ~BgiVulkanComputeCmds() override;

    BGIVULKAN_API
    void PushDebugGroup(const char* label) override;

    BGIVULKAN_API
    void PopDebugGroup() override;

    BGIVULKAN_API
    void BindPipeline(BgiComputePipelineHandle pipeline) override;

    BGIVULKAN_API
    void BindResources(BgiResourceBindingsHandle resources) override;

    BGIVULKAN_API
    void SetConstantValues(
        BgiComputePipelineHandle pipeline,
        uint32_t bindIndex,
        uint32_t byteSize,
        const void* data) override;
    
    BGIVULKAN_API
    void Dispatch(int dimX, int dimY, int dimZ) override;

    BGIVULKAN_API
    void InsertMemoryBarrier(BgiMemoryBarrier barrier) override;

    BGIVULKAN_API
    BgiComputeDispatch GetDispatchMethod() const override;

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanComputeCmds(BgiVulkan* bgi, BgiComputeCmdsDesc const& desc);

    BGIVULKAN_API
    bool _Submit(Bgi* bgi, BgiSubmitWaitType wait) override;

private:
    BgiVulkanComputeCmds() = delete;
    BgiVulkanComputeCmds & operator=(const BgiVulkanComputeCmds&) = delete;
    BgiVulkanComputeCmds(const BgiVulkanComputeCmds&) = delete;

    void _BindResources();
    void _CreateCommandBuffer();

    BgiVulkan* _bgi;
    BgiVulkanCommandBuffer* _commandBuffer;
    VkPipelineLayout _pipelineLayout;
    BgiResourceBindingsHandle _resourceBindings;
    bool _pushConstantsDirty;
    uint8_t* _pushConstants;
    uint32_t _pushConstantsByteSize;
    Vector3i _localWorkGroupSize;

    // Cmds is used only one frame so storing multi-frame state on will not
    // survive.
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
