#pragma once
#include <boost/any.hpp>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace vvv {

struct Value {
    using list_type = std::vector<Value>;
    using dict_type = std::unordered_map<std::string, Value>;
    enum class DATA_TYPE { STRING, LIST, DICT };

    Value();
    explicit Value(DATA_TYPE type);
    explicit Value(const char* data);
    explicit Value(const std::string& data);

    DATA_TYPE getType() const { return type; }

    Value& operator=(const std::string& value);
    template <typename T>
    Value& operator=(const std::vector<T>& value)
    {
        type = DATA_TYPE::LIST;
        data = list_type();
        auto& list = asList();
        const auto s = value.size();
        list.resize(s);
        for (size_t i = 0; i < s; ++i)
            list[i] = value[i];
        return *this;
    }
    template <typename T>
    Value& operator=(const std::unordered_map<std::string, T>& value)
    {
        type = DATA_TYPE::DICT;
        data = dict_type();
        auto& dict = asDict();
        for (const auto& i : value)
            dict[i.first] = i.second;
        return *this;
    }

    Value& append(const Value& value);

    size_t size() const;
    bool empty() const;

    Value& operator[](size_t i);
    const Value& operator[](size_t i) const;
    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;

    bool isString() const { return type == DATA_TYPE::STRING; }
    bool isList() const { return type == DATA_TYPE::LIST; }
    bool isDict() const { return type == DATA_TYPE::DICT; }
    const std::string& asString() const;
    std::string& asString();
    const list_type& asList() const;
    list_type& asList();
    const dict_type& asDict() const;
    dict_type& asDict();
    std::vector<std::string> asStringList() const;
    double asDouble() const;
    int asInt() const;
    long long asLong() const;

    bool operator==(const Value& other) const;

private:
    void assert_list() const;
    void assert_dict() const;
    void assert_string() const;

    boost::any data;
    DATA_TYPE type;
};

} // namespace vvv

std::ostream& operator<<(std::ostream& str, const vvv::Value& n);
std::ostream& operator<<(std::ostream& str, const vvv::Value::list_type& n);
std::ostream& operator<<(std::ostream& str, const vvv::Value::dict_type& n);
