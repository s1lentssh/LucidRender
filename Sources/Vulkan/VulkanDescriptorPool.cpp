#include "VulkanDescriptorPool.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanVertex.h>

namespace Lucid::Vulkan
{

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device) : mDevice(device)
{
	const std::uint32_t kMaxDescriptorCount = 128;
	const std::uint32_t kMaxDescriptorSets = 128;

	auto poolSize = vk::DescriptorPoolSize()
		.setDescriptorCount(kMaxDescriptorCount)
		.setType(vk::DescriptorType::eUniformBuffer);

	auto createInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizeCount(1)
		.setPPoolSizes(&poolSize)
		.setMaxSets(kMaxDescriptorSets)
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	mHandle = device.Handle().createDescriptorPoolUnique(createInfo);

	CreateDescriptorSetLayout();
}

void VulkanDescriptorPool::CreateDescriptorSets(std::size_t count, const std::vector<std::unique_ptr<VulkanUniformBuffer>>& uniformBuffers)
{
	std::vector<vk::DescriptorSetLayout> layouts(count, mDescriptorSetLayout.get());

	auto allocateInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(Handle())
		.setDescriptorSetCount(static_cast<std::uint32_t>(count))
		.setPSetLayouts(layouts.data());

	mDescriptorSets = mDevice.Handle().allocateDescriptorSetsUnique(allocateInfo);

	for (std::size_t i = 0; i < count; i++)
	{
		auto bufferInfo = vk::DescriptorBufferInfo()
			.setBuffer(uniformBuffers.at(i)->Handle())
			.setOffset(0)
			.setRange(sizeof(UniformBufferObject));

		auto descriptorWrite = vk::WriteDescriptorSet()
			.setDstSet(mDescriptorSets.at(i).get())
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo(&bufferInfo);

		mDevice.Handle().updateDescriptorSets(descriptorWrite, {});
	}
}

void VulkanDescriptorPool::CreateDescriptorSetLayout()
{
	auto layoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	auto createInfo = vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(&layoutBinding);

	mDescriptorSetLayout = mDevice.Handle().createDescriptorSetLayoutUnique(createInfo);
}

const vk::DescriptorSet& VulkanDescriptorPool::GetDescriptorSet(std::size_t index) const 
{ 
	return mDescriptorSets.at(index).get(); 
}

}
