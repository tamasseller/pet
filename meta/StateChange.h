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

#ifndef PET_META_STATECHANGE_H_
#define PET_META_STATECHANGE_H_

#include "ubiquitous/Compiler.h"

template<class T1, class T2>
struct Pair {
    template<class Current> struct Applier;
};

template<class T1, class T2>
template<class N1, class N2>
struct Pair<T1, T2>::Applier<Pair<N1, N2>> {
	really_inline static void apply() {
        T1::template Applier<N1>::apply();
        T2::template Applier<N2>::apply();
    }
};


template<class... Params> struct Pairs;

template<class P, class... Params>
struct Pairs<P, Params...> {
    using Type = Pair<P, typename Pairs<Params...>::Type>;
};

struct Stopper {
    template<class Current> struct Applier;
};

template<>
struct Stopper::Applier<Stopper> {
	really_inline static void apply() {}
};

template<>
struct Pairs<> {
    using Type = Stopper;
};

template<class... Params> struct Compound
{
    using Value = typename Pairs<Params...>::Type;
};

#define STATE(stateVar)                												\
template<class> struct stateVar;

#define VALUE(stateVar, value)                                                      \
class value;                                                                        \
template<> struct stateVar<value> {                                                 \
    template<class> struct Applier;                                                 \
};                                                                                  \
                                                                                    \
template<> template<>                                                               \
struct stateVar<value>::Applier<stateVar<value>> {									\
	really_inline static void apply() {} };        									\
                                                                                    \
template<> template<class otherValue>                                               \
struct stateVar<value>::Applier<stateVar<otherValue>> {                             \
	inline static void apply();                                                     \
};                                                                                  \
                                                                                    \
template<> template<class otherValue>                                               \
really_inline void stateVar<value>::Applier<stateVar<otherValue>>::apply()

#define VALUE_FORCED(stateVar, value)                                               \
class value;                                                                        \
template<> struct stateVar<value> {                                                 \
    template<class> struct Applier;                                                 \
};                                                                                  \
                                                                                    \
template<> template<class otherValue>                                               \
struct stateVar<value>::Applier<stateVar<otherValue>> {                             \
	inline static void apply();                                                     \
};                                                                                  \
                                                                                    \
template<> template<class otherValue>                                               \
really_inline void stateVar<value>::Applier<stateVar<otherValue>>::apply()

template<class Current, class New>
really_inline static void apply()
{
    New::template Applier<Current>::apply();
}

#endif /* PET_META_STATECHANGE_H_ */
