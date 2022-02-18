#ifndef PET_DATA_MAYBE_H_
#define PET_DATA_MAYBE_H_

#include "data/Union.h"

namespace pet {

template<class T>
class Maybe
{
    struct Container
    {
        T t;

        template<class... Args>
        constexpr inline Container(Args&&... args): t(pet::forward<Args>(args)...) {}

        constexpr inline void* operator new(size_t _, void* ret, const NewOperatorDisambiguator&) { return ret; }
        constexpr inline void operator delete(void*) {}
    };

    Container* ptr = nullptr;
    char data[(sizeof(T) + sizeof(char) - 1) / sizeof(char)] alignas(alignof(T));

public:
    constexpr inline Maybe() = default;

    constexpr inline Maybe(const T& t): ptr(new(data, NewOperatorDisambiguator{}) Container(t)) {}
    constexpr inline Maybe(T&& t): ptr(new(data, NewOperatorDisambiguator{}) Container(pet::move(t))) {}

    constexpr inline Maybe(const Maybe& v): ptr(v.ptr ? new(data, NewOperatorDisambiguator{}) Container(v.ptr->t) : nullptr) {}

    constexpr inline Maybe(Maybe&& v): ptr(v.ptr ? new(data, NewOperatorDisambiguator{}) Container(pet::move(v.ptr->t)) : nullptr) {}

    template<class... Args>
    constexpr inline Maybe(nullptr_t, Args&&... args): ptr(new(data, NewOperatorDisambiguator{}) Container(pet::forward<Args>(args)...)) {}

    constexpr inline Maybe& operator=(const Maybe& o)
    {
        delete ptr;

        if(!o.ptr)
        {
            ptr = nullptr;
        }
        else
        {
            ptr = new(data, NewOperatorDisambiguator{}) Container(o.ptr->t);
        }

        return *this;
    }

    constexpr inline Maybe& operator=(Maybe&& o)
    {
        delete ptr;

        if(!o.ptr)
        {
            ptr = nullptr;
        }
        else
        {
            ptr = new(data, NewOperatorDisambiguator{}) Container(pet::move(o.ptr->t));
        }

        return *this;
    }

    inline ~Maybe() {
        delete ptr;
    }

    constexpr const T* operator->() const { return &ptr->t; }
    constexpr T* operator->() { return &ptr->t; }
    constexpr const T& operator*() const &  { return ptr->t; }
    constexpr T& operator*() &  { return ptr->t; }
    constexpr const T&& operator*() const &&  { return ptr->t; }
    constexpr T&& operator*() &&  { return ptr->t; }

    explicit constexpr operator bool() const { return ptr; }
};

}


#endif /* PET_DATA_MAYBE_H_ */
