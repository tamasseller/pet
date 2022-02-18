/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#ifndef INTEGERDIVISION_H_
#define INTEGERDIVISION_H_

#include <stdint.h>

namespace pet {

    /**
     *	Slow but small unsigned integer division algorithm.
     *
     *	This implementation is mainly useful in scenarios where the CPU does not have
     *	hardware implementation for integer division and the compiler generated algorithm
     *	is not suitable. One possible reason for it is that the compiler has a higher
     *	performance but also bigger footprint implementation and the application is code
     *	size constrained.
     *
     * @param	_x_ the divident.
     * @param	_y_ the divisor.
     * @return	The integer part of quotient _x_/_y_.
     */
    static inline uint32_t uDiv32(uint32_t x, uint32_t y)
    {
        uint32_t ret = 0;

        while(x >= y) {
            uint32_t c = 1;
            uint32_t t = y;

            while(1)
            {
                const uint32_t shifted = t << 1;

                if((shifted > x) || (shifted < t))
                    break;

                t = shifted;
                c <<= 1;
            }

            x -= t;
            ret |= c;
        }

        return ret;
    }


    /**
     *	Slow but small signed integer division algorithm.
     *
     *	This implementation is mainly useful in scenarios where the CPU does not have
     *	hardware implementation for integer division and the compiler generated algorithm
     *	is not suitable. One possible reason for it is that the compiler has a higher
     *	performance but also bigger footprint implementation and the application is code
     *	size constrained.
     *
     * @param	_x_ the divident.
     * @param	_y_ the divisor.
     * @return	The integer part of quotient _x_/_y_.
     */
    static inline int32_t iDiv32(int32_t x, int32_t y)
    {
        bool inv = false;

        if(x < 0) {
            x = -x;
            inv = true;
        }

        if(y < 0) {
            y = -y;
            inv = !inv;
        }

        return inv ? -(int32_t)uDiv32(x, y) : uDiv32(x, y);
    }
}

#endif /* FW_LIB_PET_ALGORITHM_INTEGERDIVISION_H_ */
