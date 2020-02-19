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

#ifndef TESTBASE_H_
#define TESTBASE_H_

#include "TestInterface.h"
#include "Mock.h"
#include "FailureInjector.h"

#include <stddef.h>

namespace pet {

template<class Child>
class TestBase: public Registry<TestInterface>::StaticElement<Child> {
	/**
	 * Evil, evil, evil hack!
	 *
	 * @See Apology in the body of the _reset_ method.
	 */
	template<class T> struct CtorDtorWrapperHax: T {
		static void* operator new(size_t, CtorDtorWrapperHax* r) {
			return r;
		}

        void operator delete(void*, CtorDtorWrapperHax*) { } // MSVC compat
	};

protected:
    inline void run();
    virtual inline void reset();
    inline void dummy() {}
};

class TestGroupBase {
    public:
        inline void testSetup() {}
        inline void testTeardown() {}
};

}

////////////////////////////////////////////////////////////////

/*
 * It is required to be able to re-run a test case in place, so that:
 *
 *  1. the object for the test can be allocated statically and also
 *  2. the dynamic failure injection can be used without dynamic allocation.
 *
 * So basically for the first, normal run the object is initialized during
 * static initialization along with each other test objects. If the failure
 * injection mechanism requires the test case to be repeated, the memory at
 * the object is placed by the linker is reinitialized and reused.
 *
 * But (!) there is no easy way to do that in such a templated context where
 * the type of the object to be re-initialized is kept in a type parameter,
 * because you can not write t->T() or t->~T if _t_ is a pointer of type
 * parameter T.
 */
template<class Child>
inline void pet::TestBase<Child>::reset()
{
	/**
	 * The actual group type, the one requiring re-initialization.
	 */
	using RealGroup = typename Child::Group;

	/**
	 * Empty wrapper type that is used to implicitly access constructor
	 * and destructor of the actual type.
	 */
	using GroupHandle = CtorDtorWrapperHax<RealGroup>;

	// Navigate down to the child that this TestBase belongs to.
	Child* child = static_cast<Child*>(this);

	// Then find the group data part of the object.
	RealGroup* realGroup = static_cast<RealGroup*>(child);

	/*
	 * This part of the plan is (IMHO) not quite standard compliant, but
	 * in any sensible implementation of an empty wrapper should actually
	 * be the same thing as the original wrapper type, memory-wise.
	 */
	GroupHandle* group = reinterpret_cast<GroupHandle*>(realGroup);

	/*
	 * Call the group destructor implicitly through the wrapper's one.
	 */
	group->~CtorDtorWrapperHax<RealGroup>();

	/*
	 * Call the group default constructor implicitly through the
	 * wrapper's one, which is called by the placement new operator,
	 * that is defined in the wrapper to do nothing else.
	 */
	new(group) GroupHandle;
}

template<class Child>
inline void pet::TestBase<Child>::run()
{
    Child* child = static_cast<Child*>(this);

    MockExpectPool::clear();

    FailureInjector::disable();
    child->testSetup();
    FailureInjector::enable();

    child->testBody();

    FailureInjector::disable();
    child->testTeardown();
    FailureInjector::enable();

    MockExpectPool::check();
}


#endif /* TESTBASE_H_ */
