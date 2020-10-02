/*
 * Prng.h
 *
 *  Created on: 2020. febr. 4.
 *      Author: tooma
 */

#ifndef PRNG_H_
#define PRNG_H_

#include <stdint.h>

namespace pet {

class Prng
{
    uint32_t state;
    const uint32_t increment;

    static constexpr auto multiplier  = 1103515245u;
    static constexpr auto stateWidth  = 32;
    static constexpr auto outputWidth = 16;
    static constexpr auto rotWidth    = 4;
    static constexpr auto truncShift  = outputWidth - rotWidth;
    static constexpr auto mixShift    = (stateWidth - truncShift) / 2;

    inline uint32_t ensureNonZero(uint32_t x)
    {
		const auto foldTop = (x ^ (x >> 1));
		return (foldTop << 1) + 1;
	}
	
	inline uint32_t step()
	{
		const auto ret = state;
		state = state * multiplier + increment;
		return ret;
	}

public:
    inline Prng (uint32_t seed, uint32_t increment): state(ensureNonZero(seed)), increment(ensureNonZero(increment)) 
    {
		for(int i=0; i<3; i++)
			step();
    }

    inline uint16_t rand16()
    {
        const auto value = step();

        const auto rot          = value >> (stateWidth - rotWidth);
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

}

#endif /* PRNG_H_ */
