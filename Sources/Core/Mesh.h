#pragma once

#include <Core/Vertex.h>

namespace Lucid::Core 
{

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};

}