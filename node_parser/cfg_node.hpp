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

    CfgNode(const std::string& name);
    CfgNode(const CfgNode& node);
    CfgNode& operator=(const CfgNode& node);
    CfgNode(CfgNode&&);
    CfgNode& operator=(CfgNode&&);
    ~CfgNode();

    const std::string& getName() const;
    bool hasChild(const std::string& name) const;
    CfgNode& addChild(const std::string& name);
    void copyFrom(const CfgNode& node);

    size_t getNumChildren() const;
    const CfgNode& getChild(size_t n) const;
    CfgNode& getChild(size_t n);

    const CfgNode& getChild(const std::string& name) const;
    CfgNode& getChild(const std::string& name);

    void setProperty(const std::string& name, const std::string& value);
    void appendToPropperty(const std::string& name, const std::string& value);

    const std::string& getProperty(const std::string& name) const;
    double getPropertyAsDouble(const std::string& name) const;
    long long getPropertyAsLong(const std::string& name) const;

    property_it begin();
    property_it end();
    property_cit begin() const;
    property_cit end() const;

private:
    std::string name;
    properties_type properties;
    std::vector<std::unique_ptr<CfgNode>> children;

    children_cit find_child(const std::string& name) const;
    children_it find_child(const std::string& name);
};

std::ostream& operator<<(std::ostream& str, const CfgNode& node);
