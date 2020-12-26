#pragma once

#include <vector>
#include <filesystem>
#include <Core/Mesh.h>
#include <Core/Texture.h>

namespace Lucid
{

class Files
{
public:
	static std::vector<char> Read(const std::filesystem::path& path);
	static Core::Texture ReadImage(const std::filesystem::path& path);
	static Core::Mesh LoadModel(const std::filesystem::path& path);
};

}