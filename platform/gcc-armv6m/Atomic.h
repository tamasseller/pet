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

extern volatile bool exclusiveMonitor;

template<int Size> struct ExclusiveAccessImpl;

#define __STREX_IMPL_ASM(W)												       \
    uintptr_t temp = 0; 												       \
    bool ret;																   \
    asm volatile(														       \
        "	.thumb					  \n"									   \
        "	.syntax unified			  \n"                                      \
        "	cpsid i					  \n" /* Atomically:					*/ \
        "	ldrb %[ret], [%[mon]]	  \n" /* 		1. load the monitor.    */ \
        "	cmp %[ret], #0			  \n" /*  	    2. check if contended.  */ \
        "	beq 1f					  \n" /*	 	3. skip next if it is.  */ \
        "	str" W " %[in], [%[adr]] \n" /* 		4. store the new value. */ \
        "1: strb %[tmp], [%[mon]]	  \n" /* 		5. reset monitor.       */ \
        "	cpsie i\n"                                                         \
            : [ret] "=&l" (ret)                                                \
            : [mon] "l"   (&exclusiveMonitor),							       \
              [tmp] "l"   (temp),                                              \
              [adr] "l"   (addr),                                              \
              [in]  "l"   (in) : "memory"                                      \
        );																	   \
        return ret;

template<> struct ExclusiveAccessImpl<4> {
    template<class Value> static inline bool work(volatile Value* addr, Value in) {
        static_assert(sizeof(Value) == 4, "wrong atomic access width");
        __STREX_IMPL_ASM("")
    }
};

template<> struct ExclusiveAccessImpl<2> {
    template<class Value> static inline bool work(volatile Value* addr, Value in) {
        static_assert(sizeof(Value) == 2, "wrong atomic access width");
        __STREX_IMPL_ASM("h")
    }
};

template<> struct ExclusiveAccessImpl<1> {
    template<class Value> static inline bool work(volatile Value* addr, Value in) {
        static_assert(sizeof(Value) == 1, "wrong atomic access width");
        __STREX_IMPL_ASM("b")
    }
};

#undef __STREX_IMPL_ASM

template<class Value>
inline bool storeExclusive(volatile Value* addr, Value in) {
    return ExclusiveAccessImpl<(int)sizeof(Value)>::work(addr, in);
}

template<class Value>
inline Value loadExclusive(volatile Value* addr)
{
    exclusiveMonitor = true;
    Value ret = *addr;
    asm volatile("":::"memory");
    return ret;
}

inline void clearExclusive()
{
    exclusiveMonitor = false;
    asm volatile("":::"memory");
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
