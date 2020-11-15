#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanDevice.h>

namespace lucid
{

class VulkanShader
{
public:
	VulkanShader(VulkanDevice& device, const std::filesystem::path& path);

	vk::UniqueShaderModule& Handle() { return mModule; }

private:
	std::vector<char> ReadFile(const std::filesystem::path& path);

	vk::UniqueShaderModule mModule;
};

}