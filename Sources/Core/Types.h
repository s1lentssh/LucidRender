#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <Core/Vertex.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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

struct Material
{
    std::string name;
    TexturePtr albedo;
    TexturePtr metallicRoughness;
    TexturePtr normal;

    float roughnessFactor = 1.0;
    float metalnessFactor = 0.0;
    glm::vec4 baseColorFactor = {};

    friend std::ostream& operator<<(std::ostream& os, const Material& material);
};

using MaterialPtr = std::shared_ptr<Material>;

struct Mesh
{
    std::string name;
    std::shared_ptr<Material> material;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};

using MeshPtr = std::shared_ptr<Mesh>;

struct Camera
{
    std::string name;
    double fov = 0.5;
};

using CameraPtr = std::shared_ptr<Camera>;

} // namespace Lucid::Core
