#pragma once

#include <vector>
#include <filesystem>

namespace Lucid
{

class Files
{
public:
	static std::vector<char> Read(const std::filesystem::path& path);
};

}