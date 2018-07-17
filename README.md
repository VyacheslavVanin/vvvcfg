# VVVCFG

C++ library to read my text config format

## Format description

### Features
Format features:
- hierarchical
- each node can store value, properties and subnodes.
- referencies to other nodes to copy value or properties and subnodes
- lists can be used as values of nodes and properties
- comments
- line continuation

### Example
```shell
# Comment line

# Simple values
string_value = "Hello"
number_value = 42
floating_number_value = 3.14

# subnodes example
# Use indent to define child nodes
# Width of indent determined by first indent met (4 spaces in this example)
node1
    subnode1
        moresubnode1 = "somevalue"
        moresubnode2
    subnode2 color = "Green"       # color is a property of subnode2
        anothersubnode width = 10, # more properties example
                       height = 5  # continue declare props after comma


# Other features
value = $string_value                       # reference example
                                            # value now equals "Hello"

other_value = $node1.subnode1.moresubnode1  # copy value from nested nodes

node_copy $node1.subnode2                   # copy whole hierarchy from subnode2
                                            # node_copy color = "Green"
                                            #     anothersubnode width = "10",
                                            #                    height = "5"

# Arrays and dict can be used as values of nodes and properties.
list_ex = [3.14, 42, "foo"]
dict_ex = {a: 3, b: 42, c: "hello world", d: $list_ex}

```

## Library API description

### Usage example

File 'myconfigsample.cfg' contains config from previous Example.

```c++
#include <fstream>
#include <iostream>

#include <vvvcfg/vvvcfg.hpp>

int main(int argc, char** argv) try
{
    std::ifstream f("myconfigsample.cfg");
    vvv::CfgNode root = vvv::make_cfg(f);

    // Print full hierarchy
    std::cout << root;

    // Get subnodes
    std::cout << root.getChild(
            std::vector<std::string>{"node", "subnode", "moresubnode"});
    std::cout << root.getChild({"node", "subnode", "moresubnode"});
    std::cout << root.getChild("node.subnode.moresubnode");

    // Get node value
    std::cout << root.getChild("value").getValue().asString() << "\n";

    // Get property
    std::cout << root.getChild("node_copy").getProperty("color").asString() << "\n";

    // List example
    auto list = root.getChild("list_ex").getValue().asList();
    for (const auto& s: list)
        std::cout << s << "\n";
    auto list2 = root.getChild("list_ex").getList();
    for (const auto& s: list2)
        std::cout << s << "\n";
}
catch (std::exception& e) {
    std::cerr << e.what() << "\n";
} catch (...) {
    std::cerr << "Unexpected exception\n";
}
```
