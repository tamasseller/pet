/*
 * Tuple.h
 *
 *      Author: tamas.seller
 */

#ifndef TUPLE_H_
#define TUPLE_H_

#include "meta/Sequence.h"
#include "meta/TypeManipulation.h"

namespace pet {

namespace detail {

template<int x, class T>
struct TupleWrapper {
    T value;
    inline TupleWrapper(const T& value): value(value) {}
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
class TupleImpl<Sequence<idx...>, T...>: TupleWrapper<idx, T>... {
        template<int n>
        using Member = typename TypeAt<n, TupleWrapper<idx, T>...>::type;

        template<class> friend struct TupleExtractor;

        typedef TupleImpl<Sequence<idx...>, removeReference<T>...> NoRefPair;
    public:
        inline TupleImpl(T... args): TupleWrapper<idx, T>(args)... {}

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
                int unused[] = {(elements[idx] = get<idx>(), 0)...};
                (void) unused;
        }

        TupleImpl operator =(const TupleImpl& other) {
            int unused[] = {(get<idx>() =   other.get<idx>(), 0)...};
            (void) unused;
            return *this;
        }

        template<class = enableIf<!sameTypes<TupleImpl, NoRefPair>::value>>
        TupleImpl operator =(const NoRefPair& other) {
            int unused[] = {(get<idx>() =   other.get<idx>(), 0)...};
            (void) unused;
            return *this;
        }

};

}

template<class... T>
using Tuple = detail::TupleImpl<sequence<0, sizeof...(T)>, T...>;

template<class... T>
Tuple<T...> makeTuple(T&&... args) {
    return Tuple<T...>(args...);
}

template<class... T>
Tuple<T&...> tie(T&... args ) {
    return Tuple<T&...>(args...);
}

}

#endif /* TUPLE_H_ */
