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

#ifndef ATOMIC_IMPL_H_
#define ATOMIC_IMPL_H_

#include "../gcc-arm-common/AtomicCommon.h"

#include <stdint.h>

namespace pet {

namespace detail {

template<int Size> struct ExclusiveAccessImpl;

#define __STREX_IMPL_ASM(W)								\
    uintptr_t temp = 0;									\
    bool ret;											\
    asm volatile(										\
        "	.thumb					  			\n"		\
        "	.syntax unified			  			\n"		\
        "	strex" W " %[ret], %[in], [%[adr]]  \n"		\
            : [ret] "=&l" (ret)							\
            : [adr] "l"   (addr),						\
              [in]  "l"   (in)							\
            : "memory"                                  \
        );                                              \
    return !((bool)ret);

#define __LDREX_IMPL_ASM(W)								\
    Value ret = *addr;									\
    asm volatile(										\
        "	.thumb					  		  \n"       \
        "	.syntax unified			  		  \n"       \
        "	ldrex" W " %[ret], [%[adr]]			"       \
            : [ret] "=&l" (ret)                         \
            : [adr] "l"   (addr)                        \
            : "memory"									\
        );												\
    return ret;

template<> struct ExclusiveAccessImpl<4>
{
    template<class Value> static inline bool store(volatile Value* addr, Value in)
    {
        static_assert(sizeof(Value) == 4, "wrong atomic access width");
        __STREX_IMPL_ASM("")
    }

    template<class Value> static inline Value load(const volatile Value* addr)
    {
        static_assert(sizeof(Value) == 4, "wrong atomic access width");
        __LDREX_IMPL_ASM("")
    }
};

template<> struct ExclusiveAccessImpl<2>
{
    template<class Value> static inline bool store(volatile Value* addr, Value in)
    {
        static_assert(sizeof(Value) == 2, "wrong atomic access width");
        __STREX_IMPL_ASM("h")
    }

    template<class Value> static inline Value load(const volatile Value* addr)
    {
        static_assert(sizeof(Value) == 2, "wrong atomic access width");
        __LDREX_IMPL_ASM("h")
    }
};

template<> struct ExclusiveAccessImpl<1>
{
    template<class Value> static inline bool store(volatile Value* addr, Value in)
    {
        static_assert(sizeof(Value) == 1, "wrong atomic access width");
        __STREX_IMPL_ASM("b")
    }

    template<class Value> static inline Value load(const volatile Value* addr)
    {
        static_assert(sizeof(Value) == 1, "wrong atomic access width");
        __LDREX_IMPL_ASM("b")
    }
};

#undef __STREX_IMPL_ASM
#undef __LDREX_IMPL_ASM

template<class Value>
inline bool storeExclusive(volatile Value* addr, Value in)
{
    return ExclusiveAccessImpl<(int)sizeof(Value)>::template store<Value>(addr, in);
}

template<class Value>
inline Value loadExclusive(volatile Value* addr)
{
    return ExclusiveAccessImpl<(int)sizeof(Value)>::template load<Value>(addr);
}

inline void clearExclusive()
{
    asm volatile(
        "	.thumb					  		\n"
        "	.syntax unified			  		\n"
        "	clrex							  "
            :
            :
            : "memory"
        );
}

}

template<class Data>
struct Atomic: CortexCommon::AtomicCommon<
    Data,
    &detail::loadExclusive<Data>,
    &detail::storeExclusive<Data>,
    &detail::clearExclusive>
{
    inline Atomic(): Atomic::AtomicCommon(0) {}
    inline Atomic(Data data): Atomic::AtomicCommon(data) {}
};

}

#endif /* ATOMIC_IMPL_H_ */
