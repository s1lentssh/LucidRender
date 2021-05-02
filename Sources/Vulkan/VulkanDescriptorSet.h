#pragma once

#include <Vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

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

}