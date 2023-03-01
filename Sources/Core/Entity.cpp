#include "Entity.h"

namespace Lucid::Core
{

Entity::Entity()
    : mTransform(glm::mat4(1.0f))
{
}

Lucid::Core::Entity::Entity(const glm::mat4& transform)
    : mTransform(transform)
{
}

const glm::mat4&
Entity::Transform() const
{
    return mTransform;
}

} // namespace Lucid::Core
