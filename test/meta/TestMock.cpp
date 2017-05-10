/*
 * Test1Mock.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"
#include "1test/Mock.h"

TEST_GROUP(Mock) {};

TEST(Mock, Sane) {
    MOCK(test)::EXPECT(foo);
    MOCK(test)::CALL(foo);
}

TEST(Mock, Incomplete) {
    MOCK(test)::EXPECT(foo);
}

TEST(Mock, Multiple) {
    MOCK(test)::EXPECT(foo);
    MOCK(test)::EXPECT(bar);
    MOCK(test)::CALL(foo);
    MOCK(test)::CALL(bar);
}

TEST(Mock, MultipleIncomplete) {
    MOCK(test)::EXPECT(foo);
    MOCK(test)::EXPECT(bar);
    MOCK(test)::CALL(foo);
}

TEST(Mock, WrongOrder) {
    MOCK(test)::EXPECT(foo);
    MOCK(test)::EXPECT(bar);
    MOCK(test)::CALL(bar);
}

TEST(Mock, Unexpected) {
    MOCK(test)::CALL(foo);
}

TEST(Mock, IndependentSourcesOk) {
    MOCK(foo)::EXPECT(method);
    MOCK(bar)::EXPECT(method);

    MOCK(foo)::CALL(method);
    MOCK(bar)::CALL(method);
}

TEST(Mock, IndependentSourcesFail) {
    MOCK(foo)::EXPECT(method);
    MOCK(bar)::CALL(method);
}

TEST(Mock, ParamsSane) {
    MOCK(params)::EXPECT(method).withParam(1);
    MOCK(params)::CALL(method).withParam(1);
}

TEST(Mock, ParamsNoParamExpected) {
    MOCK(params)::EXPECT(method);
    MOCK(params)::CALL(method).withParam(1);
}

TEST(Mock, ParamsNoParamConsumed) {
    MOCK(params)::EXPECT(method).withParam(1);
    MOCK(params)::CALL(method);
}

TEST(Mock, Params) {
    MOCK(params)::EXPECT(method).withParam(1);
    MOCK(params)::CALL(method).withParam(1);
}

TEST(Mock, ParamsWrong) {
    MOCK(params)::EXPECT(method).withParam(1);
    MOCK(params)::CALL(method).withParam(-1);
}

TEST(Mock, ParamsString) {
    const char *str1 = "foo";
    const char *str2 = "foo";
    MOCK(params)::EXPECT(method).withParam(str1);
    MOCK(params)::CALL(method).withParam(str2);
}

TEST(Mock, ParamsStringWrong) {
    const char *str1 = "foo";
    const char *str2 = "bar";
    MOCK(params)::EXPECT(method).withParam(str1);
    MOCK(params)::CALL(method).withParam(str2);
}

TEST(Mock, ParamsStringLegacy) {
    char *str1 = (char *)"foo";
    char *str2 = (char *)"foo";
    MOCK(params)::EXPECT(method).withParam(str1);
    MOCK(params)::CALL(method).withParam(str2);
}

TEST(Mock, ParamsData) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xda, 0x7a};
    MOCK(params)::EXPECT(method).withParam(data1, sizeof(data1));
    MOCK(params)::CALL(method).withParam(data2, sizeof(data2));
}

TEST(Mock, ParamsDataWrong) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xba, 0xad};
    MOCK(params)::EXPECT(method).withParam(data1, sizeof(data1));
    MOCK(params)::CALL(method).withParam(data2, sizeof(data2));

}

TEST(Mock, ParamsDataWrongLength) {
    unsigned char data1[] = {0xda, 0x7a};
    unsigned char data2[] = {0xda, 0x7a, 0xba, 0xad};
    MOCK(params)::EXPECT(method).withParam(data1, sizeof(data1));
    MOCK(params)::CALL(method).withParam(data2, sizeof(data2));

}

TEST(Mock, ParamsPointer) {
    struct Object {int x;} obj1;
    MOCK(params)::EXPECT(method).withParam(&obj1);
    MOCK(params)::CALL(method).withParam(&obj1);
}

TEST(Mock, ParamsPointerWrong) {
    struct Object {int x;} obj1, obj2;
    MOCK(params)::EXPECT(method).withParam(&obj1);
    MOCK(params)::CALL(method).withParam(&obj2);
}

TEST(Mock, DisabledTriggersNoError) {
    MOCK(disabled)::disable();
    MOCK(disabled)::EXPECT(method);
}

TEST(Mock, ReenablingTriggersNoError) {
    MOCK(disabled)::disable();
    MOCK(disabled)::EXPECT(method);
    MOCK(disabled)::enable();
}

TEST(Mock, DisabledIndependentUnexpected) {
    MOCK(disabled)::disable();
    MOCK(disabled)::EXPECT(method);

    MOCK(enabled)::EXPECT(method);
}
