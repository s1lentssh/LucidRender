#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"

namespace lucid
{

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice& device, VulkanSwapchain& swapchain, VulkanRenderPass& renderPass);

private:
	void Init();
	void CreateFramebuffers();
	void CreateCommandBuffers();

	VulkanDevice& mDevice;
	VulkanSwapchain& mSwapchain;
	VulkanRenderPass& mRenderPass;

	vk::UniquePipelineLayout mLayout;
	vk::UniquePipeline mPipeline;
	std::vector<vk::UniqueFramebuffer> mFramebuffers;

	vk::UniqueCommandPool mCommandPool;
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

}