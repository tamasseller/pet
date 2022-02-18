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
