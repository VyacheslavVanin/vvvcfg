#pragma once
#include <boost/any.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace vvv {

struct Value {
    enum class DATA_TYPE { STRING, LIST };

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
        data = std::vector<Value>();
        auto& list = asList();
        const auto s = value.size();
        list.resize(s);
        for (size_t i = 0; i < s; ++i)
            list[i] = value[i];
        return *this;
    }

    Value& append(const Value& value);

    size_t size() const;
    bool empty() const;

    Value& operator[](size_t i);
    const Value& operator[](size_t i) const;

    bool isString() const {return type == DATA_TYPE::STRING;}
    bool isList() const {return type == DATA_TYPE::LIST;}
    const std::string& asString() const;
    std::string& asString();
    const std::vector<Value>& asList() const;
    std::vector<Value>& asList();
    const std::vector<std::string> asStringList() const;

    bool operator==(const Value& other) const;
private:
    void assert_list() const;
    void assert_string() const;

    using any_list = std::vector<Value>;
    boost::any data;
    DATA_TYPE type;
};

} // namespace vvv

std::ostream& operator<<(std::ostream& str, const vvv::Value& n);
std::ostream& operator<<(std::ostream& str, const std::vector<vvv::Value>& n);

