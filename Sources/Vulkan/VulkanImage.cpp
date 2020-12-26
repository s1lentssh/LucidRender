#include "VulkanImage.h"

#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanCommandPool.h>
#include <Utils/Files.h>

namespace Lucid::Vulkan
{

VulkanImage::VulkanImage(VulkanDevice& device, VulkanCommandPool& commandPool, const std::filesystem::path& path)
	: mDevice(device)
{
	Texture image = Files::ReadImage(path);

	VulkanBuffer stagingBuffer(
		device,
		image.pixels.size(),
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	stagingBuffer.Write(image.pixels.data());

	auto createInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setExtent(vk::Extent3D()
			.setWidth(image.width)
			.setHeight(image.height)
			.setDepth(1))
		.setMipLevels(1)
		.setArrayLayers(1)
		.setFormat(vk::Format::eR8G8B8A8Srgb)
		.setTiling(vk::ImageTiling::eOptimal)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
		.setSharingMode(vk::SharingMode::eExclusive)
		.setSamples(vk::SampleCountFlagBits::e1);

	mUniqueImageHolder = device.Handle().createImageUnique(createInfo);
	mHandle = mUniqueImageHolder.value().get();

	vk::MemoryRequirements requirements = device.Handle().getImageMemoryRequirements(Handle());
	std::uint32_t memoryType = VulkanBuffer::FindMemoryType(device, requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

	auto allocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(requirements.size)
		.setMemoryTypeIndex(memoryType);

	mDeviceMemory = device.Handle().allocateMemoryUnique(allocateInfo);
	device.Handle().bindImageMemory(Handle(), mDeviceMemory.get(), 0);

	Transition(commandPool, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	Write(commandPool, stagingBuffer, image.width, image.height);
	Transition(commandPool, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

VulkanImage::VulkanImage(
	VulkanDevice& device, 
	std::uint32_t width, 
	std::uint32_t height, 
	vk::Format format, 
	vk::ImageTiling tiling, 
	vk::ImageUsageFlags usage, 
	vk::MemoryPropertyFlags memoryProperty)
	: mDevice(device)
{
	auto createInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setExtent(vk::Extent3D()
			.setWidth(width)
			.setHeight(height)
			.setDepth(1))
		.setMipLevels(1)
		.setArrayLayers(1)
		.setFormat(format)
		.setTiling(tiling)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive)
		.setSamples(vk::SampleCountFlagBits::e1);

	mUniqueImageHolder = device.Handle().createImageUnique(createInfo);
	mHandle = mUniqueImageHolder.value().get();

	vk::MemoryRequirements requirements = device.Handle().getImageMemoryRequirements(Handle());
	std::uint32_t memoryType = VulkanBuffer::FindMemoryType(device, requirements.memoryTypeBits, memoryProperty);

	auto allocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(requirements.size)
		.setMemoryTypeIndex(memoryType);

	mDeviceMemory = device.Handle().allocateMemoryUnique(allocateInfo);
	device.Handle().bindImageMemory(Handle(), mDeviceMemory.get(), 0);
}

VulkanImage::VulkanImage(VulkanDevice& device, vk::Image image)
	: mDevice(device)
{
	mHandle = image;
}

void VulkanImage::Transition(VulkanCommandPool& commandPool, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	commandPool.ExecuteSingleCommand([&, this](vk::CommandBuffer& commandBuffer) {
		auto barrier = vk::ImageMemoryBarrier()
			.setOldLayout(oldLayout)
			.setNewLayout(newLayout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(Handle())
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLayerCount(1)
				.setBaseArrayLayer(0)
				.setLevelCount(1));

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier
				.setSrcAccessMask({})
				.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier
				.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
				.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			throw std::runtime_error("Unsupported transition");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
	});
}

void VulkanImage::Write(VulkanCommandPool& commandPool, const VulkanBuffer& buffer, std::uint32_t width, std::uint32_t height)
{
	commandPool.ExecuteSingleCommand([&, this](vk::CommandBuffer& commandBuffer) {
		auto region = vk::BufferImageCopy()
			.setBufferOffset(0)
			.setBufferRowLength(0)
			.setBufferImageHeight(0)
			.setImageSubresource(vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setMipLevel(0)
				.setBaseArrayLayer(0)
				.setLayerCount(1))
			.setImageOffset({ 0, 0, 0 })
			.setImageExtent({ width, height, 1 });

		commandBuffer.copyBufferToImage(buffer.Handle(), Handle(), vk::ImageLayout::eTransferDstOptimal, region);
	});
}

void VulkanImage::CreateImageView(vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	auto imageViewCreateInfo = vk::ImageViewCreateInfo()
		.setImage(Handle())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(format)
		.setComponents(vk::ComponentMapping{}) // Identity by default
		.setSubresourceRange(vk::ImageSubresourceRange{}
			.setAspectMask(aspectFlags)
			.setBaseArrayLayer(0)
			.setBaseMipLevel(0)
			.setLayerCount(1)
			.setLevelCount(1));

	mImageView = mDevice.Handle().createImageViewUnique(imageViewCreateInfo);
}

bool VulkanImage::HasStencil(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

VulkanDepthImage::VulkanDepthImage(VulkanDevice& device, const vk::Extent2D& swapchainExtent)
	: VulkanImage(
		device,
		swapchainExtent.width,
		swapchainExtent.height,
		device.FindSupportedDepthFormat(),
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal)
{
}

}