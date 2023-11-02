#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/computeCmds.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/computePipeline.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/resourceBindings.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanComputeCmds::BgiVulkanComputeCmds(
    BgiVulkan* bgi,
    BgiComputeCmdsDesc const&)
    : BgiComputeCmds()
    , _bgi(bgi)
    , _commandBuffer(nullptr)
    , _pipelineLayout(nullptr)
    , _pushConstantsDirty(false)
    , _pushConstants(nullptr)
    , _pushConstantsByteSize(0)
    , _localWorkGroupSize(Vector3i(1, 1, 1))
{
}

BgiVulkanComputeCmds::~BgiVulkanComputeCmds()
{
    delete[] _pushConstants;
}

void
BgiVulkanComputeCmds::PushDebugGroup(const char* label)
{
    _CreateCommandBuffer();
    BgiVulkanBeginLabel(_bgi->GetPrimaryDevice(), _commandBuffer, label);
}

void
BgiVulkanComputeCmds::PopDebugGroup()
{
    _CreateCommandBuffer();
    BgiVulkanEndLabel(_bgi->GetPrimaryDevice(), _commandBuffer);
}

void
BgiVulkanComputeCmds::BindPipeline(BgiComputePipelineHandle pipeline)
{
    _CreateCommandBuffer();

    BgiVulkanComputePipeline* pso = 
        static_cast<BgiVulkanComputePipeline*>(pipeline.Get());

    if (UTILS_VERIFY(pso)) {
        _pipelineLayout = pso->GetVulkanPipelineLayout();
        pso->BindPipeline(_commandBuffer->GetVulkanCommandBuffer());
    }

    // Get and store local work group size from shader function desc
    const BgiShaderFunctionHandleVector shaderFunctionsHandles = 
        pipeline.Get()->GetDescriptor().shaderProgram.Get()->GetDescriptor().
            shaderFunctions;

    for (const auto &handle : shaderFunctionsHandles) {
        const BgiShaderFunctionDesc &shaderDesc = handle.Get()->GetDescriptor();
        if (shaderDesc.shaderStage == BgiShaderStageCompute) {
            if (shaderDesc.computeDescriptor.localSize[0] > 0 && 
                shaderDesc.computeDescriptor.localSize[1] > 0 &&
                shaderDesc.computeDescriptor.localSize[2] > 0) {
                _localWorkGroupSize = shaderDesc.computeDescriptor.localSize;
            }
        }
    }
}

void
BgiVulkanComputeCmds::BindResources(BgiResourceBindingsHandle res)
{
    _CreateCommandBuffer();
    // Delay bindings until we know for sure what the pipeline will be.
    _resourceBindings = res;
}

void
BgiVulkanComputeCmds::SetConstantValues(
    BgiComputePipelineHandle pipeline,
    uint32_t bindIndex,
    uint32_t byteSize,
    const void* data)
{
    _CreateCommandBuffer();
    // Delay pushing until we know for sure what the pipeline will be.
    if (!_pushConstants || _pushConstantsByteSize != byteSize) {
        delete[] _pushConstants;
        _pushConstants = new uint8_t[byteSize];
        _pushConstantsByteSize = byteSize;
    }
    memcpy(_pushConstants, data, byteSize);
    _pushConstantsDirty = true;
}

void
BgiVulkanComputeCmds::Dispatch(int dimX, int dimY)
{
    _CreateCommandBuffer();
    _BindResources();

    const int threadsPerGroupX = _localWorkGroupSize[0];
    const int threadsPerGroupY = _localWorkGroupSize[1];
    int numWorkGroupsX = (dimX + (threadsPerGroupX - 1)) / threadsPerGroupX;
    int numWorkGroupsY = (dimY + (threadsPerGroupY - 1)) / threadsPerGroupY;

    // Determine device's num compute work group limits
    const VkPhysicalDeviceLimits limits = 
        _bgi->GetCapabilities()->vkDeviceProperties.limits;
    const Vector3i maxNumWorkGroups = Vector3i(
        limits.maxComputeWorkGroupCount[0],
        limits.maxComputeWorkGroupCount[1],
        limits.maxComputeWorkGroupCount[2]);

    if (numWorkGroupsX > maxNumWorkGroups[0]) {
        TF_WARN("Max number of work group available from device is %i, larger "
                "than %i", maxNumWorkGroups[0], numWorkGroupsX);
        numWorkGroupsX = maxNumWorkGroups[0];
    }
    if (numWorkGroupsY > maxNumWorkGroups[1]) {
        TF_WARN("Max number of work group available from device is %i, larger "
                "than %i", maxNumWorkGroups[1], numWorkGroupsY);
        numWorkGroupsY = maxNumWorkGroups[1];
    }

    vkCmdDispatch(
        _commandBuffer->GetVulkanCommandBuffer(),
        (uint32_t) numWorkGroupsX,
        (uint32_t) numWorkGroupsY,
        1);
}

bool
BgiVulkanComputeCmds::_Submit(Bgi* bgi, BgiSubmitWaitType wait)
{
    if (!_commandBuffer) {
        return false;
    }

    BgiVulkanDevice* device = _commandBuffer->GetDevice();
    BgiVulkanCommandQueue* queue = device->GetCommandQueue();

    // Submit the GPU work and optionally do CPU - GPU synchronization.
    queue->SubmitToQueue(_commandBuffer, wait);

    return true;
}

void
BgiVulkanComputeCmds::_BindResources()
{
    if (!_pipelineLayout) {
        return;
    }

    if (_resourceBindings) {
        BgiVulkanResourceBindings * rb =
            static_cast<BgiVulkanResourceBindings*>(_resourceBindings.Get());

        if (rb){
            rb->BindResources(
                _commandBuffer->GetVulkanCommandBuffer(),
                VK_PIPELINE_BIND_POINT_COMPUTE,
                _pipelineLayout);
        }

        // Make sure we bind only once
        _resourceBindings = BgiResourceBindingsHandle();
    }

    if (_pushConstantsDirty && _pushConstants && _pushConstantsByteSize > 0) {
        vkCmdPushConstants(
            _commandBuffer->GetVulkanCommandBuffer(),
            _pipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0, // offset
            _pushConstantsByteSize,
            _pushConstants);

        // Make sure we copy only once
        _pushConstantsDirty = false;
    }
}

void
BgiVulkanComputeCmds::InsertMemoryBarrier(BgiMemoryBarrier barrier)
{
    _CreateCommandBuffer();
    _commandBuffer->InsertMemoryBarrier(barrier);
}

BgiComputeDispatch
BgiVulkanComputeCmds::GetDispatchMethod() const
{
    return BgiComputeDispatchSerial;
}

void
BgiVulkanComputeCmds::_CreateCommandBuffer()
{
    if (!_commandBuffer) {
        BgiVulkanDevice* device = _bgi->GetPrimaryDevice();
        BgiVulkanCommandQueue* queue = device->GetCommandQueue();
        _commandBuffer = queue->AcquireCommandBuffer();
        UTILS_VERIFY(_commandBuffer);
    }
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
