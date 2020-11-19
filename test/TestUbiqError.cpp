/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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
#include "1test/Mock.h"

#include "ubiquitous/Error.h"

using namespace pet;

TEST_GROUP(FailPointer) {
	template <class T>
	static FailPointer<T> returnNull() {
		return 0;
	}

	template <class T>
	static FailPointer<T> returnNonNull() {
		return (T*)123;
	}

	template <class T, FailPointer<T> (*f)()>
	static FailPointer<T> returnUnchecked() {
		return f();
	}

	template <class T, FailPointer<T> (*f)()>
	static FailPointer<T> returnChecked() {
		FailPointer<T> ret = f();
		bool yolo = ret.failed();
		return ret;
	}

	TEST_SETUP() {
		assertMocked = true;
	}
	TEST_TEARDOWN() {
		assertMocked = false;
	}
};

TEST(FailPointer, Sanity) {
	CHECK(returnNull<int>().failed());
	CHECK(!returnNonNull<void>().failed());

    MOCK()::EXPECT(uncheckedErrorReport);
	CHECK(returnNonNull<void>());
}

TEST(FailPointer, ReturnUnchecked) {
    MOCK()::EXPECT(uncheckedErrorReport);

	FailPointer<void> ret = returnUnchecked<void, &returnNonNull<void> >();
	CHECK(ret != 0);
}

TEST(FailPointer, ReturnChecked) {
	FailPointer<void> ret = returnChecked<void, &returnNonNull<void> >();
	CHECK(ret != 0);
}

TEST(FailPointer, ReturnCheckedUnchecked) {
	FailPointer<int> ret = returnChecked<int, &returnUnchecked<int, &returnNull<int> > >();
	CHECK(ret == 0);
}

TEST(FailPointer, ReturnUncheckedChecked) {
	FailPointer<int> ret = returnUnchecked<int, &returnChecked<int, &returnNonNull<int> > >();
	CHECK(ret != 0);
}


TEST_GROUP(FailValue) {
	template <class T>
	static FailValue<T, 123> returnError() {
		return 123;
	}

	template <class T>
	static FailValue<T, 123> returnNonError() {
		return 234;
	}

	template <class T, FailValue<T, 123> (*f)()>
	static FailValue<T, 123> returnUnchecked() {
		return f();
	}

	template <class T, FailValue<T, 123> (*f)()>
	static FailValue<T, 123> returnChecked() {
		FailValue<T, 123> ret = f();
		bool yolo = ret.failed();
		return ret;
	}

	TEST_SETUP() {
		assertMocked = true;
	}
	TEST_TEARDOWN() {
		assertMocked = false;
	}
};

TEST(FailValue, Sanity) {
	CHECK(returnError<int>().failed());
	CHECK(!returnNonError<short>().failed());

    MOCK()::EXPECT(uncheckedErrorReport);
	CHECK(returnError<long long>());
}

TEST(FailValue, ReturnUnchecked) {
    MOCK()::EXPECT(uncheckedErrorReport);

    FailValue<char, 123> ret = returnUnchecked<char, &returnError<char> >();
	CHECK(ret == 123);
}

TEST(FailValue, ReturnChecked) {
    FailValue<short, 123> ret = returnChecked<short, &returnNonError<short> >();
	CHECK(ret != 123);
}

TEST(FailValue, ReturnCheckedUnchecked) {
    FailValue<int, 123> ret = returnChecked<int, &returnUnchecked<int, &returnError<int> > >();
	CHECK(ret == 123);
}

TEST(FailValue, ReturnUncheckedChecked) {
    FailValue<long, 123> ret = returnUnchecked<long, &returnChecked<long, &returnNonError<long> > >();
	CHECK(ret != 123);
}
