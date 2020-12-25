#include "VulkanRender.h"

#include <Utils/Defaults.hpp>
#include <Utils/Logger.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Vulkan
{

VulkanRender::VulkanRender(const IWindow& window) : mWindow(&window)
{
	// Create instance
	mInstance = std::make_unique<VulkanInstance>(window.GetRequiredInstanceExtensions());

	// Create surface
	mSurface = std::make_unique<VulkanSurface>(*mInstance.get(), window);

	// Create and init device
	mDevice = std::make_unique<VulkanDevice>(mInstance->PickSuitableDeviceForSurface(*mSurface.get()));
	mDevice->InitLogicalDeviceForSurface(*mSurface.get());

	RecreateSwapchain();

	// Create semaphores
	auto semaphoreCreateInfo = vk::SemaphoreCreateInfo();

	auto fenceCreateInfo = vk::FenceCreateInfo()
		.setFlags(vk::FenceCreateFlagBits::eSignaled);
	
	for (std::uint32_t i = 0; i < Defaults::MaxFramesInFlight; i++)
	{
		mImagePresentedSemaphores.push_back(mDevice->Handle().createSemaphoreUnique(semaphoreCreateInfo));
		mRenderFinishedSemaphores.push_back(mDevice->Handle().createSemaphoreUnique(semaphoreCreateInfo));
		mInFlightFences.push_back(mDevice->Handle().createFenceUnique(fenceCreateInfo));
	}

	mImagesInFlight.resize(Defaults::MaxFramesInFlight, {});
}

void VulkanRender::DrawFrame()
{
	// Render frame
	mDevice->Handle().waitForFences(mInFlightFences[mCurrentFrame].get(), true, std::numeric_limits<std::uint64_t>::max());

	vk::ResultValue acquireResult = mSwapchain->AcquireNextImage(mImagePresentedSemaphores[mCurrentFrame]);

	if (vk::Result::eErrorOutOfDateKHR == acquireResult.result)
	{
		RecreateSwapchain();
	}
	else if (vk::Result::eSuccess != acquireResult.result && vk::Result::eSuboptimalKHR != acquireResult.result)
	{
		throw std::runtime_error("Error during render");
	}

	std::uint32_t imageIndex = acquireResult.value;

	UpdateUniformBuffer(imageIndex);

	// Fix if max frames in flight greater than swapchain image count or if aquire returns out of order
	if (mImagesInFlight[imageIndex])
	{
		mDevice->Handle().waitForFences(mImagesInFlight[imageIndex], true, std::numeric_limits<std::uint64_t>::max());
	}

	mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame].get();

	vk::Semaphore waitSemaphores[] = { mImagePresentedSemaphores[mCurrentFrame].get() };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::Semaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame].get() };

	auto submitInfo = vk::SubmitInfo()
		.setWaitSemaphoreCount(static_cast<std::uint32_t>(std::size(waitSemaphores)))
		.setPWaitSemaphores(waitSemaphores)
		.setPWaitDstStageMask(waitStages)
		.setCommandBufferCount(1)
		.setPCommandBuffers(&mCommandPool->Get(imageIndex).get())
		.setSignalSemaphoreCount(static_cast<std::uint32_t>(std::size(signalSemaphores)))
		.setPSignalSemaphores(signalSemaphores);

	mDevice->Handle().resetFences(mInFlightFences[mCurrentFrame].get());
	mDevice->GetGraphicsQueue().submit(submitInfo, mInFlightFences[mCurrentFrame].get());

	// Present frame
	vk::SwapchainKHR swapchains[] = { mSwapchain->Handle() };

	auto presentInfo = vk::PresentInfoKHR()
		.setSwapchainCount(static_cast<std::uint32_t>(std::size(swapchains)))
		.setPSwapchains(swapchains)
		.setPImageIndices(&imageIndex)
		.setWaitSemaphoreCount(static_cast<std::uint32_t>(std::size(signalSemaphores)))
		.setPWaitSemaphores(signalSemaphores);

	vk::Result presentResult = mDevice->GetPresentQueue().presentKHR(presentInfo);

	if (vk::Result::eErrorOutOfDateKHR == presentResult || vk::Result::eSuboptimalKHR == presentResult)
	{
		RecreateSwapchain();
	}
	else if (vk::Result::eSuccess != presentResult)
	{
		throw std::runtime_error("Error during present");
	}

	mCurrentFrame = (mCurrentFrame + 1) % Defaults::MaxFramesInFlight;
}

void VulkanRender::RecreateSwapchain()
{
	Logger::Action("Swapchain recreation");

	mDevice->Handle().waitIdle();

	while (mWindow->GetSize().x == 0 || mWindow->GetSize().y == 0)
	{
		mWindow->WaitEvents();
	}

	// Create swapchain
	mSwapchain = std::make_unique<VulkanSwapchain>(*mDevice.get(), *mSurface.get(), mWindow->GetSize());

	// Create render pass
	mRenderPass = std::make_unique<VulkanRenderPass>(*mDevice.get(), mSwapchain->GetImageFormat());

	// Create framebuffers for swapchain
	mSwapchain->CreateFramebuffers(*mRenderPass.get());

	// Create descriptor pool
	mDescriptorPool = std::make_unique<VulkanDescriptorPool>(*mDevice.get());

	// Create pipeline
	mPipeline = std::make_unique<VulkanPipeline>(*mDevice.get(), mSwapchain->GetExtent(), *mRenderPass.get(), *mDescriptorPool.get());

	// Create command pool
	mCommandPool = std::make_unique<VulkanCommandPool>(*mDevice.get(), *mRenderPass.get(), *mSwapchain.get(), *mPipeline.get());

	// Create vertex buffer
	mVertexBuffer = std::make_unique<VulkanVertexBuffer>(*mDevice.get(), *mCommandPool.get(), mVertices);

	// Create index buffer
	mIndexBuffer = std::make_unique<VulkanIndexBuffer>(*mDevice.get(), *mCommandPool.get(), mIndices);

	// Create uniform buffers
	for (std::size_t i = 0; i < mSwapchain->GetImageCount(); i++)
	{
		mUniformBuffers.emplace_back(std::make_unique<VulkanUniformBuffer>(*mDevice.get()));
	}

	// Create descriptor sets
	mDescriptorPool->CreateDescriptorSets(mSwapchain->GetImageCount(), mUniformBuffers);

	// Record command buffers
	mCommandPool->RecordCommandBuffers(*mVertexBuffer.get(), *mIndexBuffer.get(), *mDescriptorPool.get());
}

void VulkanRender::UpdateUniformBuffer(std::uint32_t imageIndex)
{
	static auto startTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vk::Extent2D extent = mSwapchain->GetExtent();
	float aspectRatio = extent.width / (float)extent.height;

	UniformBufferObject ubo;
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
	ubo.projection[1][1] *= -1;

	mUniformBuffers.at(imageIndex)->Write(&ubo);
}

}
