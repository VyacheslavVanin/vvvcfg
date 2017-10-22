#include "cfg_node.hpp"
#include <iostream>

namespace {
void print_node(std::ostream& str, const CfgNode& node, int tab_width = 4,
                int lvl = 0)
{
    for (int i = 0; i < lvl * tab_width; ++i)
        str << " ";
    str << node.getName();
    for (auto p : node) {
        str << " " << p.first;
        if (!p.second.empty())
            str << " = \"" << p.second << "\"";
        str << ",";
    }
    str << "\n";
    for (size_t i = 0; i < node.getNumChildren(); ++i)
        print_node(str, node.getChild(i), tab_width, lvl + 1);
}
}; // namespace

std::ostream& operator<<(std::ostream& str, const CfgNode& node)
{
    print_node(str, node);
    return str;
}
