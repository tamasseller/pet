/*
 * Atomic.h
 *
 *  Created on: 2020. nov. 7.
 *      Author: doodie
 */

#ifndef ATOMIC_IMPL_H_
#define ATOMIC_IMPL_H_

#include "../gcc-cortex-common/AtomicCommon.h"

#include <stdint.h>

namespace pet {

namespace detail {

template<class Value>
inline bool storeExclusive(volatile Value* addr, Value in) {
	uintptr_t temp = 0;
	bool ret;
	asm volatile(
		"	.thumb					  			\n"
		"	.syntax unified			  			\n"
		"	strex %[ret], %[in], [%[adr]]  	  	\n"
			: [ret] "=&l" (ret)
			: [adr] "l"   (addr),
			  [in]  "l"   (in)
			: "memory"
		);
	return !((bool)ret);
}

template<class Value>
inline Value loadExclusive(volatile Value* addr)
{
	Value ret = *addr;
	asm volatile(
		"	.thumb					  			\n"
		"	.syntax unified			  			\n"
		"	ldrex %[ret], [%[adr]]				  "
			: [ret] "=&l" (ret)
			: [adr] "l"   (addr)
			: "memory"
		);
	return ret;
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
