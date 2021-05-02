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

	mHandle = device.Handle()->createDescriptorPoolUnique(createInfo);

	CreateDescriptorSetLayout();
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

	mDescriptorSetLayout = mDevice.Handle()->createDescriptorSetLayoutUnique(createInfo);
}

}
