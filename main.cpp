#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "token_parser.h"
#include "token_parser/dncfg_token_fsm.h"

class CfgNode {
public:
    using properties_type = std::map<std::string, std::string>;
    using property_it = properties_type::iterator;
    using property_cit = properties_type::const_iterator;

    CfgNode(const std::string& name)
        : name(name), properties(), children()
    {
    }
    CfgNode(const CfgNode& node) { copyFrom(node); }
    CfgNode& operator=(const CfgNode& node)
    {
        copyFrom(node);
        return *this;
    }
    CfgNode(CfgNode&&) = default;
    CfgNode& operator=(CfgNode&&) = default;
    ~CfgNode()                    = default;

    const std::string& getName() const { return name; }

    bool hasChild(const std::string& name) const
    {
        return std::find_if(children.begin(), children.end(),
                            [&name](const auto& child) {
                                return name == child->getName();
                            }) != children.end();
    }

    CfgNode& addChild(const std::string& name)
    {
        if (hasChild(name))
            throw std::logic_error("Dupplicate key");
        children.push_back(std::make_unique<CfgNode>(name));
        return *children.back();
    }

    void copyFrom(const CfgNode& node) {
        properties.insert(node.begin(), node.end());
        for (const auto& child : node.children)
            children.push_back(std::make_unique<CfgNode>(*child.get()));
    }

    size_t getNumChildren() const {return children.size();}

    CfgNode& getChild(size_t n) {
        return *children[n];
    }

    void setProperty(const std::string& name, const std::string& value)
    {
        properties[name] = value;
    }

    const std::string& getProperty(const std::string& name) const
    {
        return properties.at(name);
    }

    double getPropertyAsDouble(const std::string& name) const
    {
        return std::stod(getProperty(name));
    }

    long long getPropertyAsLong(const std::string& name) const
    {
        return std::stoll(getProperty(name));
    }

    property_it begin() {return properties.begin();}
    property_it end() {return properties.end();}
    property_cit begin() const {return properties.cbegin();}
    property_cit end() const {return properties.cend();}

private:
    std::string name;
    properties_type properties;
    std::vector<std::unique_ptr<CfgNode>> children;
};

struct name_properties
{
    std::string name;
    CfgNode::properties_type properties;
};

name_properties name_properties_from_tokens(const std::vector<token_t>& tokens)
{
    name_properties ret;
    if (tokens.empty())
        throw std::logic_error("No tokens");
    const auto& nameToken = tokens.front();
    if (nameToken.type != TOKEN_TYPE_NAME)
        throw std::logic_error("First token not name");
    if (nameToken.value.empty())
        throw std::logic_error("Name is empty");
    
    ret.name = nameToken.value;

    return ret;
}

void addChild(CfgNode& parent, const std::vector<token_t>& tokens)
{
}


size_t count_spaces(const std::string& str)
{
    return std::find_if(str.begin(), str.end(),
                        [](const auto& ch) { return ch != ' '; }) -
           str.begin();
}

size_t indent_lvl(size_t spaces, size_t indent_width=4)
{
    if (spaces == 0)
        return 0;
    if (spaces % indent_width)
        throw std::logic_error("Invalid indent");
    return spaces / indent_width;
}

int main(int argc, char** argv)
{
    std::ifstream f("./myconfigsample.cfg");
    std::string line;

    TokenStream ts(f);
    token_t tok;
    while (ts >> tok) {
        std::cout << tok << "\n";
    }

    //CfgNode root("root");
    //std::vector<CfgNode*> stack {&root};
    //size_t last_lvl = 0;

    //size_t tab_width = 0;
    //while (std::getline(f, line)) {
    //    const auto spaces = count_spaces(line);
    //    tab_width = tab_width ? tab_width : spaces;
    //    const auto lvl = indent_lvl(spaces, tab_width);
    //    if (lvl > last_lvl + 1)
    //        throw std::logic_error("Overindent");

    //    last_lvl = lvl;
    //    stack.resize(lvl + 1);

    //    const auto tokens = line_to_tokens(line);
    //    if (tokens.status == parse_token_result::STATUS::ERROR) {
    //        std::cout << tokens.error_message << "\n";
    //        break;
    //    }

    //    auto& parrent = stack.back();
    //    const auto& name = tokens.tokens.front().value;
    //    //new_node = Node()
    //    //new_node.properties['value'] = value
    //    //stack.append(new_node)
    //    //stack[lvl].children[key] = new_node

    //    std::cout << "-------------------- " << count_spaces(line) << " " << lvl << "\n";
    //    for (const auto& token : tokens.tokens)
    //        std::cout << token << "\n";
    //}
}
