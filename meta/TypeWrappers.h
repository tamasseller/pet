/*
 * TypeWrappers.h
 *
 *  Created on: 2017.05.31.
 *      Author: tooma
 */

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
