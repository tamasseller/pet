/*
 * Atomic.sample.h
 *
 *  Created on: 2018.02.28.
 *      Author: tooma
 */

#ifndef ATOMIC_SAMPLE_H_
#define ATOMIC_SAMPLE_H_

/**
 * Atomically modifiable data container.
 *
 * The platform implementation is required to provide
 * a solution, that is usable from any contexts.
 *
 * The _Data_ parameter is at most native machine
 * word sized.
 */
template<class Data> class Atomic {
	/**
	 * Zeroing constructor.
	 *
	 * The default constructor is required to
	 * store the adequate null value.
	 */
	Atomic();

	/**
	 * Non-atomic copy.
	 *
	 * The default copy constructor is not required
	 * to implement atomic access behavior.
	 */
	Atomic(const Data& value);

	/**
	 * Non-atomic getter.
	 */
	operator Data();

	/**
	 * Arbitrary atomic modification.
	 *
	 * The platform implementation is required to execute
	 * the op functor atomically with the following constraints:
	 *
	 *  - it has to provide the old value as the first argument;
	 *  - it has to provide a valid, non-const, lvalue-reference to
	 *    a _Data_ instance that is filled with the new value by the
	 *    functor, as the second argument;
	 *  - it has to forward the additional arguments to the functor;
	 *  - if the boolean return value of the functor is true, it must
	 *    try to apply the new value, otherwise it must abort the
	 *    operation and leave the stored value unchanged;
	 *  - the implementation may restart the whole process (to allow
	 *    for the case of contention);
	 *  - after the operation is finished, it must return the
	 *    resulting value.
	 */
	template<class Op, class... Args> Data operator()(Op&& op, Args... args);
};

#endif /* ATOMIC_SAMPLE_H_ */
