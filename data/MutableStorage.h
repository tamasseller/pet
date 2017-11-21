/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef MUTABLESTORAGE_H_
#define MUTABLESTORAGE_H_

#include "meta/TypePackInfo.h"

namespace pet {

template<class... HeldTypes>
class MutableStorage {
	static constexpr auto size = (TypePackInfo<HeldTypes...>::size + sizeof(char) - 1) / sizeof(char);
	char data[size] alignas(TypePackInfo<HeldTypes...>::alignment);

	template<class Type>
    struct Hax {
		Type data;

        static void* operator new(size_t, void* r) {
            return r;
        }

        template<class... C> Hax(C... c): data(c...) {}
    };

public:
    template<class Type, class... C>
    inline Type* construct(C... c) {
    	static_assert(TypePackInfo<HeldTypes...>::template Position<Type>::value != -1, "Unfamiliar type requested");
    	auto obj = new(data) Hax<Type>(c...);
    	return &obj->data;
    }

    template<class Type>
    inline Type* as() {
    	static_assert(TypePackInfo<HeldTypes...>::template Position<Type>::value != -1, "Unfamiliar type requested");
    	auto obj = reinterpret_cast<Hax<Type>*>(data);
		return &obj->data;
    }

    template<class Type>
    inline void destroy() {
    	static_assert(TypePackInfo<HeldTypes...>::template Position<Type>::value != -1, "Unfamiliar type requested");
    	reinterpret_cast<Hax<Type>*>(data)->~Hax<Type>();
    }

    template<class Type>
    static inline MutableStorage *self(Type* object) {
    	static_assert(TypePackInfo<HeldTypes...>::template Position<Type>::value != -1, "Unfamiliar type requested");
    	return reinterpret_cast<MutableStorage*>(object);
    }
};

}

#endif /* MUTABLESTORAGE_H_ */
