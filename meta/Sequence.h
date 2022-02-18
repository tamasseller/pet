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
#ifndef SEQUENCE_H_
#define SEQUENCE_H_

namespace pet {

template<int... T>
class Sequence {};

namespace detail {

template<int... rest> struct SequenceMaker;

template<int min, int... rest> struct SequenceMaker<min, rest...> {
    using type = typename SequenceMaker<min - 1, min, rest...>::type;
};

template<int... rest> struct SequenceMaker<0, rest...> {
    using type = Sequence<0, rest...>;
};

template<> struct SequenceMaker<-1> {
    using type = Sequence<>;
};


template<int shift>
struct SequenceShifter {
    template<int... idx>
    static Sequence<(idx + shift)...> x(const Sequence<idx...>&) {return Sequence<(idx + shift)...>();}
};

}

template<int min, int max>
using sequence = decltype(detail::SequenceShifter<min>::x(typename detail::SequenceMaker<max - min - 1>::type()));

}

#endif /* SEQUENCE_H_ */
