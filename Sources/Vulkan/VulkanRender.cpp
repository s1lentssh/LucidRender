#include "VulkanRender.h"

namespace lucid {

VulkanRender::VulkanRender(const IWindow& window)
{
	mInstance = std::make_unique<VulkanInstance>(window.GetRequiredInstanceExtensions());

	mSurface = std::make_unique<VulkanSurface>(*mInstance.get(), window.GetHandle());

	mDevice = std::make_unique<VulkanDevice>(mInstance->PickSuitableDeviceForSurface(*mSurface.get()));
	mDevice->InitLogicalDeviceForSurface(*mSurface.get());

	mSwapchain = std::make_unique<VulkanSwapchain>(*mDevice.get(), *mSurface.get(), window);

	mRenderPass = std::make_unique<VulkanRenderPass>(*mDevice.get(), *mSwapchain.get());

	mPipeline = std::make_unique<VulkanPipeline>(*mDevice.get(), *mSwapchain.get(), *mRenderPass.get());
}

}
