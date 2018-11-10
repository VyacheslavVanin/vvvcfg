#include "cfg_node_value.hpp"

namespace vvv {
Value::Value() : data(std::string()), type(DATA_TYPE::STRING) {}
Value::Value(const std::string& data) : data(data), type(DATA_TYPE::STRING) {}
Value::Value(const char* data)
    : data(std::string(data)), type(DATA_TYPE::STRING)
{
}

Value::Value(DATA_TYPE type) : data(), type(type)
{
    switch (type) {
    case DATA_TYPE::STRING: data = std::string(); break;
    case DATA_TYPE::LIST: data = list_type(); break;
    case DATA_TYPE::DICT: data = dict_type(); break;
    default: throw std::logic_error("Invalud DATA_TYPE");
    }
}

void Value::assert_list() const
{
    if (type != DATA_TYPE::LIST)
        throw std::logic_error("Non-list object");
}

void Value::assert_dict() const
{
    if (type != DATA_TYPE::DICT)
        throw std::logic_error("Non-dict object");
}

void Value::assert_string() const
{
    if (type != DATA_TYPE::STRING)
        throw std::logic_error("Non-string object");
}

Value& Value::operator=(const std::string& value)
{
    type = DATA_TYPE::STRING;
    data = value;
    return *this;
}

Value& Value::append(const Value& value)
{
    assert_list();
    auto* list = boost::any_cast<list_type>(&data);
    list->push_back(value);
    return *this;
}

size_t Value::size() const
{
    assert_list();
    const auto* list = boost::any_cast<list_type>(&data);
    return list->size();
}

bool Value::empty() const { return data.empty(); }

Value& Value::operator[](size_t i)
{
    return const_cast<Value&>(static_cast<const Value&>(*this)[i]);
}

const Value& Value::operator[](size_t i) const
{
    auto& list = *boost::any_cast<list_type>(&data);
    return list[i];
}

Value& Value::operator[](const std::string& key)
{
    auto& list = *boost::any_cast<dict_type>(&data);
    return list.at(key);
}

const Value& Value::operator[](const std::string& key) const
{
    auto& list = *boost::any_cast<dict_type>(&data);
    return list.at(key);
}

std::string& Value::asString()
{
    return const_cast<std::string&>(
        static_cast<const Value&>(*this).asString());
}

const std::string& Value::asString() const
{
    assert_string();
    return *boost::any_cast<std::string>(&data);
}

Value::list_type& Value::asList()
{
    return const_cast<list_type&>(static_cast<const Value&>(*this).asList());
}

const Value::list_type& Value::asList() const
{
    assert_list();
    return *boost::any_cast<list_type>(&data);
}

Value::dict_type& Value::asDict()
{
    return const_cast<dict_type&>(static_cast<const Value&>(*this).asDict());
}

const Value::dict_type& Value::asDict() const
{
    assert_dict();
    return *boost::any_cast<dict_type>(&data);
}

std::vector<std::string> Value::asStringList() const
{
    const auto s = size();
    std::vector<std::string> ret;
    ret.reserve(s);
    for (size_t i = 0; i < s; ++i)
        ret.push_back((*this)[i].asString());
    return ret;
}

bool Value::operator==(const Value& other) const
{
    if (type != other.getType())
        return false;
    if (type == DATA_TYPE::STRING)
        return asString() == other.asString();
    if (type == DATA_TYPE::LIST)
        return asList() == other.asList();
    return true;
}

} // namespace vvv

std::ostream& operator<<(std::ostream& str, const vvv::Value::list_type& n)
{
    const auto s = n.size();
    str << "[";
    if (s > 0) {
        for (size_t i = 0; i < s - 1; ++i)
            str << n[i] << ", ";
        for (size_t i = s - 1; i < s; ++i)
            str << n[i];
    }
    str << "]";
    return str;
}

std::ostream& operator<<(std::ostream& str, const vvv::Value::dict_type& n)
{
    const auto s = n.size();
    str << "{";
    if (s == 1) {
        const auto& i = *n.begin();
        str << "{" << i.first << ":" << i.second << "}";
    }
    else if (s > 1) {
        const auto& front = *n.begin();
        str << "{" << front.first << ":" << front.second << "}";
        auto first = n.begin();
        auto second = first;
        second++;
        for (auto i = second; i != n.end(); ++i)
            str << ", {" << i->first << ":" << i->second << "}";
    }
    str << "}";
    return str;
}

std::ostream& operator<<(std::ostream& str, const vvv::Value& n)
{
    switch (n.getType()) {
    case vvv::Value::DATA_TYPE::STRING:
        str << "\"" << n.asString() << "\"";
        break;
    case vvv::Value::DATA_TYPE::LIST: str << n.asList(); break;
    case vvv::Value::DATA_TYPE::DICT: str << n.asDict(); break;
    }
    return str;
}
