#include "VulkanRenderPass.h"

#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Utils/Logger.hpp>

namespace Lucid
{

VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, vk::Format imageFormat)
{
	// Create subpass 
	auto colorAttachmentReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	// Referenced directly from shader layout(location={index})
	auto subpass = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttachmentReference);

	// Create render pass
	auto subpassDependency = vk::SubpassDependency()
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask({})
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

	auto colorAttachment = vk::AttachmentDescription()
		.setFormat(imageFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	auto renderPassCreateInfo = vk::RenderPassCreateInfo()
		.setAttachmentCount(1)
		.setPAttachments(&colorAttachment)
		.setSubpassCount(1)
		.setPSubpasses(&subpass)
		.setDependencyCount(1)
		.setPDependencies(&subpassDependency);

	mRenderPass = device.Handle()->createRenderPassUnique(renderPassCreateInfo);
	Logger::Info("Render pass created");
}

}
