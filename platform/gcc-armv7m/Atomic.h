/*
 * Atomic.h
 *
 *  Created on: 2020. nov. 7.
 *      Author: doodie
 */

#ifndef ATOMIC_IMPL_H_
#define ATOMIC_IMPL_H_

#include "../gcc-armv7m/AtomicCommon.h"

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
	return ExclusiveAccessImpl<(int)sizeof(Value)>::store<Value>(addr, in);
}

template<class Value>
inline Value loadExclusive(volatile Value* addr)
{
	return ExclusiveAccessImpl<(int)sizeof(Value)>::load<Value>(addr);
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
