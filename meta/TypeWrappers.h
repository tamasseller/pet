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

#ifndef TYPEWRAPPERS_H_
#define TYPEWRAPPERS_H_

namespace pet {

/*
 * Note:
 * 		It is pretty much like std::integral_constant.
 */

template<class Type, Type InputValue>
struct Value {
    static constexpr Type value = InputValue;
    constexpr operator Type() const noexcept { return value; }
    constexpr Type operator()() const noexcept { return value; }
};

typedef Value<bool, true> True;
typedef Value<bool, false> False;

}



#endif /* TYPEWRAPPERS_H_ */
