#include "VulkanShader.h"

#include <Vulkan/VulkanDevice.h>
#include <Utils/Files.h>

namespace Lucid
{

VulkanShader::VulkanShader(VulkanDevice& device, const std::filesystem::path& path)
{
	std::vector<char> code = Files::Read(path);

	auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(code.size())
		.setPCode(reinterpret_cast<const std::uint32_t*>(code.data()));

	mModule = device.Handle()->createShaderModuleUnique(shaderModuleCreateInfo);
}

}
