#pragma once

#include <vulkan/vulkan.hpp>

#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanSurface.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanCommandManager.h>
#include <Utils/Interfaces.hpp>

namespace lucid {

class VulkanDevice;

class VulkanRender
{
public:
	VulkanRender(const IWindow& window);
	void DrawFrame();

private:
	std::unique_ptr<VulkanInstance> mInstance;
	std::unique_ptr<VulkanDevice> mDevice;
	std::unique_ptr<VulkanSurface> mSurface;
	std::unique_ptr<VulkanSwapchain> mSwapchain;
	std::unique_ptr<VulkanRenderPass> mRenderPass;
	std::unique_ptr<VulkanPipeline> mPipeline;
	std::unique_ptr<VulkanCommandManager> mCommandManager;

	vk::UniqueSemaphore mImagePresentedSemaphore;
	vk::UniqueSemaphore mRenderFinishedSemaphore;
};

}