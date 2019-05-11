#pragma once
#include <memory>

namespace vvv {
class Any {
public:
    Any() = default;

    template <typename T>
    Any(T const& v) : data(new storage<T>(v))
    {
    }

    Any(Any const& other) : data(other.data ? other.data->clone() : nullptr) {}

    void swap(Any& other) { data.swap(other.data); }
    friend void swap(Any& a, Any& b) { a.swap(b); };
    Any& operator=(Any other)
    {
        swap(other);
        return *this;
    }

    bool empty() const { return data.get() == nullptr; }

private:
    struct storage_base {
        virtual std::unique_ptr<storage_base> clone() const = 0;
        virtual ~storage_base() = default;
    };

    template <typename T>
    struct storage : public storage_base {
        explicit storage(const T& v) : value(v) {}

        std::unique_ptr<storage_base> clone() const override
        {
            return std::unique_ptr<storage_base>(new storage<T>(value));
        }
        T value;
    };
    std::unique_ptr<storage_base> data;

    template <typename T>
    friend T& any_cast(Any&);
    template <typename T>
    friend T const& any_cast(const Any&);
};

template <typename T>
inline T& any_cast(Any& a)
{
    if (auto p = dynamic_cast<Any::storage<T>*>(a.data.get()))
        return p->value;
    else
        throw std::bad_cast();
}

template <typename T>
inline const T& any_cast(const Any& a)
{
    if (auto p = dynamic_cast<Any::storage<T> const*>(a.data.get()))
        return p->value;
    else
        throw std::bad_cast();
}
} // namespace vvv
