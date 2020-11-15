#pragma once

#include <vulkan/vulkan.hpp>

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPass.h>

namespace lucid
{

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