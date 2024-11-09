#pragma once

#include <Core/UniformBufferObject.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanDescriptorSet.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanSampler.h>

namespace Lucid::Vulkan
{

class VulkanMesh
{
public:
    VulkanMesh(VulkanDevice& device, VulkanDescriptorPool& pool, VulkanCommandPool& manager, const Core::MeshPtr& mesh);
    void Draw(vk::CommandBuffer& commandBuffer, VulkanPipeline& pipeline) const;
    void UpdateTransform(const Core::UniformBufferObject& ubo);

private:
    VulkanVertexBuffer mVertexBuffer;
    VulkanIndexBuffer mIndexBuffer;
    VulkanUniformBuffer mUniformBuffer;
    VulkanMaterialBuffer mMaterialBuffer;
    std::unique_ptr<VulkanImage> mAlbedoTexture;
    std::unique_ptr<VulkanImage> mMetallicRoughnessTexture;
    std::unique_ptr<VulkanImage> mNormalTexture;
    std::unique_ptr<VulkanSampler> mAlbedoSampler;
    std::unique_ptr<VulkanSampler> mMetallicRoughnessSampler;
    std::unique_ptr<VulkanSampler> mNormalSampler;
    std::unique_ptr<VulkanDescriptorSet> mDescriptorSet;
};

} // namespace Lucid::Vulkan
