#include "cfg_node.hpp"
#include <iostream>
#include <numeric>
#include <boost/algorithm/string.hpp>

CfgNode::CfgNode(const std::string& name) : name(name), properties(), children()
{
}

CfgNode::CfgNode(const CfgNode& node)
{
    name = node.getName();
    copyFrom(node);
}

CfgNode& CfgNode::operator=(const CfgNode& node)
{
    name = node.getName();
    copyFrom(node);
    return *this;
}

CfgNode::CfgNode(CfgNode&&) = default;
CfgNode& CfgNode::operator=(CfgNode&&) = default;
CfgNode::~CfgNode() = default;

const std::string& CfgNode::getName() const { return name; }

bool CfgNode::hasChild(const std::string& name) const
{
    return std::find_if(children.begin(), children.end(),
                        [&name](const auto& child) {
                            return name == child->getName();
                        }) != children.end();
}

CfgNode& CfgNode::addChild(const std::string& name)
{
    if (hasChild(name))
        throw std::logic_error("Dupplicate key");
    children.push_back(std::make_unique<CfgNode>(name));
    return *children.back();
}

void CfgNode::copyFrom(const CfgNode& node)
{
    properties.insert(node.begin(), node.end());
    for (const auto& child : node.children)
        children.push_back(std::make_unique<CfgNode>(*child.get()));
}

size_t CfgNode::getNumChildren() const { return children.size(); }

const CfgNode& CfgNode::getChild(size_t n) const { return *children[n]; }

CfgNode& CfgNode::getChild(size_t n)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode&>(c->getChild(n));
}

const CfgNode& CfgNode::getChild(const std::string& name) const
{
    std::vector<std::string> names;
    boost::split(names, name, boost::is_any_of("."));
    return getChild(names);
}

template <typename I>
static std::string join(I begin, I end, const std::string& sep = ", ")
{
    return std::accumulate(
        begin+1, end, std::string(*begin),
        [&sep](std::string& res, const std::string& e) {
            return res += sep + e; 
        });
}

const CfgNode& CfgNode::getChild(const std::vector<std::string>& names) const
{
    const CfgNode* node = this;
    size_t i = 0;
    for (const auto& name : names) {
        ++i;
        auto it = node->find_child(name);
        if (it == node->children.end())
            throw std::out_of_range(join(names.begin(), names.begin()+i));
        node = it->get();
    }
    return *node;
}

const CfgNode& CfgNode::getChild(std::initializer_list<std::string> list) const
{
    const CfgNode* node = this;
    size_t i = 0;
    for (const auto& name : list) {
        ++i;
        auto it = node->find_child(name);
        if (it == node->children.end())
            throw std::out_of_range(join(list.begin(), list.begin()+i));
        node = it->get();
    }
    return *node;
}

CfgNode& CfgNode::getChild(const std::string& name)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode&>(c->getChild(name));
}

void CfgNode::setProperty(const std::string& name, const std::string& value)
{
    properties[name] = value;
}

void CfgNode::appendToPropperty(const std::string& name,
                                const std::string& value)
{
    properties[name] += value;
}

const std::string& CfgNode::getProperty(const std::string& name) const
{
    return properties.at(name);
}

double CfgNode::getPropertyAsDouble(const std::string& name) const
{
    return std::stod(getProperty(name));
}

long long CfgNode::getPropertyAsLong(const std::string& name) const
{
    return std::stoll(getProperty(name));
}

CfgNode::property_it CfgNode::begin() { return properties.begin(); }
CfgNode::property_it CfgNode::end() { return properties.end(); }
CfgNode::property_cit CfgNode::begin() const { return properties.cbegin(); }
CfgNode::property_cit CfgNode::end() const { return properties.cend(); }

CfgNode::children_cit CfgNode::find_child(const std::string& name) const
{
    return std::find_if(
        children.begin(), children.end(),
        [&name](const auto& child) { return name == child->getName(); });
}

CfgNode::children_it CfgNode::find_child(const std::string& name)
{
    const auto cit = static_cast<const CfgNode*>(this)->find_child(name);
    return children.erase(cit, cit);
}


const std::string& CfgNode::getValue() const
{
    return value;
}

void CfgNode::setValue(const std::string& value)
{
    this->value = value;
}

void CfgNode::appendToValue(const std::string& value)
{
    this->value += value;
}

namespace {
void print_node(std::ostream& str, const CfgNode& node, int tab_width = 4,
                int lvl = 0)
{
    for (int i = 0; i < lvl * tab_width; ++i)
        str << " ";
    str << node.getName();
    if (!node.getValue().empty())
        str << " = \"" << node.getValue() << "\"";
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
