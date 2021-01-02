#include "VulkanRender.h"

#include <Utils/Defaults.hpp>
#include <Utils/Logger.hpp>
#include <Utils/Files.h>
#include <Core/UniformBufferObject.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Vulkan
{

VulkanRender::VulkanRender(const Core::IWindow& window, const Core::Scene& scene) : mWindow(&window), mScene(scene)
{
	// Create instance
	mInstance = std::make_unique<VulkanInstance>(window.GetRequiredInstanceExtensions());

	// Create surface
	mSurface = std::make_unique<VulkanSurface>(*mInstance.get(), window);

	// Create and init device
	mDevice = std::make_unique<VulkanDevice>(mInstance->PickSuitableDeviceForSurface(*mSurface.get()));
	mDevice->InitLogicalDeviceForSurface(*mSurface.get());

	// Create descriptor pool
	mDescriptorPool = std::make_unique<VulkanDescriptorPool>(*mDevice.get());

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

	// Create pipeline
	mPipeline = std::make_unique<VulkanPipeline>(*mDevice.get(), mSwapchain->GetExtent(), *mRenderPass.get(), *mDescriptorPool.get());

	// Create depth image
	mDepthImage = VulkanImage::CreateDepthImage(*mDevice.get(), mSwapchain->GetExtent(), mDevice->FindSupportedDepthFormat(), vk::ImageAspectFlagBits::eDepth);

	// Create MSAA RenderTarget
	mResolveImage = VulkanImage::CreateImage(*mDevice.get(), mSwapchain->GetImageFormat(), mSwapchain->GetExtent());

	// Create framebuffers for swapchain
	mSwapchain->CreateFramebuffers(*mRenderPass.get(), *mDepthImage.get(), *mResolveImage.get());

	// Create uniform buffers
	if (mUniformBuffers.size() != mSwapchain->GetImageCount())
	{
		mUniformBuffers.clear();
		for (std::size_t i = 0; i < mSwapchain->GetImageCount(); i++)
		{
			mUniformBuffers.emplace_back(std::make_unique<VulkanUniformBuffer>(*mDevice.get()));
		}
	}

	// Create command pool
	mCommandPool = std::make_unique<VulkanCommandPool>(*mDevice.get(), *mSwapchain.get(), *mPipeline.get());

	// Create vertex buffer
	mVertexBuffer = std::make_unique<VulkanVertexBuffer>(*mDevice.get(), *mCommandPool.get(), mScene.GetMeshDebug()->vertices);

	// Create index buffer
	mIndexBuffer = std::make_unique<VulkanIndexBuffer>(*mDevice.get(), *mCommandPool.get(), mScene.GetMeshDebug()->indices);

	// Load texture
	mTextureImage = VulkanImage::CreateImageFromResource(*mDevice.get(), *mCommandPool.get(), "Resources/Textures/VikingRoom.png", vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);

	// Create sampler
	mSampler = std::make_unique<VulkanSampler>(*mDevice.get(), mTextureImage->GetMipLevels());

	// Create descriptor sets
	mDescriptorPool->CreateDescriptorSets(mSwapchain->GetImageCount(), mUniformBuffers, *mTextureImage.get(), *mSampler.get());

	// Record command buffers
	mCommandPool->RecordCommandBuffers(*mRenderPass.get(), *mVertexBuffer.get(), *mIndexBuffer.get(), *mDescriptorPool.get());
}

void VulkanRender::UpdateUniformBuffer(std::uint32_t imageIndex)
{
	static auto startTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vk::Extent2D extent = mSwapchain->GetExtent();
	float aspectRatio = extent.width / (float)extent.height;

	Core::UniformBufferObject ubo;
	ubo.model = mScene.GetMeshDebug()->Transform();
	ubo.view = mScene.GetCamera()->Transform();
	ubo.projection = glm::perspective(glm::radians(mScene.GetCamera()->FieldOfView()), aspectRatio, 0.1f, 120.0f);
	ubo.projection[1][1] *= -1;

	mUniformBuffers.at(imageIndex)->Write(&ubo);
}

}
