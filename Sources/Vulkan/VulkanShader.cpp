#include "VulkanShader.h"

#include <Vulkan/VulkanDevice.h>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>

namespace Lucid::Vulkan
{

VulkanShader::VulkanShader(VulkanDevice& device, Type type, const std::filesystem::path& path)
{
	Logger::Info("Compiling {}", path.string());

	std::vector<char> code = Files::LoadFile(path);
	std::string preprocessed = PreprocessShader({ code.begin(), code.end() }, type, path.string());
	std::vector<std::uint32_t> compiled = CompileShader(preprocessed, type, path.string());

	auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(compiled.size() * sizeof(std::uint32_t))
		.setPCode(compiled.data());

	mHandle = device.Handle()->createShaderModuleUnique(shaderModuleCreateInfo);
}

std::string VulkanShader::PreprocessShader(const std::string& source, Type type, const std::string& name)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	shaderc_shader_kind kind = VulkanShader::TypeMap.at(type);

	shaderc::PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(source.data(), kind, name.data(), options);

	if (shaderc_compilation_status::shaderc_compilation_status_success != result.GetCompilationStatus())
	{
		throw std::runtime_error(result.GetErrorMessage());
	}

	return { result.cbegin(), result.cend() };
}

std::vector<std::uint32_t> VulkanShader::CompileShader(const std::string& source, Type type, const std::string& name)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_size);
	shaderc_shader_kind kind = VulkanShader::TypeMap.at(type);

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source.data(), kind, name.data(), options);
	if (shaderc_compilation_status::shaderc_compilation_status_success != result.GetCompilationStatus())
	{
		throw std::runtime_error(result.GetErrorMessage());
	}

	return { result.cbegin(), result.cend() };
}

}
