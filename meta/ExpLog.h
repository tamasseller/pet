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

#ifndef EXPLOG_H_
#define EXPLOG_H_

namespace pet {

namespace detail {
    template<unsigned int base, unsigned int n>
    struct logWorker {
        static constexpr auto value = logWorker<base, n/base>::value + 1;
    };

    template<unsigned int base>
    struct logWorker<base, 0> {
        static constexpr auto value = -1u;
    };

    template<unsigned int base, unsigned int n>
    struct expWorker {
        static constexpr auto value = expWorker<base, n-1>::value * base;
    };

    template<unsigned int base>
    struct expWorker<base, 0> {
        static constexpr auto value = 1u;
    };
}

template<unsigned int base>
struct log {
    template<unsigned int n>
    using x = detail::logWorker<base, n>;
};

template<unsigned int base>
struct exp {
    template<unsigned int n>
    using x = detail::expWorker<base, n>;
};

}

#endif /* EXPLOG_H_ */
