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
	enum class Type
	{
		Vertex,
		Fragment
	};

	VulkanShader(VulkanDevice& device, Type type);

	vk::UniqueShaderModule& Handle() { return mModule; }

private:
	std::vector<char> ReadFile(const std::filesystem::path& path);

	vk::UniqueShaderModule mModule;
};

}