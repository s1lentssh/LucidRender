#pragma once

#include <string>
#include <vector>

#include <Core/Entity.h>
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

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};

struct Node
{
    std::string name;
    std::vector<std::shared_ptr<Node>> children;
    std::weak_ptr<Node> parent;
    std::shared_ptr<Mesh> mesh;

    static void PrintTree(std::ostream& os, const Node& node, std::string prefix = "", bool isLastChild = true);

    friend std::ostream& operator<<(std::ostream&, const Node&);
};

struct Texture
{
    Vector2d<std::uint32_t> size;
    std::vector<char> pixels;
    std::uint32_t mipLevels;
};

class Asset : public Entity
{
public:
    Asset(Mesh mesh, Texture texture)
        : Entity()
        , mMesh(mesh)
        , mTexture(texture)
    {
    }
    Asset(const Asset& rhs)
        : Entity(rhs)
        , mMesh(rhs.mMesh)
        , mTexture(rhs.mTexture)
    {
    }

    void SetPosition(const glm::vec3& position) { mTransform = glm::translate(glm::mat4(1), position); }

    [[nodiscard]] Mesh GetMesh() const { return mMesh; }

    [[nodiscard]] Texture GetTexture() const { return mTexture; }

private:
    Mesh mMesh;
    Texture mTexture;
};

} // namespace Lucid::Core
