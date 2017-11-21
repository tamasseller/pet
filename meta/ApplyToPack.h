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

#ifndef APPLYTOPACK_H_
#define APPLYTOPACK_H_

namespace pet {

template<class R, template<R, class, class...> class F, R, class... >
struct ApplyToPack {};

template<class R, template<R, class, class...> class F, R r>
struct ApplyToPack<R, F, r> { static constexpr R value = r; };

template<class R, template<R, class, class...> class F, R r, class C, class... T>
struct ApplyToPack<R, F, r, C, T...> {
    static constexpr R value = F<ApplyToPack<R, F, r, T...>::value, C, T...>::value;
};

}

#endif /* APPLYTOPACK_H_ */
