/*
 * TestMetaMetaString.cpp
 *
 *      Author: tamas.seller
 */

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
