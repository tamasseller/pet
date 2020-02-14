/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

		Wrapper* const wrapper;
		Container Wrapper::* const field;
		constexpr inline Accessor(Wrapper* wrapper, Container Wrapper::* field) : wrapper(wrapper), field(field) {}
	public:

		constexpr inline operator Type()
		{
			Container value = wrapper->*field;
			auto shiftedValue = value >> offset;
			auto maskedValue = shiftedValue & baseMask;
			return static_cast<Type>(maskedValue);
		}

		inline Type operator =(const Type& value)
		{
			auto castValue = static_cast<const Container>(value);
			auto shiftedValue = castValue << offset;
			wrapper->*field = ((wrapper->*field) & offsetUnmask) | shiftedValue;
			return value;
		}
	};

	struct AccessProvider
	{
		template<class Params, class Wrapper, class Container>
		inline constexpr auto access(Container Wrapper:: *c) {
			return Accessor<Params, Container, Wrapper>(static_cast<Wrapper*>(this), c);
		}

		template<class Params, class Wrapper, class Container>
		inline constexpr auto access(Container Wrapper:: *c) const {
			return Accessor<Params, Container, const Wrapper>(static_cast<const Wrapper*>(this), c);
		}
	};
};

}

#endif /* BITFIELD_H_ */
