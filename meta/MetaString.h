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
