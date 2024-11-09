#pragma once

#include <memory>
#include <optional>

#include "Types.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Core::Scene
{

class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeWeakPtr = std::weak_ptr<Node>;

class Node
{
    Node(const std::string& name, NodePtr parent);

public:
    template <typename... Args> static std::shared_ptr<Node> Create(Args&&... args)
    {
        return std::shared_ptr<Node> { new Node { std::forward<Args>(args)... } };
    }

    // Getters
    std::size_t GetId() const;
    const std::string& GetName() const;
    const std::vector<NodePtr>& GetChildren() const;
    NodePtr GetParent() const;
    glm::mat4 GetTransform() const;

    const std::optional<MeshPtr>& GetOptionalMesh() const;
    const std::optional<CameraPtr>& GetOptionalCamera() const;

    // Setters
    void AddChildren(NodePtr& node);
    void SetTransform(const glm::mat4& transform);
    void SetMesh(const MeshPtr& mesh);
    void SetCamera(const CameraPtr& camera);

private:
    // Must have fields
    std::string mName;
    std::vector<NodePtr> mChildren;
    NodeWeakPtr mParent;
    glm::mat4 mTransform { 1.0f };

    // Optional fields
    std::optional<MeshPtr> mMesh;
    std::optional<CameraPtr> mCamera;

    // Utility fields
    std::size_t mId;
};

} // namespace Lucid::Core::Scene
