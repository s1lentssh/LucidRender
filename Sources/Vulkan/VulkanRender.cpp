#include "VulkanRender.h"

namespace Lucid {

VulkanRender::VulkanRender(const IWindow& window)
{
	// Basic initialization
	mInstance = std::make_unique<VulkanInstance>(window.GetRequiredInstanceExtensions());
	mSurface = std::make_unique<VulkanSurface>(*mInstance.get(), window);
	mDevice = std::make_unique<VulkanDevice>(mInstance->PickSuitableDeviceForSurface(*mSurface.get()));
	mDevice->InitLogicalDeviceForSurface(*mSurface.get());
	mSwapchain = std::make_unique<VulkanSwapchain>(*mDevice.get(), *mSurface.get(), window);
	mRenderPass = std::make_unique<VulkanRenderPass>(*mDevice.get(), *mSwapchain.get());
	mPipeline = std::make_unique<VulkanPipeline>(*mDevice.get(), *mSwapchain.get(), *mRenderPass.get());
	mCommandManager = std::make_unique<VulkanCommandManager>(*mDevice.get(), *mPipeline.get(), *mRenderPass.get(), *mSwapchain.get());

	// Create semaphores
	mImagePresentedSemaphore = mDevice->Handle()->createSemaphoreUnique({});
	mRenderFinishedSemaphore = mDevice->Handle()->createSemaphoreUnique({});
}

void VulkanRender::DrawFrame()
{
	// Render frame
	std::uint32_t imageIndex = mSwapchain->AcquireNextImage(mImagePresentedSemaphore);

	vk::Semaphore waitSemaphores[] = { mImagePresentedSemaphore.get() };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

	vk::Semaphore signalSemaphores[] = { mRenderFinishedSemaphore.get() };

	auto submitInfo = vk::SubmitInfo()
		.setWaitSemaphoreCount(std::size(waitSemaphores))
		.setPWaitSemaphores(waitSemaphores)
		.setPWaitDstStageMask(waitStages)
		.setCommandBufferCount(1)
		.setPCommandBuffers(&mCommandManager->Get(imageIndex).get())
		.setSignalSemaphoreCount(std::size(signalSemaphores))
		.setPSignalSemaphores(signalSemaphores);

	mDevice->GetGraphicsQueue().submit(submitInfo, {});

	// Present frame
	vk::SwapchainKHR swapchains[] = { mSwapchain->Handle().get() };

	auto presentInfo = vk::PresentInfoKHR()
		.setSwapchainCount(std::size(swapchains))
		.setPSwapchains(swapchains)
		.setPImageIndices(&imageIndex)
		.setWaitSemaphoreCount(std::size(signalSemaphores))
		.setPWaitSemaphores(signalSemaphores);

	mDevice->GetPresentQueue().presentKHR(presentInfo);
	mDevice->GetPresentQueue().waitIdle();
}

}
