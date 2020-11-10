/*******************************************************************************
 *
 * Copyright (c) 2017 Tamás Seller. All rights reserved.
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

#include "data/MutableStorage.h"

using namespace pet;

TEST_GROUP(MutableStorage) {};

TEST(MutableStorage, Simple) {
	typedef MutableStorage<int, char> Storage;
	Storage storage;

	int* x = storage.construct<int>(4);
	CHECK(*x == 4);

	CHECK(Storage::self(x) == &storage);
	CHECK(storage.as<int>() == x);

	storage.destroy<int>();

	char* y = storage.construct<char>('a');
	CHECK(*y == 'a');

	CHECK(Storage::self(y) == &storage);

	storage.destroy<char>();

}

namespace {
	struct A {
		static int c;
		A() {c++;}
		~A() {c--;}
	};

	int A::c = 0;

	struct B {
		static int c;
		B() {c++;}
		~B() {c--;}
	};

	int B::c = 0;
}

TEST(MutableStorage, CtorDtor) {
	MutableStorage<A, B> storage;

	auto a = storage.construct<A>();
	CHECK(A::c == 1);

	CHECK(storage.as<A>() == a);

	storage.destroy<A>();
	CHECK(A::c == 0);

	auto b = storage.construct<B>();
	CHECK(B::c == 1);

	CHECK(storage.as<B>() == b);

	storage.destroy<B>();
	CHECK(B::c == 0);
}
