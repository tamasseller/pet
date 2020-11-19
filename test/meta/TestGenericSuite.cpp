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

#include "1test/Test.h"

template<class Uut>
struct GenericSuite
{
    BEGIN_TEST_CASE(ParametricUngrouped)
    {
        CHECK(true);
    }
    END_TEST_CASE()

    TEST_GROUP(ParametricGroup1) {
        bool ok = true;
    };

    BEGIN_TEST_CASE(ParametricGroup1, ParametricTest1)
    {
        CHECK(this->ok);
    }
    END_TEST_CASE()

    BEGIN_TEST_CASE(ParametricGroup1, ParametricTest2)
    {
        CHECK(!this->ok);
    }
    END_TEST_CASE()

    TEST_GROUP(ParametricGroup2) {
        bool ok = false;
    };

    BEGIN_TEST_CASE(ParametricGroup2, ParametricTest3)
    {
        CHECK(this->ok);
    }
    END_TEST_CASE()

    BEGIN_TEST_CASE(ParametricGroup2, ParametricTest4)
    {
        CHECK(!this->ok);
    }
    END_TEST_CASE()
};

template class GenericSuite<int>;
template class GenericSuite<void>;
