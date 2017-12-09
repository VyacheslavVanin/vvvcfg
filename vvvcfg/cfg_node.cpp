#include "cfg_node.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <numeric>

namespace vvv {

CfgNode::CfgNode(const std::string& name) : name(name), properties(), children()
{
}

CfgNode::CfgNode(const CfgNode& node) { copyFull(node); }

CfgNode& CfgNode::operator=(const CfgNode& node)
{
    copyFull(node);
    return *this;
}

CfgNode::CfgNode(CfgNode&&) = default;
CfgNode& CfgNode::operator=(CfgNode&&) = default;
CfgNode::~CfgNode() = default;

const std::string& CfgNode::getName() const { return name; }

bool CfgNode::hasChild(const std::string& name) const
{
    return find_child(name) != children.end();
}

CfgNode& CfgNode::addChild(const std::string& name)
{
    auto child_it = find_child(name);
    if (child_it != children.end())
        return *child_it;
    children.emplace_back(name);
    return children.back();
}

void CfgNode::copyFull(const CfgNode& node)
{
    name = node.getName();
    value = node.value;
    copyProperties(node);
    copyChildren(node);
}

void CfgNode::copyValue(const CfgNode& node) { value = node.value; }

void CfgNode::copyProperties(const CfgNode& node)
{
    properties.insert(node.getProperties().begin(), node.getProperties().end());
}

void CfgNode::copyChildren(const CfgNode& node)
{
    for (const auto& child : node.children)
        children.push_back(child);
}

size_t CfgNode::getNumChildren() const { return children.size(); }

const CfgNode& CfgNode::getChild(size_t n) const { return children[n]; }

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
    return std::accumulate(begin + 1, end, std::string(*begin),
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
            throw std::out_of_range(join(names.begin(), names.begin() + i));
        node = &*it;
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
            throw std::out_of_range(join(list.begin(), list.begin() + i));
        node = &*it;
    }
    return *node;
}

CfgNode& CfgNode::getChild(const std::string& name)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode&>(c->getChild(name));
}

void CfgNode::setProperty(const std::string& name, const value_type& value)
{
    properties[name] = value;
}

void CfgNode::setProperty(const std::string& name, const std::string& value)
{
    properties[name] = value;
}

void CfgNode::appendToStringProperty(const std::string& name,
                                     const std::string& value)
{
    properties[name].asString() += value;
}


void CfgNode::setProperty(const std::string& name, const value_list_type& value)
{
    properties[name] = value;
}

void CfgNode::appendToListProperty(const std::string& name, const std::string& value)
{
    properties[name].asList().push_back(value);
}

void CfgNode::appendToLastListProperty(const std::string& name, const std::string& value)
{
    auto& list = properties[name].asList();
    if (list.empty()) {
        list.push_back(value);
        return;
    }
    list.back() += value;
}

bool CfgNode::hasProperty(const std::string& name) const
{
    return properties.find(name) != properties.end();
}

const CfgNode::value_type& CfgNode::getProperty(const std::string& name) const
{
    return properties.at(name);
}

const std::string& CfgNode::getPropertyAsString(const std::string& name) const
{
    return getProperty(name).asString();
}

double CfgNode::getPropertyAsDouble(const std::string& name) const
{
    return std::stod(getPropertyAsString(name));
}

long long CfgNode::getPropertyAsLong(const std::string& name) const
{
    return std::stoll(getPropertyAsString(name));
}

const CfgNode::children_container_type& CfgNode::getChildren() const
{
    return children;
}

const CfgNode::properties_type& CfgNode::getProperties() const
{
    return properties;
}

CfgNode::children_cit CfgNode::find_child(const std::string& name) const
{
    return std::find_if(
        children.begin(), children.end(),
        [&name](const auto& child) { return name == child.getName(); });
}

CfgNode::children_it CfgNode::find_child(const std::string& name)
{
    const auto cit = static_cast<const CfgNode*>(this)->find_child(name);
    return children.erase(cit, cit);
}

const CfgNode::value_type& CfgNode::getValue() const
{
    return value;
}

void CfgNode::setValue(const CfgNode::value_type& value)
{
    this->value = value;
}

bool CfgNode::hasValue() const { return !value.empty(); }

bool CfgNode::isValueString() const { return value.which() == 0; }

bool CfgNode::isValueList() const { return value.which() == 1; }

const std::string& CfgNode::getValueAsString() const
{
    return value.asString();
}

void CfgNode::setValue(const std::string& value) { this->value = value; }

void CfgNode::appendToValue(const std::string& value)
{
    const auto type = this->value.which();
    switch (type) {
    case 0: this->value.asString() += value; break;
    case 1: this->value.asList().push_back(value); break;
    default: throw std::logic_error("Shouldn't be here");
    }
}

const CfgNode::value_list_type& CfgNode::getList() const
{
    return value.asList();
}

void CfgNode::push_back(const std::string& value)
{
    this->value.asList().push_back(value);
}

void CfgNode::appendToLast(const std::string& value)
{
    auto& list = this->value.asList();
    if (list.empty()) {
        list.push_back(value);
        return;
    }
    list.back() += value;
}

void CfgNode::addEmptyList() { this->value = value_list_type{}; }

} // namespace vvv

namespace {
std::ostream& operator<<(std::ostream& str,
                         const vvv::CfgNode::value_list_type& list)
{
    std::copy(list.begin(), list.end(),
              std::ostream_iterator<std::string>(str, ", "));
    return str;
}

void print_node(std::ostream& str, const vvv::CfgNode& node, int tab_width = 4,
                int lvl = 0)
{
    for (int i = 0; i < lvl * tab_width; ++i)
        str << " ";
    str << node.getName();
    if (node.hasValue()) {
        if (node.isValueString() && node.getValueAsString().size())
            str << " = \"" << node.getValueAsString() << "\"";
        else if (node.isValueList()) {
            str << " = [" << node.getList() << "]";
        }
    }
    for (auto p : node.getProperties()) {
        str << " " << p.first;
        if (!p.second.empty()) {
            if (p.second.isString()) {
                const auto& s = p.second.asString();
                if (s.size())
                    str << " = \"" << s << "\"";
            } else if (p.second.isList()) {
                const auto& list = p.second.asList();
                str << " = [" << list << "]";
            }
        }
        str << ",";
    }
    str << "\n";
    for (size_t i = 0; i < node.getNumChildren(); ++i)
        print_node(str, node.getChild(i), tab_width, lvl + 1);
}
}; // namespace

std::ostream& operator<<(std::ostream& str, const vvv::CfgNode& node)
{
    print_node(str, node);
    return str;
}
