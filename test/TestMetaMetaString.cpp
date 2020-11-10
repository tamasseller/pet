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

#ifndef TESTMETAMETASTRING_CPP_
#define TESTMETAMETASTRING_CPP_

#include "1test/Test.h"

#include "meta/MetaString.h"

#include "algorithm/Str.h"

using namespace pet;

static_assert(metaLength("") == 0, "pet::metaLength error");
static_assert(metaLength("foo") == 3, "pet::metaLength error");
static_assert(metaLength("foobar") == 6, "pet::metaLength error");

TEST_GROUP(MetaString) {};

TEST(MetaString, Local) {
    auto foo = metaStr("foo");
    typedef decltype(foo) C;
    CHECK(Str::ncmp("foobar", C::characters, C::length) == 0);
}

template<class C>
void receiver(const C&) {
        CHECK(Str::ncmp("foobar", C::characters, C::length) == 0);
}

TEST(MetaString, Param) {
    receiver(metaStr("foobar"));
}



#endif /* TESTMETAMETASTRING_CPP_ */
