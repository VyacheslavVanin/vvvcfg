#include <fstream>
#include <iostream>

#include "node_parser/node_parser.hpp"

int main(int argc, char** argv) try
{
    (void)argc;
    (void)argv;
    std::ifstream f("myconfigsample.cfg");
    CfgNode root = make_cfg(f);

    std::cout << root;
}
catch (std::exception& e) {
    std::cerr << e.what() << "\n";
} catch (...) {
    std::cerr << "Unexpected exception\n";
}
