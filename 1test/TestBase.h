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

#ifndef TEST_H_
#define TEST_H_

#include "TestHandle.h"

namespace pet {

template<class Child>
class TestBase: public TestHandle {
protected:
	TestBase();
	static Child instance;
};

}

////////////////////////////////////////////////////////////////

#include "TestRunner.h"

template<class Child>
Child pet::TestBase<Child>::instance;

template<class Child>
pet::TestBase<Child>::TestBase()
{
	TestRunner::registerTest(&instance);
}

#endif /* TEST_H_ */
