#include "Types.h"

#include <iomanip>
#include <iostream>

namespace Lucid::Core
{

void
Node::PrintTree(std::ostream& os, const Node& node, std::string prefix, bool isLastChild)
{
    os << prefix;
    os << (isLastChild ? "\\--" : "|--");
    os << node.name;
    os << " (";
    for (std::int32_t i = 0; i < 4; ++i)
    {
        for (std::int32_t j = 0; j < 4; ++j)
        {
            os << node.transform[j][i];
            if (i != 3 || j != 3)
            {
                os << ", ";
            }
        }
    }
    os << ")";
    os << std::endl;

    // enter the next tree level
    for (size_t i = 0; i < node.children.size(); i++)
    {
        bool isLast = (i == node.children.size() - 1);
        Node::PrintTree(os, *node.children[i].get(), prefix + (isLastChild ? "    " : "|   "), isLast);
    }
}

glm::mat4
Node::Transform() const
{
    if (!parent.expired())
    {
        return parent.lock()->Transform() * transform;
    }
    else
    {
        return transform;
    }
}

std::ostream&
operator<<(std::ostream& os, const Node& node)
{
    os << std::endl;
    Node::PrintTree(os, node);
    return os;
}

} // namespace Lucid::Core
