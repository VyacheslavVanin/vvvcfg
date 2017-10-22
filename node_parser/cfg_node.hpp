#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

class CfgNode {
public:
    using properties_type = std::map<std::string, std::string>;
    using property_it = properties_type::iterator;
    using property_cit = properties_type::const_iterator;
    using children_container = std::vector<std::unique_ptr<CfgNode>>;
    using children_cit = children_container::const_iterator;
    using children_it = children_container::iterator;

    CfgNode(const std::string& name) : name(name), properties(), children() {}
    CfgNode(const CfgNode& node)
    {
        name = node.getName();
        copyFrom(node);
    }
    CfgNode& operator=(const CfgNode& node)
    {
        name = node.getName();
        copyFrom(node);
        return *this;
    }
    CfgNode(CfgNode&&) = default;
    CfgNode& operator=(CfgNode&&) = default;
    ~CfgNode() = default;

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

    void copyFrom(const CfgNode& node)
    {
        properties.insert(node.begin(), node.end());
        for (const auto& child : node.children)
            children.push_back(std::make_unique<CfgNode>(*child.get()));
    }

    size_t getNumChildren() const { return children.size(); }

    const CfgNode& getChild(size_t n) const { return *children[n]; }

    CfgNode& getChild(size_t n)
    {
        const auto* c = static_cast<const CfgNode*>(this);
        return const_cast<CfgNode&>(c->getChild(n));
    }

    const CfgNode& getChild(const std::string& name) const
    {
        auto it = find_child(name);
        return *it->get();
    }

    CfgNode& getChild(const std::string& name)
    {
        const auto* c = static_cast<const CfgNode*>(this);
        return const_cast<CfgNode&>(c->getChild(name));
    }

    void setProperty(const std::string& name, const std::string& value)
    {
        properties[name] = value;
    }

    void appendToPropperty(const std::string& name, const std::string& value)
    {
        properties[name] += value;
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

    property_it begin() { return properties.begin(); }
    property_it end() { return properties.end(); }
    property_cit begin() const { return properties.cbegin(); }
    property_cit end() const { return properties.cend(); }

private:
    std::string name;
    properties_type properties;
    std::vector<std::unique_ptr<CfgNode>> children;

    children_cit find_child(const std::string& name) const
    {
        return std::find_if(
            children.begin(), children.end(),
            [&name](const auto& child) { return name == child->getName(); });
    }

    children_it find_child(const std::string& name)
    {
        const auto cit = static_cast<const CfgNode*>(this)->find_child(name);
        return children.erase(cit, cit);
    }
};

std::ostream& operator<<(std::ostream& str, const CfgNode& node);
