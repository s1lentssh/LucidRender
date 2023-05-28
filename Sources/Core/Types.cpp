#include "Types.h"

#include <iostream>

namespace Lucid::Core
{

void
Node::PrintTree(std::ostream& os, const Node& node, std::string prefix, bool isLastChild)
{
    os << prefix;

    os << (isLastChild ? "\\--" : "|--");

    // print the value of the node
    os << node.name << std::endl;

    // enter the next tree level
    for (size_t i = 0; i < node.children.size(); i++)
    {
        bool isLast = (i == node.children.size() - 1);
        Node::PrintTree(os, *node.children[i].get(), prefix + (isLastChild ? "    " : "|   "), isLast);
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
