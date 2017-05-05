/*
 * MetaTest1.cpp
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#include "1test/Test.h"

TEST(Sanity) {
}

TEST(Failure) {
	FAIL();
}

TEST(CheckTrue) {
	CHECK(true);
}

TEST(CheckFalse) {
	CHECK(false);
}
