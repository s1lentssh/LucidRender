#include "VulkanRenderPass.h"
#include <Utils/Logger.hpp>

namespace lucid
{

VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, const VulkanSwapchain& swapchain)
{
	/* 
		Create subpass 
	*/
	auto colorAttachmentReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	// Referenced directly from shader layout(location={index})
	auto subpass = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttachmentReference);

	/*
		Create render pass
	*/
	auto colorAttachment = vk::AttachmentDescription()
		.setFormat(swapchain.GetImageFormat())
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
		.setPSubpasses(&subpass);

	mRenderPass = device.Handle()->createRenderPassUnique(renderPassCreateInfo);
	Logger::Info("Render pass created");
}

}
