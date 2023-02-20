#include "VulkanRenderPass.h"

#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Utils/Logger.hpp>

namespace Lucid::Vulkan
{

VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, vk::Format imageFormat)
{
	// Create subpass 
	auto colorAttachmentReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	auto depthAttachmentReference = vk::AttachmentReference()
		.setAttachment(1)
		.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	auto resolveAttachmentReference = vk::AttachmentReference()
		.setAttachment(2)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	// Referenced directly from shader layout(location={index})
	auto subpass = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttachmentReference)
		.setPDepthStencilAttachment(&depthAttachmentReference)
		.setPResolveAttachments(&resolveAttachmentReference);

	// Create render pass
	auto subpassDependency = vk::SubpassDependency()
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setSrcAccessMask({})
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	auto colorAttachment = vk::AttachmentDescription()
		.setFormat(imageFormat)
		.setSamples(device.GetMsaaSamples())
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

	auto depthAttachment = vk::AttachmentDescription()
		.setFormat(device.FindSupportedDepthFormat())
		.setSamples(device.GetMsaaSamples())
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	auto resolveAttachment = vk::AttachmentDescription()
		.setFormat(imageFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentDescription attachments[] = { colorAttachment, depthAttachment, resolveAttachment };

	auto renderPassCreateInfo = vk::RenderPassCreateInfo()
		.setAttachmentCount(static_cast<std::uint32_t>(std::size(attachments)))
		.setPAttachments(attachments)
		.setSubpassCount(1)
		.setPSubpasses(&subpass)
		.setDependencyCount(1)
		.setPDependencies(&subpassDependency);

	mHandle = device.Handle()->createRenderPassUnique(renderPassCreateInfo);
	LoggerInfo << "Render pass created";
}

}
