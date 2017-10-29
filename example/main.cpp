#include <fstream>
#include <iostream>

#include <vvvcfg/node_parser.hpp>

int main(int argc, char** argv) try
{
    (void)argc;
    (void)argv;
    std::ifstream f("myconfigsample.cfg");
    CfgNode root = make_cfg(f);

    std::cout << root;
    std::cout << root.getChild(std::vector<std::string>{"view", "l1", "text1"});
    std::cout << root.getChild({"view", "l1", "line1"});
    std::cout << root.getChild("view.l1.line1");
    std::cout << root.getChild("aaa").getValue() << "\n";
}
catch (std::exception& e) {
    std::cerr << e.what() << "\n";
} catch (...) {
    std::cerr << "Unexpected exception\n";
}
