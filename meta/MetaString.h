/*
 * MetaString.h
 *
 *      Author: tamas.seller
 */

#ifndef METASTRING_H_
#define METASTRING_H_

#include "Sequence.h"

#include <stddef.h>

namespace pet {

template<char... c>
struct MetaString {
    static constexpr const char characters[] = {c...};
    static constexpr size_t length = sizeof...(c);
};

template<char... c>
constexpr const char MetaString<c...>::characters[];

template<char... c>
constexpr size_t MetaString<c...>::length;

template<class, class> struct MetaStringMaker;

template<class Str, int... i> struct MetaStringMaker<Str, Sequence<i...>> {
    typedef MetaString<Str().characters[i]...> type;
};

constexpr int metaLength(const char* s) {
    return *s ? 1 + metaLength(s + 1) : 0;
}

#define metaStr(x)                                                      \
    [](){                                                               \
        struct Str {                                                    \
            const char characters[metaLength(x)+1] = x;                 \
        };                                                              \
                                                                        \
        return MetaStringMaker<Str, sequence<0, metaLength(x)>>::type();\
    }()

}


#endif /* METASTRING_H_ */
