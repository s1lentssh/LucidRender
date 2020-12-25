#include "Files.h"

#include <fstream>
#include <stb_image.h>

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
	std::vector<char> pixels(fileSize);

	file.seekg(0);
	file.read(pixels.data(), fileSize);
	file.close();

	return pixels;
}

Texture Files::ReadImage(const std::filesystem::path& path)
{
	int width, height, channels;
	stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

	/*if (channels != 4)
	{
		throw std::runtime_error("Detected texture with channels != 4");
	}*/

	std::size_t size = static_cast<std::size_t>(width) * height * 4;
	std::vector<char> result(size);
	std::memcpy(result.data(), pixels, size);

	return { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), result };
}

}
