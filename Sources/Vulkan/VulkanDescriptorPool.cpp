#include "VulkanDescriptorPool.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanSampler.h>
#include <Vulkan/VulkanImage.h>
#include <Core/UniformBufferObject.h>

namespace Lucid::Vulkan
{

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device) : mDevice(device)
{
	const std::uint32_t kMaxDescriptorCount = 128;
	const std::uint32_t kMaxDescriptorSets = 128;

	auto vertexPoolSize = vk::DescriptorPoolSize()
		.setDescriptorCount(kMaxDescriptorCount)
		.setType(vk::DescriptorType::eUniformBuffer);

	auto samplerPoolSize = vk::DescriptorPoolSize()
		.setDescriptorCount(kMaxDescriptorCount)
		.setType(vk::DescriptorType::eCombinedImageSampler);

	vk::DescriptorPoolSize poolSizes[] = { vertexPoolSize, samplerPoolSize };

	auto createInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizeCount(static_cast<std::uint32_t>(std::size(poolSizes)))
		.setPPoolSizes(poolSizes)
		.setMaxSets(kMaxDescriptorSets)
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	mHandle = device.Handle().createDescriptorPoolUnique(createInfo);

	CreateDescriptorSetLayout();
}

void VulkanDescriptorPool::CreateDescriptorSets(
	std::size_t count,
	const std::vector<std::unique_ptr<VulkanUniformBuffer>>& uniformBuffers,
	VulkanImage& texture,
	VulkanSampler& sampler)
{
	std::vector<vk::DescriptorSetLayout> layouts(count, mDescriptorSetLayout.get());

	auto allocateInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(Handle())
		.setDescriptorSetCount(static_cast<std::uint32_t>(count))
		.setPSetLayouts(layouts.data());

	mDescriptorSets = mDevice.Handle().allocateDescriptorSetsUnique(allocateInfo);

	for (std::size_t i = 0; i < count; i++)
	{
		// Vertex
		auto bufferInfo = vk::DescriptorBufferInfo()
			.setBuffer(uniformBuffers.at(i)->Handle())
			.setOffset(0)
			.setRange(sizeof(Core::UniformBufferObject));

		auto bufferDescriptorWrite = vk::WriteDescriptorSet()
			.setDstSet(mDescriptorSets.at(i).get())
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo(&bufferInfo);

		// Sampler
		auto imageInfo = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(texture.GetImageView())
			.setSampler(sampler.Handle());

		auto imageDescriptorWrite = vk::WriteDescriptorSet()
			.setDstSet(mDescriptorSets.at(i).get())
			.setDstBinding(1)
			.setDstArrayElement(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setPImageInfo(&imageInfo);

		mDevice.Handle().updateDescriptorSets({ bufferDescriptorWrite, imageDescriptorWrite }, {});
	}
}

void VulkanDescriptorPool::CreateDescriptorSetLayout()
{
	auto vertexLayoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	auto samplerLayoutBinding = vk::DescriptorSetLayoutBinding()
		.setBinding(1)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setDescriptorCount(1)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutBinding bindings[] = { vertexLayoutBinding, samplerLayoutBinding };

	auto createInfo = vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(static_cast<std::uint32_t>(std::size(bindings)))
		.setPBindings(bindings);

	mDescriptorSetLayout = mDevice.Handle().createDescriptorSetLayoutUnique(createInfo);
}

const vk::DescriptorSet& VulkanDescriptorPool::GetDescriptorSet(std::size_t index) const 
{ 
	return mDescriptorSets.at(index).get(); 
}

}
