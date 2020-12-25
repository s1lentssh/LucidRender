#pragma once

#include <filesystem>
#include <map>

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;

class VulkanShader : public VulkanEntity<vk::UniqueShaderModule>
{
public:
	enum class Type
	{
		Vertex,
		Fragment
	};

	VulkanShader(VulkanDevice& device, Type type, const std::filesystem::path& path);

private:
	[[nodiscard]] std::string PreprocessShader(const std::string& source, Type type, const std::string& name);
	[[nodiscard]] std::vector<std::uint32_t> CompileShader(const std::string& source, Type type, const std::string& name);

	inline static const std::map<Type, shaderc_shader_kind> TypeMap{
		{ Type::Fragment, shaderc_shader_kind::shaderc_fragment_shader },
		{ Type::Vertex, shaderc_shader_kind::shaderc_vertex_shader },
	};
};

}