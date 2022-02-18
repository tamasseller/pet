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

#ifndef PRNG_H_
#define PRNG_H_

#include <stdint.h>

namespace pet {

template<class Child>
class PrngBase
{
    uint32_t state;

    static constexpr auto multiplier  = 1103515245u;
    static constexpr auto stateWidth  = 32;
    static constexpr auto outputWidth = 16;
    static constexpr auto rotWidth    = 4;
    static constexpr auto truncShift  = outputWidth - rotWidth;			// 12
    static constexpr auto mixShift    = (stateWidth - truncShift) / 2;	// 10

    inline uint32_t step()
    {
        const auto ret = state;
        state = state * multiplier + static_cast<Child*>(this)->getIncrement();
        return ret;
    }

protected:
    static inline uint32_t ensureOdd(uint32_t x)
    {
        const auto foldTop = (x ^ (x >> 1));
        return (foldTop << 1) + 1;
    }

public:
    inline PrngBase(uint32_t seed): state(ensureOdd(seed))
    {
        for(int i=0; i<3; i++)
            step();
    }

    inline uint16_t rand16()
    {
        const auto value = step();

        /*
         * 	pppppppp qqqqqqqq rrrrrrrr ssssssss
         * 	|  |
         * 	+--+
         * 	ROT
         */
        const auto rot          = value >> (stateWidth - rotWidth);

        /*
         * 	pppppppp qqqqqqqq rrrrrrrr ssssssss
         *
         * XOR
         *
         *  00000000 00pppppp ppqqqqqq qqrrrrrr
         *      |      	         |
         *      +----------------+
         *          truncated
         */
        const auto mixed        = value ^ (value >> mixShift);
        const auto truncated    = (uint16_t)(mixed >> truncShift);

        const auto extended     = ((uint32_t)truncated << 16) | truncated;

        return extended >> rot;
    }

    inline uint16_t rand16(uint16_t range) {
        return (rand16() * range) >> 16;
    }

    inline uint8_t rand8()
    {
        auto r = rand16();
        return (r >> 8) ^ r;
    }

    inline uint32_t rand32() {
        return ((uint32_t)rand16() << 16) | rand16();
    }
};

class Prng: PrngBase<Prng>
{
    friend Prng::PrngBase;

    const uint32_t increment;

    inline auto getIncrement() {
        return increment;
    }

public:
    inline Prng(uint32_t seed, uint32_t increment): PrngBase(seed), increment(ensureOdd(increment)) {}

    using Prng::PrngBase::rand8;
    using Prng::PrngBase::rand16;
    using Prng::PrngBase::rand32;
};

}

#endif /* PRNG_H_ */
