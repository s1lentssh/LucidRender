#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lucid::Core 
{

class Entity
{
public:
    Entity();
    Entity(const glm::mat4& transform);

    const glm::mat4& Transform() const;

protected:
    glm::mat4 mTransofrm;
};

}