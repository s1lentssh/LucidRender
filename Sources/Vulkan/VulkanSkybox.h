#pragma once

#include <Core/Types.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanDescriptorSet.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanMesh.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanSampler.h>

namespace Lucid::Vulkan
{

class VulkanSkybox
{
public:
    VulkanSkybox(VulkanDevice& device, VulkanDescriptorPool& pool, VulkanCommandPool& manager);

    void Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const;
    void UpdateTransform(const Core::UniformBufferObject& ubo);

private:
    std::unique_ptr<VulkanImage> mTexture;
    std::unique_ptr<VulkanSampler> mSampler;
    VulkanDescriptorSet mDescriptorSet;
    std::unique_ptr<VulkanVertexBuffer> mVertexBuffer;
    std::unique_ptr<VulkanIndexBuffer> mIndexBuffer;
    VulkanUniformBuffer mUniformBuffer;
};

} // namespace Lucid::Vulkan
