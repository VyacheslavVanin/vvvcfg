#include "cfg_node.hpp"
#include <iostream>
#include <iterator>
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

bool CfgNode::operator==(const CfgNode& other) const
{
    return name == other.name && value == other.value &&
           properties == other.properties && children == other.children;
}

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

namespace {
inline void split(const std::string& input, char delimiter,
                  std::vector<std::string>& ret)
{
    using namespace std;
    size_t last_pos = 0;
    while (true) {
        const size_t current_pos = input.find(delimiter, last_pos);
        if (current_pos == string::npos) {
            const string substr =
                input.substr(last_pos, input.size() - last_pos);
            ret.push_back(substr);
            break;
        }
        else {
            const string substr =
                input.substr(last_pos, current_pos - last_pos);
            ret.push_back(substr);
            last_pos = current_pos + 1;
        }
    }
}
} // namespace

const CfgNode& CfgNode::getChild(const std::string& name) const
{
    std::vector<std::string> names;
    split(name, '.', names);
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

CfgNode& CfgNode::getChild(const std::string& name)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode&>(c->getChild(name));
}

const CfgNode* CfgNode::getChildPtr(const std::string& name) const
try {
    return &getChild(name);
}
catch (...) {
    return nullptr;
}

const CfgNode* CfgNode::getChildPtr(const std::vector<std::string>& names) const
try {
    return &getChild(names);
}
catch (...) {
    return nullptr;
}

CfgNode* CfgNode::getChildPtr(const std::string& name)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode*>(c->getChildPtr(name));
}

CfgNode* CfgNode::getChildPtr(const std::vector<std::string>& names)
{
    const auto* c = static_cast<const CfgNode*>(this);
    return const_cast<CfgNode*>(c->getChildPtr(names));
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

void CfgNode::appendToListProperty(const std::string& name,
                                   const std::string& value)
{
    properties[name].asList().push_back(Value(value));
}

void CfgNode::appendToLastListProperty(const std::string& name,
                                       const std::string& value)
{
    auto& list = properties[name].asList();
    if (list.empty()) {
        list.push_back(Value(value));
        return;
    }
    list.back().asString() += value;
}

bool CfgNode::hasProperty(const std::string& name) const
{
    return properties.find(name) != properties.end();
}

const CfgNode::value_type& CfgNode::getProperty(const std::string& name) const
{
    return properties.at(name);
}

const CfgNode::value_type&
CfgNode::getProperty(const std::string& name,
                     const CfgNode::value_type& default_value) const
{
    const auto& it = properties.find(name);
    if (it == properties.end())
        return default_value;

    return it->second;
}

CfgNode::value_type& CfgNode::getProperty(const std::string& name)
{
    return properties.at(name);
}

const std::string& CfgNode::getPropertyAsString(const std::string& name) const
{
    return getProperty(name).asString();
}

const std::string&
CfgNode::getPropertyAsString(const std::string& name,
                             const std::string& default_value) const
{
    const auto& it = properties.find(name);
    if (it == properties.end())
        return default_value;

    return it->second.asString();
}

double CfgNode::getPropertyAsDouble(const std::string& name) const
{
    return std::stod(getPropertyAsString(name));
}

double CfgNode::getPropertyAsDouble(const std::string& name,
                                    double default_value) const
{
    const auto& it = properties.find(name);
    if (it == properties.end())
        return default_value;

    return std::stod(it->second.asString());
}

long long CfgNode::getPropertyAsLong(const std::string& name) const
{
    return std::stoll(getPropertyAsString(name));
}

long long CfgNode::getPropertyAsLong(const std::string& name,
                                     long long default_value) const
{
    const auto& it = properties.find(name);
    if (it == properties.end())
        return default_value;

    return std::stoll(it->second.asString());
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

const CfgNode::value_type& CfgNode::getValue() const { return value; }
CfgNode::value_type& CfgNode::getValue() { return value; }

void CfgNode::setValue(const CfgNode::value_type& value)
{
    this->value = value;
}

bool CfgNode::hasValue() const { return !value.empty(); }

bool CfgNode::isValueString() const { return value.isString(); }

bool CfgNode::isValueList() const { return value.isList(); }

const std::string& CfgNode::getValueAsString() const
{
    return value.asString();
}

void CfgNode::setValue(const std::string& value) { this->value = value; }

void CfgNode::appendToValue(const std::string& value)
{
    if (this->value.isString())
        this->value.asString() += value;
    else if (this->value.isList())
        this->value.asList().push_back(Value(value));
    else
        throw std::logic_error("Shouldn't be here");
}

CfgNode::value_list_type CfgNode::getList() const
{
    return value.asStringList();
}

void CfgNode::push_back(const std::string& value)
{
    this->value.asList().push_back(Value(value));
}

void CfgNode::appendToLast(const std::string& value)
{
    auto& list = this->value.asList();
    if (list.empty()) {
        list.push_back(Value(value));
        return;
    }
    list.back().asString() += value;
}

void CfgNode::addEmptyList() { this->value = Value(Value::DATA_TYPE::LIST); }

} // namespace vvv

namespace {
std::ostream& operator<<(std::ostream& str,
                         const vvv::CfgNode::value_list_type& list)
{
    std::copy(list.begin(), list.end(),
              std::ostream_iterator<std::string>(str, ", "));
    return str;
}

using ::operator<<;
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
            }
            else if (p.second.isList()) {
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
} // namespace

std::ostream& operator<<(std::ostream& str, const vvv::CfgNode& node)
{
    print_node(str, node);
    return str;
}
