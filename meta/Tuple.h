/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef TUPLE_H_
#define TUPLE_H_

#include "meta/Sequence.h"
#include "meta/TypeManipulation.h"
#include "meta/Utility.h"

namespace pet {

namespace detail {

template<int x, class T>
struct TupleWrapper
{
    T value;

    inline TupleWrapper() = default;

    template<class A>
    inline TupleWrapper(A&& value): value(pet::forward<A>(value)) {}
};

template<int, class...> struct TypeAt;

template<int n, class T, class... Ts> struct TypeAt<n, T, Ts...> {
    typedef typename TypeAt<n-1, Ts...>::type type;
};

template<class T, class... Ts> struct TypeAt<0, T, Ts...> {
    typedef T type;
};

template<class idx, class... T> class TupleImpl;

template<int... idx, class... T>
class TupleImpl<Sequence<idx...>, T...>: TupleWrapper<idx, T>...
{
    template<int n>
    using Member = typename TypeAt<n, TupleWrapper<idx, T>...>::type;

    template<class> friend struct TupleExtractor;

    typedef TupleImpl<Sequence<idx...>, removeReference<T>...> NoRefPair;
public:
    template<class... Args>
    inline TupleImpl(nullptr_t, Args&&... args): TupleWrapper<idx, T>(pet::forward<Args>(args))... {}

    inline TupleImpl() = default;
    inline TupleImpl(TupleImpl&&) = default;
    inline TupleImpl(const TupleImpl&) = default;

    template<int n>
    inline decltype(Member<n>::value) &get() {
        return static_cast<Member<n>*>(this)->value;
    }

    template<int n>
    inline const decltype(Member<n>::value) &get() const {
        return static_cast<const Member<n>*>(this)->value;
    }

    template<class Element>
    inline void extract(Element* elements) {
        int unused[] = {0, (elements[idx] = get<idx>(), 0)...};
        (void) unused;
    }

    inline TupleImpl &operator =(const TupleImpl& other) {
        int unused[] = {0, (get<idx>() = other.get<idx>(), 0)...};
        (void) unused;
        return *this;
    }

    inline TupleImpl &operator =(TupleImpl&& other) {
        int unused[] = {0, (get<idx>() = pet::move(other.get<idx>()), 0)...};
        (void) unused;
        return *this;
    }

    template<class = enableIf<!sameTypes<TupleImpl, NoRefPair>::value>>
    inline TupleImpl &operator =(const NoRefPair& other) {
        int unused[] = {0, (get<idx>() = other.template get<idx>(), 0)...};
        (void) unused;
        return *this;
    }

    template<class C>
    inline decltype(auto) moveApply(C&& c) &&
    {
        return c(pet::move(get<idx>())...);
    }

    template<class C>
    inline decltype(auto) copyApply(C&& c)
    {
        return c(pet::forward<T>(get<idx>())...);
    }

    template<class... Args>
    static inline auto create(Args&&... args)
    {
        return TupleImpl(nullptr, pet::forward<Args>(args)...);
    }
};

}

template<class... T>
using Tuple = detail::TupleImpl<sequence<0, sizeof...(T)>, T...>;

template<class... T>
auto makeTuple(T&&... args) {
    return Tuple<T...>(nullptr, pet::forward<T>(args)...);
}

#if 0
// GCC 8 says: "sorry, unimplemented: mangling reference_type"
template<class... T>
Tuple<T&...> tie(T&... args ) {
    return Tuple<T&...>(args...);
}
#endif

}

#endif /* TUPLE_H_ */
