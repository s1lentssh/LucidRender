#pragma once

#include <vector>
#include <filesystem>

namespace Lucid
{

struct Texture
{
	std::uint32_t width;
	std::uint32_t height;
	std::vector<char> pixels;
};

class Files
{
public:
	static std::vector<char> Read(const std::filesystem::path& path);
	static Texture ReadImage(const std::filesystem::path& path);
};

}