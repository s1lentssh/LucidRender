#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanUniformBuffer;
class VulkanSampler;
class VulkanImage;

class VulkanDescriptorPool : public VulkanEntity<vk::UniqueDescriptorPool>
{
public:
    VulkanDescriptorPool(VulkanDevice& device, const std::string& name);
    void CreateDescriptorSetLayout();

    [[nodiscard]] vk::DescriptorSetLayout& Layout() { return mDescriptorSetLayout.get(); }

private:
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
};

} // namespace Lucid::Vulkan
