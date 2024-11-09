#include "SceneNode.h"

#include <string>

namespace Lucid::Core::Scene
{

Node::Node(const std::string& name, NodePtr parent)
    : mName(name)
    , mParent(parent)
{
    static std::size_t idGenerator = 0;
    mId = idGenerator++;
}

std::size_t
Node::GetId() const
{
    return mId;
}

const std::string&
Node::GetName() const
{
    return mName;
}

const std::vector<NodePtr>&
Node::GetChildren() const
{
    return mChildren;
}

NodePtr
Node::GetParent() const
{
    if (mParent.expired())
    {
        return nullptr;
    }

    return mParent.lock();
}

glm::mat4
Node::GetTransform() const
{
    if (NodePtr parent = GetParent(); parent != nullptr)
    {
        return parent->GetTransform() * mTransform;
    }
    else
    {
        return mTransform;
    }
}

const std::optional<MeshPtr>&
Node::GetOptionalMesh() const
{
    return mMesh;
}

const std::optional<CameraPtr>&
Node::GetOptionalCamera() const
{
    return mCamera;
}

void
Node::AddChildren(NodePtr& node)
{
    mChildren.push_back(node);
}

void
Node::SetTransform(const glm::mat4& transform)
{
    mTransform = transform;
}

void
Node::SetMesh(const MeshPtr& mesh)
{
    if (mesh == nullptr)
    {
        return;
    }

    mMesh = mesh;
}

void
Node::SetCamera(const CameraPtr& camera)
{
    if (camera == nullptr)
    {
        return;
    }

    mCamera = camera;
}

} // namespace Lucid::Core::Scene
