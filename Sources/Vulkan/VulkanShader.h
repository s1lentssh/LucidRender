#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace Lucid
{

class VulkanDevice;

class VulkanShader
{
public:
	VulkanShader(VulkanDevice& device, const std::filesystem::path& path);

	vk::UniqueShaderModule& Handle() { return mModule; }

private:
	vk::UniqueShaderModule mModule;
};

}