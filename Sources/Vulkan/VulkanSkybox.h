#pragma once

#include <Vulkan/VulkanSampler.h>

namespace Lucid::Vulkan
{

class VulkanSkybox
{
public:
    VulkanSkybox();

private:
    VulkanSampler mSampler;
};

} // namespace Lucid::Vulkan
