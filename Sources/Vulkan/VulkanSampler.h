#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;

class VulkanSampler : public VulkanEntity<vk::UniqueSampler>
{
public:
    VulkanSampler(VulkanDevice& device, std::uint32_t mipLevels, const std::string& name);
};

} // namespace Lucid::Vulkan
