#include "Files.h"

#include <fstream>

namespace Lucid
{

std::vector<char> Files::Read(const std::filesystem::path& path)
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
