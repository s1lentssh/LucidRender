#pragma once

#include <vector>
#include <Core/Types.h>

namespace Lucid::Core
{

struct Texture
{
	Vector2d<std::uint32_t> size;
	std::vector<char> pixels;
};

}