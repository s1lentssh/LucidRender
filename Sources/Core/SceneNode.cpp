#include "SceneNode.h"

#include <string>

namespace Lucid::Core
{

SceneNode::SceneNode(const std::string& name, SceneNodePtr parent)
    : mName(name)
    , mParent(parent)
{
    static std::size_t idGenerator = 0;
    mId = idGenerator++;
}

std::size_t
SceneNode::GetId() const
{
    return mId;
}

const std::string&
SceneNode::GetName() const
{
    return mName;
}

const std::vector<SceneNodePtr>&
SceneNode::GetChildren() const
{
    return mChildren;
}

SceneNodePtr
SceneNode::GetParent() const
{
    if (mParent.expired())
    {
        return nullptr;
    }

    return mParent.lock();
}

glm::mat4
SceneNode::GetTransform() const
{
    if (SceneNodePtr parent = GetParent(); parent != nullptr)
    {
        return parent->GetTransform() * mTransform;
    }
    else
    {
        return mTransform;
    }
}

const std::optional<MeshPtr>&
SceneNode::GetOptionalMesh() const
{
    return mMesh;
}

void
SceneNode::AddChildren(SceneNodePtr& node)
{
    mChildren.push_back(node);
}

void
SceneNode::SetTransform(const glm::mat4& transform)
{
    mTransform = transform;
}

void
SceneNode::SetMesh(const MeshPtr& mesh)
{
    if (mesh == nullptr)
    {
        return;
    }

    mMesh = mesh;
}

} // namespace Lucid::Core
