#include "VulkanCommandPool.h"

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanMesh.h>
#include <Utils/Logger.hpp>
#include <Utils/Defaults.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace Lucid::Vulkan
{

VulkanCommandPool::VulkanCommandPool(VulkanDevice& device)
	: mDevice(device)
{
	// Create command pool
	auto commandPoolCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(device.FindGraphicsQueueFamily().value());

	mHandle = device.Handle()->createCommandPoolUnique(commandPoolCreateInfo);
}

vk::UniqueCommandBuffer& VulkanCommandPool::Get(std::size_t index) 
{ 
	return mCommandBuffers.at(index); 
}

void VulkanCommandPool::RecreateCommandBuffers(VulkanSwapchain& swapchain)
{
	mCommandBuffers.clear();

	std::size_t imageCount = swapchain.GetFramebuffers().size();

	// Allocate command buffers
	auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(static_cast<std::uint32_t>(imageCount))
		.setCommandPool(Handle().get())
		.setLevel(vk::CommandBufferLevel::ePrimary);

	mCommandBuffers = mDevice.Handle()->allocateCommandBuffersUnique(commandBufferAllocateInfo);

	Logger::Info("Command buffers allocated");
}

void VulkanCommandPool::RecordCommandBuffers(VulkanPipeline& pipeline, VulkanSwapchain& swapchain, const VulkanRenderPass& renderPass, const std::vector<VulkanMesh>& meshes)
{
	Logger::Action("Record command buffers");

	std::size_t imageCount = swapchain.GetFramebuffers().size();

	for (std::uint32_t i = 0; i < imageCount; i++)
	{
		vk::UniqueCommandBuffer& commandBuffer = mCommandBuffers.at(i);
		auto commandBufferBeginInfo = vk::CommandBufferBeginInfo();
		commandBuffer->begin(commandBufferBeginInfo);

		vk::ClearValue clearColor = vk::ClearColorValue(Defaults::BackgroundColor);
		vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

		vk::ClearValue clearValues[] = { clearColor, clearDepth };

		auto renderPassBeginInfo = vk::RenderPassBeginInfo()
			.setRenderPass(renderPass.Handle().get())
			.setFramebuffer(swapchain.GetFramebuffers().at(i).get())
			.setRenderArea(vk::Rect2D()
				.setOffset({ 0, 0 })
				.setExtent(swapchain.GetExtent()))
			.setClearValueCount(static_cast<std::uint32_t>(std::size(clearValues)))
			.setPClearValues(clearValues);

		commandBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
		commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.Handle().get());

		Core::PushConstants constants;
    	constants.ambientColor = glm::make_vec3(Defaults::AmbientColor.data());
		constants.ambientFactor = 3.0f;
		constants.lightPosition = glm::vec3(10.0, 50.0, 0.0);
		constants.lightColor = glm::vec3(2.0, 2.0, 4.7);
		commandBuffer->pushConstants(pipeline.Layout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(Core::PushConstants), &constants);

		for (const VulkanMesh& mesh : meshes)
		{
			mesh.Draw(commandBuffer, pipeline);
		}

		commandBuffer->endRenderPass();
		commandBuffer->end();
	}
}

void VulkanCommandPool::ExecuteSingleCommand(const std::function<void(vk::CommandBuffer&)>& function)
{
	auto allocateInfo = vk::CommandBufferAllocateInfo()
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandPool(Handle().get())
		.setCommandBufferCount(1);

	auto commandBuffers = mDevice.Handle()->allocateCommandBuffersUnique(allocateInfo);
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