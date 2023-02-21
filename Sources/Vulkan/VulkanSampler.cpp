#include "VulkanSampler.h"

#include <Vulkan/VulkanDevice.h>

namespace Lucid::Vulkan
{

VulkanSampler::VulkanSampler(VulkanDevice& device, std::uint32_t mipLevels)
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
                          .setBorderColor(vk::BorderColor::eIntOpaqueWhite)
                          .setUnnormalizedCoordinates(false)
                          .setCompareEnable(false)
                          .setCompareOp(vk::CompareOp::eAlways)
                          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                          .setMipLodBias(0.0f)
                          .setMinLod(0.0f)
                          .setMaxLod(static_cast<float>(mipLevels));

    mHandle = device.Handle()->createSamplerUnique(createInfo);
}

} // namespace Lucid::Vulkan
