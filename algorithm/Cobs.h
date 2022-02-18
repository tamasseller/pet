/*******************************************************************************
 *
 * Copyright (c) 2021 Tamás Seller. All rights reserved.
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

#ifndef PET_ALGORITHM_COBS_H_
#define PET_ALGORITHM_COBS_H_

namespace pet {

/**
 * Consistent overhead byte stuffing - encoder
 *
 * Requires the Child to:
 *
 *  - have base class CobsEncoder<Child>,
 *  - implement _char* writeAccess()_ method that returns a pointers to the
 *    next successive byte of the output buffer or null if there is no more.
 *  - implement _bool readInput(char&)_ method that returns false when no
 *    more input data is left, otherwise sets retrieves the next input byte
 *    via the reference argument.
 *
 * NOTE: see https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
template<class Child>
struct CobsEncoder
{
    bool encodeBlock()
    {
        uint8_t code = 1;
        char* codeLocation = static_cast<Child*>(this)->writeAccess();

        if(!codeLocation)
        {
            return false;
        }

        bool skip = false;

        for(char c; static_cast<Child*>(this)->readInput(c);)
        {
            if(c)
            {
                if(auto ptr = static_cast<Child*>(this)->writeAccess())
                {
                    *ptr = c;
                }
                else
                {
                    return false;
                }

                code++;
            }

            if(c == 0 || code == 0xff)
            {
                *codeLocation = code;
                skip = (code == 0xff);
                codeLocation = static_cast<Child*>(this)->writeAccess();

                if(!codeLocation)
                {
                    return false;
                }

                code = 1;
            }
        }

        if(skip && code == 1)
        {
            *codeLocation = 0;
        }
        else
        {
            *codeLocation = code;

            if(auto ptr = static_cast<Child*>(this)->writeAccess())
            {
                *ptr = 0;
            }
            else
            {
                return false;
            }
        }

        return true;
    }
};


/**
 * Consistent overhead byte stuffing - decoder
 *
 * Requires the Child to:
 *
 *  - have base class CobsEncoder<Child>,
 *  - implement _char* writeAccess()_ method that returns a pointers to the
 *    next successive byte of the output buffer or null if there is no more.
 *  - implement _bool readInput(char&)_ method that returns false when no
 *    more input data is left, otherwise sets retrieves the next input byte
 *    via the reference argument.
 *
 * NOTE: see https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
template<class Child>
struct CobsDecoder
{
    bool decodeBlock()
    {
        uint8_t ctr = 1;
        bool skip = true;

        char c;

        while(static_cast<Child*>(this)->readInput(c))
        {
            if(c == 0)
            {
                return true;
            }

            if(!--ctr)
            {
                if(!skip)
                {
                    if(auto ptr = static_cast<Child*>(this)->writeAccess())
                    {
                        *ptr = 0;
                    }
                    else
                    {
                        return false;
                    }
                }

                ctr = c;
                skip = (c == '\xff');
            }
            else
            {
                if(auto ptr = static_cast<Child*>(this)->writeAccess())
                {
                    *ptr = c;
                }
                else
                {
                    return false;
                }
            }
        }

        return false;
    }
};


}

#endif /* PET_ALGORITHM_COBS_H_ */
