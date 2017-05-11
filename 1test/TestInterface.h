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

#ifndef TESTFWD_H_
#define TESTFWD_H_

#include "ubiquitous/Registry.h"

namespace pet {

class TestRunner;

template<class> class LinkedList;

class TestInterface
{
        friend TestRunner;
        virtual void runTest() = 0;
        virtual const char* getName() = 0;
        virtual const char* getSourceInfo() = 0;
    public:
        inline virtual ~TestInterface() {}
};

}

#endif /* TESTFWD_H_ */
