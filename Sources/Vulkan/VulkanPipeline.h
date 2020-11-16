#pragma once

#include <vulkan/vulkan.hpp>

namespace Lucid
{

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice& device, VulkanSwapchain& swapchain, VulkanRenderPass& renderPass);
	vk::UniquePipeline& Handle() { return mPipeline; }
	std::vector<vk::UniqueFramebuffer>& GetFramebuffers() { return mFramebuffers; }

private:
	void Init();
	void CreateFramebuffers();

	VulkanDevice& mDevice;
	VulkanSwapchain& mSwapchain;
	VulkanRenderPass& mRenderPass;

	vk::UniquePipelineLayout mLayout;
	vk::UniquePipeline mPipeline;
	std::vector<vk::UniqueFramebuffer> mFramebuffers;
};

}