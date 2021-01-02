#include "Entity.h"

namespace Lucid::Core
{

Entity::Entity()
	: mTransofrm(glm::mat4(1.0f))
{}

Lucid::Core::Entity::Entity(const glm::mat4& transform)
	: mTransofrm(transform)
{}

const glm::mat4& Entity::Transform() const
{
	return mTransofrm;
}

}
