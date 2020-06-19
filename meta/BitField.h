/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tam�s. All rights reserved.
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

#ifndef BITFIELD_H_
#define BITFIELD_H_

#include <stddef.h>
#include <stdint.h>

#include "platform/Compiler.h"

namespace pet {

struct BitField
{
    template<uint8_t offset, uint8_t length, class Type> struct Params;

    template<class Params, class Container, class Wrapper> class Accessor;
    template<uint8_t offset, uint8_t length, class Type, class Container, class Wrapper>
    class Accessor<Params<offset, length, Type>, Container, Wrapper>
    {
        static constexpr Container baseMask = (1u << length) - 1;
        static constexpr Container offsetUnmask = (Container)(~(baseMask << offset));

        friend BitField;
        static_assert(sizeof(Type) <= sizeof(uint32_t));
        static_assert(sizeof(Type) <= sizeof(Container));

    public:
        static constexpr really_inline Type read(Container Wrapper::* field, Wrapper* wrapper)
        {
            Container value = wrapper->*field;
            auto shiftedValue = value >> offset;
            auto maskedValue = shiftedValue & baseMask;
            return static_cast<Type>(maskedValue);
        }

        static really_inline void write(Container Wrapper::* field, Wrapper* wrapper, const Type& value)
        {
            auto castValue = static_cast<const Container>(value);
            auto shiftedValue = castValue << offset;
            wrapper->*field = ((wrapper->*field) & offsetUnmask) | shiftedValue;
        }
    };

    struct AccessProvider
    {
        template<class Params, class Wrapper, class Container, class Type>
        really_inline void write(Container Wrapper:: *c, Type v) {
            Accessor<Params, Container, Wrapper>::write(c, static_cast<Wrapper*>(this), v);
        }

        template<class Params, class Wrapper, class Container>
        really_inline constexpr auto read(Container Wrapper:: *c) const {
            return Accessor<Params, Container, const Wrapper>::read(c, static_cast<const Wrapper*>(this));
        }
    };
};

}

#endif /* BITFIELD_H_ */
