#pragma once

#include <vulkan/vulkan.hpp>

namespace Lucid
{

class VulkanPipeline;
class VulkanSwapchain;
class VulkanDevice;

class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& device, vk::Format imageFormat);
	vk::RenderPass& Handle() { return mRenderPass.get(); }

private:
	vk::UniqueRenderPass mRenderPass;
};

}