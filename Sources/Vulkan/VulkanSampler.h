#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;

class VulkanSampler : public VulkanEntity<vk::UniqueSampler>
{
public:
	VulkanSampler(VulkanDevice& device, std::uint32_t mipLevels);
};

}
