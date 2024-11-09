#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanDescriptorPool;

class VulkanDescriptorSet : public VulkanEntity<vk::UniqueDescriptorSet>
{
public:
    VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool, const std::string& name);
    void Update(
        const vk::DescriptorBufferInfo& bufferInfo,
        const vk::DescriptorImageInfo& albedoInfo,
        const vk::DescriptorImageInfo& metallicRoughnessInfo,
        const vk::DescriptorImageInfo& normalInfo,
        const vk::DescriptorBufferInfo& materialInfo);
};

} // namespace Lucid::Vulkan
