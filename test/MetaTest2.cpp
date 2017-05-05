/*
 * MetaTest2.cpp
 *
 *  Created on: 2017.05.05.
 *      Author: tooma
 */

#include "1test/Test.h"

void f(bool ok)
{
	CHECK(ok);
}

TEST(FunctionOk) {
	f(true);
}

TEST(FunctionNotOk) {
	f(false);
}
