#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lucid::Core
{

struct Light
{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

} // namespace Lucid::Core
