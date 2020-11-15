#include "VulkanCommandManager.h"

#include <Utils/Logger.hpp>
#include <Utils/Defaults.hpp>

namespace lucid
{

VulkanCommandManager::VulkanCommandManager(VulkanDevice& device, VulkanPipeline& pipeline, VulkanRenderPass& renderPass, VulkanSwapchain& swapchain)
{
	auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(device.FindGraphicsQueueFamily().value());

	mCommandPool = device.Handle()->createCommandPoolUnique(commandPoolCreateInfo);

	Logger::Info("Command pool created");

	std::size_t imageCount = pipeline.GetFramebuffers().size();

	auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(imageCount)
		.setCommandPool(mCommandPool.get())
		.setLevel(vk::CommandBufferLevel::ePrimary);

	mCommandBuffers = device.Handle()->allocateCommandBuffersUnique(commandBufferAllocateInfo);

	Logger::Info("Command buffers allocated");

	for (std::uint32_t i = 0; i < imageCount; i++)
	{
		vk::UniqueCommandBuffer& commandBuffer = mCommandBuffers.at(i);
		auto commandBufferBeginInfo = vk::CommandBufferBeginInfo();
		commandBuffer->begin(commandBufferBeginInfo);

		vk::ClearValue clearColor = Defaults::BackgroundColor;

		auto renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(renderPass.Handle().get())
			.setFramebuffer(pipeline.GetFramebuffers().at(i).get())
			.setRenderArea(vk::Rect2D()
				.setOffset({ 0, 0 })
				.setExtent(swapchain.GetExtent()))
			.setClearValueCount(1)
			.setPClearValues(&clearColor);
		
		commandBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
		commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.Handle().get());
		commandBuffer->draw(3, 1, 0, 0);
		commandBuffer->endRenderPass();

		commandBuffer->end();
	}
}

}