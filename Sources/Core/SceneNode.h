#pragma once

#include <memory>
#include <optional>

#include "Types.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Core
{

class SceneNode;
using SceneNodePtr = std::shared_ptr<SceneNode>;
using SceneNodeWeakPtr = std::weak_ptr<SceneNode>;

class SceneNode
{
    SceneNode(const std::string& name, SceneNodePtr parent);

public:
    template <typename... Args> static std::shared_ptr<SceneNode> Create(Args&&... args)
    {
        return std::shared_ptr<SceneNode> { new SceneNode { std::forward<Args>(args)... } };
    }

    // Getters
    std::size_t GetId() const;
    const std::string& GetName() const;
    const std::vector<SceneNodePtr>& GetChildren() const;
    SceneNodePtr GetParent() const;
    glm::mat4 GetTransform() const;

    const std::optional<MeshPtr>& GetOptionalMesh() const;

    // Setters
    void AddChildren(SceneNodePtr& node);
    void SetTransform(const glm::mat4& transform);
    void SetMesh(const MeshPtr& mesh);

private:
    // Must have fields
    std::string mName;
    std::vector<SceneNodePtr> mChildren;
    SceneNodeWeakPtr mParent;
    glm::mat4 mTransform { 1.0f };

    // Optional fields
    std::optional<MeshPtr> mMesh;

    // Utility fields
    std::size_t mId;
};

} // namespace Lucid::Core
