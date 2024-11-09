#include "Types.h"

namespace Lucid::Core
{

std::ostream&
operator<<(std::ostream& os, const Material& material)
{
    os << "{ \"name\": " << material.name << ", \"color\": " << material.albedo->pixels.size()
       << ", \"metallic/roughness\": " << material.metallicRoughness->pixels.size() << "}";
    return os;
}

} // namespace Lucid::Core
