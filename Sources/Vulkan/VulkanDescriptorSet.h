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
    VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool);
    void Update(const vk::DescriptorBufferInfo& bufferInfo, const vk::DescriptorImageInfo& imageInfo);

private:
    VulkanDevice& mDevice;
};

} // namespace Lucid::Vulkan
