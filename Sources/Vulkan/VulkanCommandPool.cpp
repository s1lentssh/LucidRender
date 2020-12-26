#include "VulkanCommandPool.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Utils/Logger.hpp>
#include <Utils/Defaults.hpp>

namespace Lucid::Vulkan
{

VulkanCommandPool::VulkanCommandPool(
	VulkanDevice& device, 
	VulkanRenderPass& renderPass, 
	VulkanSwapchain& swapchain, 
	VulkanPipeline& pipeline)
	: mSwapchain(swapchain)
	, mRenderPass(renderPass)
	, mPipeline(pipeline)
	, mDevice(device)
{
	// Create command pool
	auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(device.FindGraphicsQueueFamily().value());

	mHandle = device.Handle().createCommandPoolUnique(commandPoolCreateInfo);

	std::size_t imageCount = swapchain.GetFramebuffers().size();

	// Allocate command buffers
	auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(static_cast<std::uint32_t>(imageCount))
		.setCommandPool(Handle())
		.setLevel(vk::CommandBufferLevel::ePrimary);

	mCommandBuffers = device.Handle().allocateCommandBuffersUnique(commandBufferAllocateInfo);

	Logger::Info("Command buffers allocated");
}

vk::UniqueCommandBuffer& VulkanCommandPool::Get(std::size_t index) 
{ 
	return mCommandBuffers.at(index); 
}

void VulkanCommandPool::RecordCommandBuffers(const VulkanVertexBuffer& vertexBuffer, const VulkanIndexBuffer& indexBuffer, const VulkanDescriptorPool& descriptorPool)
{
	std::size_t imageCount = mSwapchain.GetFramebuffers().size();

	for (std::uint32_t i = 0; i < imageCount; i++)
	{
		vk::UniqueCommandBuffer& commandBuffer = mCommandBuffers.at(i);
		auto commandBufferBeginInfo = vk::CommandBufferBeginInfo();
		commandBuffer->begin(commandBufferBeginInfo);

		vk::ClearValue clearColor = vk::ClearColorValue(Defaults::BackgroundColor);
		vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

		vk::ClearValue clearValues[] = { clearColor, clearDepth };

		auto renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(mRenderPass.Handle())
			.setFramebuffer(mSwapchain.GetFramebuffers().at(i).get())
			.setRenderArea(vk::Rect2D()
				.setOffset({ 0, 0 })
				.setExtent(mSwapchain.GetExtent()))
			.setClearValueCount(std::size(clearValues))
			.setPClearValues(clearValues);

		commandBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
		commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.Handle());
		vk::Buffer vertexBuffers[] = { vertexBuffer.Handle() };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffer->bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffer->bindIndexBuffer(indexBuffer.Handle(), 0, vk::IndexType::eUint16);
		commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipeline.Layout(), 0, 1, &descriptorPool.GetDescriptorSet(i), 0, {});
		commandBuffer->drawIndexed(static_cast<std::uint32_t>(indexBuffer.IndicesCount()), 1, 0, 0, 0);
		commandBuffer->endRenderPass();

		commandBuffer->end();
	}
}

void VulkanCommandPool::ExecuteSingleCommand(const std::function<void(vk::CommandBuffer&)>& function)
{
	auto allocateInfo = vk::CommandBufferAllocateInfo()
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(Handle())
		.setCommandBufferCount(1);

	auto commandBuffers = mDevice.Handle().allocateCommandBuffersUnique(allocateInfo);
	vk::CommandBuffer commandBuffer = commandBuffers.at(0).get();

	auto beginInfo = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	commandBuffer.begin(beginInfo);
	function(commandBuffer);
	commandBuffer.end();

	auto submitInfo = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&commandBuffer);

	mDevice.GetGraphicsQueue().submit(submitInfo, {});
	mDevice.GetGraphicsQueue().waitIdle();
}

}