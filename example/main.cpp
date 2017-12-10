#include <fstream>
#include <iostream>

#include <vvvcfg/vvvcfg.hpp>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    std::ifstream f("myconfigsample.cfg");
    vvv::CfgNode root = vvv::make_cfg(f);

    std::cout << root;
    std::cout << root.getChild(std::vector<std::string>{"view", "l1", "text1"});
    std::cout << root.getChild({"view", "l1", "line1"});
    std::cout << root.getChild("view.l1.line1");
    std::cout << root.getChild("aaa").getValue().asString() << "\n";
    std::cout << root.getChild("view.l1.text1").getProperty("font").asString() << "\n";
    std::cout << "\nList example\n";
    auto list = root.getChild("list_ex").getValue().asList();
    for (const auto& s: list)
        std::cout << s << "\n";
}
