/*
 * Test1Mock.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"
#include "1test/Mock.h"

TEST(MockSane) {
    CHECK(MOCK("test")::expect("foo"));
    CHECK(MOCK("test")::call("foo"));
    CHECK(MOCK("test")::check());
}

TEST(MockIncomplete) {
    CHECK(MOCK("test")::expect("foo"));
    CHECK(!MOCK("test")::check());
    MOCK()::clear();
}

TEST(MockMultiple) {
    CHECK(MOCK("test")::expect("foo"));
    CHECK(MOCK("test")::expect("bar"));
    CHECK(MOCK("test")::call("foo"));
    CHECK(MOCK("test")::call("bar"));
    CHECK(MOCK("test")::check());
}

TEST(MockMultipleIncomplete) {
    CHECK(MOCK("test")::expect("foo"));
    CHECK(MOCK("test")::expect("bar"));
    CHECK(MOCK("test")::call("foo"));
    CHECK(!MOCK("test")::check());
    MOCK()::clear();
}

TEST(MockWrongOrder) {
    CHECK(MOCK("test")::expect("foo"));
    CHECK(MOCK("test")::expect("bar"));
    CHECK(!MOCK("test")::call("bar"));
    MOCK()::clear();
}

TEST(MockUnexpected) {
    CHECK(!MOCK("test")::call("foo"));
}

TEST(MockIndependentSourcesOk) {
    CHECK(MOCK("foo")::expect("method"));
    CHECK(MOCK("bar")::expect("method"));

    CHECK(MOCK("foo")::call("method"));
    CHECK(MOCK("bar")::call("method"));

    CHECK(MOCK("test")::check());
}

TEST(MockIndependentSourcesFail) {
    CHECK(MOCK("foo")::expect("method"));
    CHECK(!MOCK("bar")::call("method"));
    MOCK()::clear();
}

TEST(MockParamsSane) {
    CHECK(MOCK("params")::expect("method").withParam(1));
    CHECK(MOCK("params")::call("method").withParam(1));
    CHECK(MOCK("test")::check());
}

TEST(MockParamsNoParamExpected) {
    CHECK(MOCK("params")::expect("method"));
    CHECK(!MOCK("params")::call("method").withParam(1));
    MOCK()::clear();
}

TEST(MockParamsNoParamConsumed) {
    CHECK(MOCK("params")::expect("method").withParam(1));
    CHECK(MOCK("params")::call("method"));
    CHECK(!MOCK("test")::check());
    MOCK()::clear();
}

TEST(MockParamsString) {
    const char *str1 = "foo";
    const char *str2 = "foo";
    CHECK(MOCK("params")::expect("method").withParam(str1));
    CHECK(MOCK("params")::call("method").withParam(str2));
    CHECK(MOCK("test")::check());
}

TEST(MockParamsWrong) {
    const char *str1 = "foo";
    const char *str2 = "bar";
    CHECK(MOCK("params")::expect("method").withParam(str1));
    CHECK(!MOCK("params")::call("method").withParam(str2));
    MOCK()::clear();
}

TEST(MockParamsStringLegacy) {
    char *str1 = (char *)"foo";
    char *str2 = (char *)"foo";
    CHECK(MOCK("params")::expect("method").withParam(str1));
    CHECK(MOCK("params")::call("method").withParam(str2));
    CHECK(MOCK("test")::check());
}

TEST(MockParamsData) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xda, 0x7a};
    CHECK(MOCK("params")::expect("method").withParam(data1, sizeof(data1)));
    CHECK(MOCK("params")::call("method").withParam(data2, sizeof(data2)));
    CHECK(MOCK("test")::check());
}

TEST(MockParamsDataWrong) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xba, 0xad};
    CHECK(MOCK("params")::expect("method").withParam(data1, sizeof(data1)));
    CHECK(!MOCK("params")::call("method").withParam(data2, sizeof(data2)));
    MOCK()::clear();
}

TEST(MockParamsDataWrongLength) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xda, 0x7a, 0xba, 0xad};
    CHECK(MOCK("params")::expect("method").withParam(data1, sizeof(data1)));
    CHECK(!MOCK("params")::call("method").withParam(data2, sizeof(data2)));
    MOCK()::clear();
}

TEST(MockParamsPointer) {
    struct Object {int x;} obj1;
    CHECK(MOCK("params")::expect("method").withParam(&obj1));
    CHECK(MOCK("params")::call("method").withParam(&obj1));
    CHECK(MOCK("test")::check());
}

TEST(MockParamsPointerWrong) {
    struct Object {int x;} obj1, obj2;
    CHECK(MOCK("params")::expect("method").withParam(&obj1));
    CHECK(!MOCK("params")::call("method").withParam(&obj2));
    MOCK()::clear();
}

