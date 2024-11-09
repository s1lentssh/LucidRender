#include <Core/UniformBufferObject.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanDescriptorSet.h>
#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool, const std::string& name)
    : VulkanEntity(name, device.Handle())
{
    // Allocate
    auto allocateInfo = vk::DescriptorSetAllocateInfo()
                            .setDescriptorPool(pool.Handle())
                            .setDescriptorSetCount(1)
                            .setPSetLayouts(&pool.Layout());

    VulkanEntity::SetHandle(std::move(device.Handle().allocateDescriptorSetsUnique(allocateInfo).at(0)));
}

void
VulkanDescriptorSet::Update(
    const vk::DescriptorBufferInfo& bufferInfo,
    const vk::DescriptorImageInfo& albedoInfo,
    const vk::DescriptorImageInfo& metallicRoughnessInfo,
    const vk::DescriptorImageInfo& normalInfo,
    const vk::DescriptorBufferInfo& materialInfo)
{
    auto bufferDescriptorWrite = vk::WriteDescriptorSet()
                                     .setDstSet(Handle())
                                     .setDstBinding(0)
                                     .setDstArrayElement(0)
                                     .setDescriptorCount(1)
                                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                     .setPBufferInfo(&bufferInfo);

    auto albedoDescriptorWrite = vk::WriteDescriptorSet()
                                     .setDstSet(Handle())
                                     .setDstBinding(1)
                                     .setDstArrayElement(0)
                                     .setDescriptorCount(1)
                                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                     .setPImageInfo(&albedoInfo);

    auto metallicRoughnessDescriptorWrite = vk::WriteDescriptorSet()
                                                .setDstSet(Handle())
                                                .setDstBinding(2)
                                                .setDstArrayElement(0)
                                                .setDescriptorCount(1)
                                                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                                .setPImageInfo(&metallicRoughnessInfo);

    auto normalDescriptorWrite = vk::WriteDescriptorSet()
                                     .setDstSet(Handle())
                                     .setDstBinding(3)
                                     .setDstArrayElement(0)
                                     .setDescriptorCount(1)
                                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                     .setPImageInfo(&normalInfo);

    auto materialDescriptorWrite = vk::WriteDescriptorSet()
                                       .setDstSet(Handle())
                                       .setDstBinding(4)
                                       .setDstArrayElement(0)
                                       .setDescriptorCount(1)
                                       .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                       .setPBufferInfo(&materialInfo);

    Device().updateDescriptorSets(
        { bufferDescriptorWrite,
          albedoDescriptorWrite,
          metallicRoughnessDescriptorWrite,
          normalDescriptorWrite,
          materialDescriptorWrite },
        {});
}

} // namespace Lucid::Vulkan
