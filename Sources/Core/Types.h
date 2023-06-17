#pragma once

#include <string>
#include <vector>
#include <memory>

#include <Core/Vertex.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Core
{

template <typename T> struct Vector2d
{
    T x;
    T y;

    bool IsZero() const
    {
        return std::abs(x) <= std::numeric_limits<T>::epsilon() && std::abs(y) <= std::numeric_limits<T>::epsilon();
    }
};

struct Texture
{
    Vector2d<std::uint32_t> size;
    std::vector<unsigned char> pixels;
    std::uint32_t mipLevels = 1;
};

using TexturePtr = std::shared_ptr<Texture>;

struct Mesh
{
    std::shared_ptr<Texture> texture;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace Lucid::Core
