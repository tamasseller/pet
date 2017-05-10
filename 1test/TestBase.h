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

namespace pet {

template<class Child>
class TestBase: public Registry<TestInterface>::StaticElement<Child> {
protected:
    inline void run();
    inline void dummy() {}
};

class TestGroupBase {
    public:
        inline void testSetup() {}
        inline void testTeardown() {}
};

}

////////////////////////////////////////////////////////////////

template<class Child>
inline void pet::TestBase<Child>::run()
{
    pet::MockExpectPool::clear();
    static_cast<Child*>(this)->testSetup();
    static_cast<Child*>(this)->testBody();
    static_cast<Child*>(this)->testTeardown();
    pet::MockExpectPool::check();
}


#endif /* TESTBASE_H_ */
