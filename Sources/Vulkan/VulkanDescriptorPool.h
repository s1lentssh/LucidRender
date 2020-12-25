#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanUniformBuffer;
class VulkanSampler;
class VulkanImageView;

class VulkanDescriptorPool : public VulkanEntity<vk::UniqueDescriptorPool>
{
public:
	VulkanDescriptorPool(VulkanDevice& device);

	void CreateDescriptorSets(
		std::size_t count, 
		const std::vector<std::unique_ptr<VulkanUniformBuffer>>& uniformBuffers,
		VulkanImageView& texture,
		VulkanSampler& sampler);

	void CreateDescriptorSetLayout();
	[[nodiscard]] const vk::DescriptorSet& GetDescriptorSet(std::size_t index) const;
	[[nodiscard]] vk::DescriptorSetLayout& Layout() { return mDescriptorSetLayout.get(); }

private:
	VulkanDevice& mDevice;

	std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
	vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
};

}