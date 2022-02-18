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

#ifndef CHARLOOKUPTABLE_H_
#define CHARLOOKUPTABLE_H_

#include "meta/Sequence.h"
#include "algorithm/Math.h"

#include <stddef.h>
#include <stdint.h>

namespace pet
{

namespace detail
{
    template<char...> struct FindMax
    {
        static constexpr char value = 0;
    };

    template<char c, char... cs> struct FindMax<c, cs...>
    {
        static constexpr auto value = pet::max(c, FindMax<cs...>::value);
    };

    template<char... c> struct IsOneOf
    {
        static inline constexpr bool check(...) { return false; }
    };

    template<char c, char... cs> struct IsOneOf<c, cs...>
    {
        static inline constexpr bool check(char d)
        {
            return c == d || IsOneOf<cs...>::check(d);
        }
    };

    template<class Seq, size_t, char... cs> class MakeWord;
    template<int... is, size_t base, char... cs> class MakeWord<pet::Sequence<is...>, base, cs...>
    {
        using Set = IsOneOf<cs...>;
        static constexpr bool bits[] = {Set::check(base + is)...};

    public:
        static constexpr inline uint32_t flatten(int idx = 31)
        {
            return (idx >= 0) ? (((bits[idx] ? 1 : 0) << idx) | flatten(idx - 1)) : 0;
        }
    };

    template<class Seq, char... cs> class MakeTable;
    template<int... is, char... cs> class MakeTable<pet::Sequence<is...>, cs...>
    {
    public:
        static inline constexpr uint32_t data[] = {MakeWord<pet::sequence<0, 32>, is * 32, cs...>::flatten()...};
    };
}

template<char... cs>
class CharLookupTable
{
    static constexpr auto limit = detail::FindMax<cs...>::value + 1;
    static constexpr auto nWords = (limit + 31u) / 32u;
    using Table = detail::MakeTable<pet::sequence<0, nWords>, cs...>;

public:
    static constexpr inline bool contains(char c)
    {
        uint8_t v = c;

        if(v >= limit)
            return false;

        return Table::data[v >> 5u] & 1u << (v & 31u);
    }
};

}

#endif /* CHARLOOKUPTABLE_H_ */
