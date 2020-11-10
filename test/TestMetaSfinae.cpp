/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#include "meta/Sfinae.h"

template<bool x>
typename EnableIf<!x, int>::Type foo() {
	return 0;
}

template<bool x>
typename EnableIf<x, int>::Type foo() {
	return 1;
}

template<bool y>
struct Embedded {
	template<bool x=y>
	typename EnableIf<!x, int>::Type foo() {
		return 0;
	}

	template<bool x=y>
	typename EnableIf<x, int>::Type foo() {
		return 1;
	}
};

TEST_GROUP(MetaSfinae) {};

TEST(MetaSfinae, Simple) {
	CHECK(foo<true>() == 1);
	CHECK(foo<false>() == 0);
}

TEST(MetaSfinae, Embedded) {
	CHECK(Embedded<true>().foo() == 1);
	CHECK(Embedded<false>().foo() == 0);
}
