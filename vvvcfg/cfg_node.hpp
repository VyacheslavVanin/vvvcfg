#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "cfg_node_value.hpp"

namespace vvv {

class CfgNode {
public:
    using value_list_type = std::vector<std::string>;
    using value_type = vvv::Value;
    using properties_type = std::map<std::string, value_type>;
    using property_it = properties_type::iterator;
    using property_cit = properties_type::const_iterator;
    using children_container_type = std::vector<CfgNode>;
    using children_cit = children_container_type::const_iterator;
    using children_it = children_container_type::iterator;

    CfgNode(const std::string& name);
    CfgNode(const CfgNode& node);
    CfgNode& operator=(const CfgNode& node);
    CfgNode(CfgNode&&);
    CfgNode& operator=(CfgNode&&);
    ~CfgNode();

    bool operator==(const CfgNode& other) const;

    const std::string& getName() const;
    bool hasChild(const std::string& name) const;
    CfgNode& addChild(const std::string& name);
    void copyFull(const CfgNode& node);
    void copyValue(const CfgNode& node);
    void copyProperties(const CfgNode& node);
    void copyChildren(const CfgNode& node);

    size_t getNumChildren() const;
    const CfgNode& getChild(size_t n) const;
    CfgNode& getChild(size_t n);

    const CfgNode& getChild(const std::string& name) const;
    CfgNode& getChild(const std::string& name);
    const CfgNode& getChild(const std::vector<std::string>& names) const;
    const CfgNode& getChild(std::initializer_list<std::string> list) const;

    void setProperty(const std::string& name, const value_type& value);
    void setProperty(const std::string& name, const std::string& value);
    void appendToStringProperty(const std::string& name, const std::string& value);
    void setProperty(const std::string& name, const value_list_type& value);
    void appendToListProperty(const std::string& name, const std::string& value);
    void appendToLastListProperty(const std::string& name, const std::string& value);

    bool hasProperty(const std::string& name) const;
    const value_type& getProperty(const std::string& name) const;
    value_type& getProperty(const std::string& name);
    const std::string& getPropertyAsString(const std::string& name) const;
    double getPropertyAsDouble(const std::string& name) const;
    long long getPropertyAsLong(const std::string& name) const;

    bool hasValue() const;
    const value_type& getValue() const;
    value_type& getValue();
    void setValue(const value_type& value);

    bool isValueString() const;
    const std::string& getValueAsString() const;
    void setValue(const std::string& value);
    void appendToValue(const std::string& value);

    bool isValueList() const;
    value_list_type getList() const;
    void push_back(const std::string& value);
    void appendToLast(const std::string& value);
    void addEmptyList();

    const children_container_type& getChildren() const;
    const properties_type& getProperties() const;

private:
    std::string name;
    value_type value;
    properties_type properties;
    children_container_type children;

    children_cit find_child(const std::string& name) const;
    children_it find_child(const std::string& name);
};

}

std::ostream& operator<<(std::ostream& str, const vvv::CfgNode& node);
