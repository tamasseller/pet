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

#ifndef TYPEPACKINFO_H_
#define TYPEPACKINFO_H_

#include "meta/ApplyToPack.h"
#include "meta/TypeManipulation.h"

namespace pet {

template<class T> struct IsSomeReference { static constexpr bool value = false; };
template<class T> struct IsSomeReference<T&> { static constexpr bool value = true; };
template<class T> struct IsSomeReference<T&&> { static constexpr bool value = true; };

template<class... T> class TypePackInfo {
    template<class C> struct FindPosition {
        template<int rest, class X, class...> struct Worker {
            static constexpr int value =  (sameTypes<C, X>::value) ? 0 : ((rest == -1) ? -1 : (rest + 1));
        };
    };

public:

    template<class C>
    using Position = ApplyToPack<int, FindPosition<C>::template Worker, -1, T...>;

private:
    template<size_t rest, class X, class...> struct MaxSize { static constexpr size_t value = rest > sizeof(X) ? rest : sizeof(X); };
    template<size_t rest, class X, class...> struct MaxAlign { static constexpr size_t value = rest > alignof(X) ? rest : alignof(X); };
    template<bool rest, class X, class... Y> struct AllUnique { static constexpr bool value = rest && (TypePackInfo<Y...>::template Position<X>::value == -1); };
    template<bool rest, class X, class...> struct NoReferences { static constexpr bool value = rest && !IsSomeReference<X>::value; };

public:

    static constexpr auto size = ApplyToPack<size_t, MaxSize, 0, T...>::value;
    static constexpr auto alignment = ApplyToPack<size_t, MaxAlign, 0, T...>::value;
    static constexpr auto allUnique = ApplyToPack<bool, AllUnique, true, T...>::value;
    static constexpr auto noReferences = ApplyToPack<bool, NoReferences, true, T...>::value;

};

}

#endif /* TYPEPACKINFO_H_ */
