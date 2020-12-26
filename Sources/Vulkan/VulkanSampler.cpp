#include "VulkanSampler.h"

#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanSampler::VulkanSampler(VulkanDevice& device)
{
	vk::PhysicalDeviceProperties properties = device.GetPhysicalDevice().getProperties();

	auto createInfo = vk::SamplerCreateInfo()
		.setMagFilter(vk::Filter::eLinear)
		.setMinFilter(vk::Filter::eLinear)
		.setAddressModeU(vk::SamplerAddressMode::eRepeat)
		.setAddressModeV(vk::SamplerAddressMode::eRepeat)
		.setAddressModeW(vk::SamplerAddressMode::eRepeat)
		.setAnisotropyEnable(true)
		.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
		.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
		.setUnnormalizedCoordinates(false)
		.setCompareEnable(false)
		.setCompareOp(vk::CompareOp::eAlways)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear)
		.setMipLodBias(0.0f)
		.setMinLod(0.0f)
		.setMaxLod(0.0f);

	mHandle = device.Handle().createSamplerUnique(createInfo);
}

}