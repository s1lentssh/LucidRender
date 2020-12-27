#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanSurface.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDescriptorPool.h>
#include <Vulkan/VulkanImage.h>
#include <Vulkan/VulkanSampler.h>
#include <Core/Interfaces.h>
#include <Core/Mesh.h>

namespace Lucid::Vulkan
{

class VulkanRender : public Core::IRender
{
public:
	VulkanRender(const Core::IWindow& window);
	void DrawFrame() override;

private:
	void RecreateSwapchain();
	void UpdateUniformBuffer(std::uint32_t imageIndex);

	// Vulkan entities
	std::unique_ptr<VulkanInstance> mInstance;
	std::unique_ptr<VulkanDevice> mDevice;
	std::unique_ptr<VulkanSurface> mSurface;
	std::unique_ptr<VulkanSwapchain> mSwapchain;
	std::unique_ptr<VulkanRenderPass> mRenderPass;
	std::unique_ptr<VulkanPipeline> mPipeline;
	std::unique_ptr<VulkanCommandPool> mCommandPool;
	std::unique_ptr<VulkanVertexBuffer> mVertexBuffer;
	std::unique_ptr<VulkanIndexBuffer> mIndexBuffer;
	std::unique_ptr<VulkanDescriptorPool> mDescriptorPool;
	std::unique_ptr<VulkanSampler> mSampler;
	std::unique_ptr<VulkanImage> mResolveImage;
	std::unique_ptr<VulkanImage> mTextureImage;
	std::unique_ptr<VulkanImage> mDepthImage;
	std::vector<std::unique_ptr<VulkanUniformBuffer>> mUniformBuffers;

	// Synchronization
	std::vector<vk::UniqueSemaphore> mImagePresentedSemaphores;
	std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores;
	std::vector<vk::UniqueFence> mInFlightFences;
	std::vector<vk::Fence> mImagesInFlight;

	std::size_t mCurrentFrame = 0;
	const Core::IWindow* mWindow = nullptr;

	Core::Mesh mMesh;
};

}