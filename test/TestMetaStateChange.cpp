/*
 * TestMetaStateChange.cpp
 *
 *  Created on: 2020. jan. 24.
 *      Author: tooma
 */

#include "1test/Test.h"

#include "meta/StateChange.h"

namespace asd {
    STATE(En);

    VALUE(En, s0) {
        MOCK(En)::CALL(0);
    }

    VALUE(En, s1) {
        MOCK(En)::CALL(1);
    }

    STATE(Start);

    VALUE(Start, s0) {
        MOCK(Start)::CALL(0);
    }

    VALUE(Start, s1) {
        MOCK(Start)::CALL(1);
    }

}

STATE(Lo);

VALUE(Lo, s0) {
    MOCK(Lo)::CALL(0);
}

VALUE(Lo, s1) {
    MOCK(Lo)::CALL(1);
}

using MasterState0 = Compound<asd::En<asd::s1>, asd::Start<asd::s1>, Lo<s0>>::Value;
using MasterState1 = Compound<asd::En<asd::s0>, asd::Start<asd::s1>, Lo<s1>>::Value;
using MasterState2 = Compound<asd::En<asd::s1>, asd::Start<asd::s0>, Lo<s0>>::Value;

STATE(Plus);

VALUE(Plus, s0) {
    MOCK(Plus)::CALL(0);
}

VALUE(Plus, s1) {
    MOCK(Plus)::CALL(1);
}

using UberState0 = Compound<MasterState0, Plus<s0>>::Value;
using UberState1 = Compound<MasterState1, Plus<s1>>::Value;

TEST_GROUP(MetaStateChange) {};

TEST(MetaStateChange, Primitive)
{
    apply<Lo<s0>, Lo<s0>>();

    MOCK(Lo)::EXPECT(1);
    apply<Lo<s0>, Lo<s1>>();
}

TEST(MetaStateChange, Compound)
{
    MOCK(En)::EXPECT(0);
    MOCK(Lo)::EXPECT(1);
    apply<MasterState0, MasterState1>();

    MOCK(En)::EXPECT(1);
    MOCK(Start)::EXPECT(0);
    MOCK(Lo)::EXPECT(0);
    apply<MasterState1, MasterState2>();

    MOCK(Start)::EXPECT(1);
    apply<MasterState2, MasterState0>();
}

TEST(MetaStateChange, Nested)
{
    MOCK(En)::EXPECT(0);
    MOCK(Lo)::EXPECT(1);
    MOCK(Plus)::EXPECT(1);
    apply<UberState0, UberState1>();

    MOCK(En)::EXPECT(1);
    MOCK(Lo)::EXPECT(0);
    MOCK(Plus)::EXPECT(0);
    apply<UberState1, UberState0>();
}
