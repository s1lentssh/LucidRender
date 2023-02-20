#include <Vulkan/VulkanDescriptorSet.h>

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanBuffer.h>
#include <Core/UniformBufferObject.h>

namespace Lucid::Vulkan
{

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice& device, VulkanDescriptorPool& pool)
	: mDevice(device)
{
	// Allocate
	auto allocateInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(pool.Handle().get())
		.setDescriptorSetCount(1)
		.setPSetLayouts(&pool.Layout());

	mHandle = std::move(device.Handle()->allocateDescriptorSetsUnique(allocateInfo).at(0));
}

void VulkanDescriptorSet::Update(const vk::DescriptorBufferInfo& bufferInfo, const vk::DescriptorImageInfo& imageInfo)
{
	auto bufferDescriptorWrite = vk::WriteDescriptorSet()
		.setDstSet(Handle().get())
		.setDstBinding(0)
		.setDstArrayElement(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setPBufferInfo(&bufferInfo);

	auto imageDescriptorWrite = vk::WriteDescriptorSet()
		.setDstSet(Handle().get())
		.setDstBinding(1)
		.setDstArrayElement(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setPImageInfo(&imageInfo);

	mDevice.Handle()->updateDescriptorSets({ bufferDescriptorWrite, imageDescriptorWrite }, {});
}

}