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

#include "1test/Test.h"

#include "managed/IdentifierRegistry.h"
#include "MockAllocator.h"

#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace pet;

// TODO failable testing
typedef IdentifierRegistry<Allocator> TestRegistry;

TEST_GROUP(RegistrySanity) {
	TEST_TEARDOWN() {
		CHECK(Allocator::allFreed());
	}
};

TEST(RegistrySanity, NonexistentQuery) {
	TestRegistry r;
	CHECK(r.get(666) == 0);
};

TEST(RegistrySanity, InvalidQuery) {
	TestRegistry r;
	CHECK(r.get(-1) == 0);
};


TEST(RegistrySanity, Success) {
	TestRegistry r;
	CHECK(r.final("foo") == 0);
}

TEST(RegistrySanity, ValidQueryAfterSuccess) {
	TestRegistry r;
	CHECK(r.final("foo") == 0);
	CHECK(strcmp(r.get(0), "foo") == 0);
}

TEST(RegistrySanity, SequentialAdd) {
	TestRegistry r;
	CHECK(r.put('f'));
	CHECK(r.put('o'));
	CHECK(r.put('o'));
	CHECK(r.done() == 0);
	CHECK(strcmp(r.get(0), "foo") == 0);
}

TEST(RegistrySanity, SequentialAddFinal) {
	TestRegistry r;
	CHECK(r.put('b'));
	CHECK(r.put('a'));
	CHECK(r.final("r") == 0);
	CHECK(strcmp(r.get(0), "bar") == 0);
}


TEST(RegistrySanity, SuccessAfterNonexistentQuery) {
	TestRegistry r;
	CHECK(r.get(666) == 0);
	CHECK(r.final("foo") == 0);
}

TEST(RegistrySanity, Different) {
	TestRegistry r;
	CHECK(r.final("foo") == 0);
	CHECK(r.final("bar") == 1);
}

TEST(RegistrySanity, DifferentAfterInvalidQuery) {
	TestRegistry r;
	CHECK(r.final("foo") == 0);
	CHECK(r.get(-1) == 0);
	CHECK(r.final("bar") == 1);
}

TEST(RegistrySanity, HashCollision) {
	TestRegistry r;
	CHECK(r.final("liquid") == 0);
	CHECK(r.final("costarring") == 1);
}

TEST_GROUP(Registry) {
	void fill(TestRegistry &r) {
		CHECK(r.final("foo") == 0);
		CHECK(r.final("bar") == 1);
		CHECK(r.final("liquid") == 2);
		CHECK(r.final("costarring") == 3);
		CHECK(r.final("lamantine") == 4);
		CHECK(r.final("lasagne") == 5);
		CHECK(r.final("some") == 6);
		CHECK(r.final("words") == 7);
		CHECK(r.final("are") == 8);
		CHECK(r.final("longer") == 9);
		CHECK(r.final("than") == 10);
		CHECK(r.final("the") == 11);
		CHECK(r.final("other") == 12);
	}

	TEST_TEARDOWN() {
		CHECK(Allocator::allFreed());
	}
};

TEST(Registry, GetNonLast) {
	TestRegistry r;
	fill(r);
	CHECK(strcmp(r.get(6), "some") == 0);
}

TEST(Registry, Refill) {
	TestRegistry r;
	fill(r);
	fill(r);
}

TEST(Registry, Iterator) {
	TestRegistry r;

	fill(r);

	std::vector<std::string> names;
	for(TestRegistry::Iterator it = r.iterator(); it.current(); it.step())
		names.push_back(it.current()->hstr.str());

	std::sort(names.begin(), names.end());

	CHECK(names.size() == 13);
	CHECK(names[0] == "are");
	CHECK(names[1] == "bar");
	CHECK(names[2] == "costarring");
	CHECK(names[3] == "foo");
	CHECK(names[4] == "lamantine");
	CHECK(names[5] == "lasagne");
	CHECK(names[6] == "liquid");
	CHECK(names[7] == "longer");
	CHECK(names[8] == "other");
	CHECK(names[9] == "some");
	CHECK(names[10] == "than");
	CHECK(names[11] == "the");
	CHECK(names[12] == "words");
}

