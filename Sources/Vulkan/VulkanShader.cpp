#include "VulkanShader.h"
#include <fstream>

namespace lucid
{

VulkanShader::VulkanShader(VulkanDevice& device, const std::filesystem::path& path)
{
	std::vector<char> code = ReadFile(path);

	auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(code.size())
		.setPCode(reinterpret_cast<const std::uint32_t*>(code.data()));

	mModule = device.Handle()->createShaderModuleUnique(shaderModuleCreateInfo);
}

std::vector<char> VulkanShader::ReadFile(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Can't open file: " + path.string());
	}

	std::size_t fileSize = file.tellg();
	std::vector<char> data(fileSize);

	file.seekg(0);
	file.read(data.data(), fileSize);
	file.close();

	return data;
}

}
