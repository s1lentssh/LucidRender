#pragma once

#include <Core/Vertex.h>
#include <Core/Entity.h>

namespace Lucid::Core 
{

struct Mesh : public Entity
{
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};

}