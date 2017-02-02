/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#include "managed/IdentifierRegistry.h"
#include "MockAllocator.h"

#include "CppUTest/TestHarness.h"

#include <cstring>

#undef new

using namespace container;

struct HstringHelper: public IdentifierRegistryInternals::Hstring {
	inline HstringHelper(const char* str): IdentifierRegistryInternals::Hstring(str){}
	inline void* operator new(unsigned int u, void* ret){
		return ret;
	}

};

IdentifierRegistryInternals::Hstring* create(const char *str){
	char *ret = (char*)Allocator::alloc((unsigned int)sizeof(IdentifierRegistryInternals::Hstring)+strlen(str)+1);

	int i;
	for(i=0; i<strlen(str); i++)
		ret[sizeof(IdentifierRegistryInternals::Hstring)+i] = str[i];

	ret[i+sizeof(IdentifierRegistryInternals::Hstring)] = '\0';

	return new(ret) HstringHelper(str);
}


TEST_GROUP(HString) {
	IdentifierRegistryInternals::Hstring *sp, *asd, *qwe, *liquid, *costarring;

	TEST_SETUP() {
		sp = create("");
		asd = create("asd");
		qwe = create("qwe");
		liquid = create("liquid");
		costarring = create("costarring");
	}

	TEST_TEARDOWN() {
		Allocator::free(sp);
		Allocator::free(asd);
		Allocator::free(qwe);
		Allocator::free(liquid);
		Allocator::free(costarring);
		CHECK(Allocator::allFreed());
	}
};

TEST(HString, SameEqual) {
	CHECK(*sp-*sp == 0);
	CHECK(*asd-*asd == 0);
	CHECK(*qwe-*qwe == 0);
	CHECK(*liquid-*liquid == 0);
	CHECK(*costarring-*costarring == 0);
}

TEST(HString, Greater) {
	CHECK(*sp-*qwe > 0);
	CHECK(*liquid-*sp > 0);
	CHECK(*liquid-*costarring > 0);
	CHECK(*costarring-*sp > 0);
	CHECK(*asd-*sp > 0);
	CHECK(*asd-*liquid > 0);
	CHECK(*asd-*costarring > 0);
	CHECK(*qwe-*asd > 0);
	CHECK(*qwe-*liquid > 0);
	CHECK(*qwe-*costarring > 0);
}

TEST(HString, Smaller) {
	CHECK(*asd-*qwe < 0);
	CHECK(*qwe-*sp < 0);
	CHECK(*sp-*asd < 0);
	CHECK(*sp-*liquid < 0);
	CHECK(*sp-*costarring < 0);
	CHECK(*liquid-*asd < 0);
	CHECK(*liquid-*qwe < 0);
	CHECK(*costarring-*asd < 0);
	CHECK(*costarring-*qwe < 0);
	CHECK(*costarring-*liquid < 0);
}
