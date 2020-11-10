/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#include "meta/StateChange.h"

namespace simple {
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

namespace forced {
    STATE(En);

    VALUE_FORCED(En, s0) {
        MOCK(En)::CALL(0);
    }

    VALUE_FORCED(En, s1) {
        MOCK(En)::CALL(1);
    }
}

STATE(Lo);

VALUE(Lo, s0) {
    MOCK(Lo)::CALL(0);
}

VALUE(Lo, s1) {
    MOCK(Lo)::CALL(1);
}

using MasterState0 = Compound<simple::En<simple::s1>, simple::Start<simple::s1>, Lo<s0>>::Value;
using MasterState1 = Compound<simple::En<simple::s0>, simple::Start<simple::s1>, Lo<s1>>::Value;
using MasterState2 = Compound<simple::En<simple::s1>, simple::Start<simple::s0>, Lo<s0>>::Value;

STATE(Plus);

VALUE(Plus, s0) {
    MOCK(Plus)::CALL(0);
}

VALUE(Plus, s1) {
    MOCK(Plus)::CALL(1);
}

using UberState0 = Compound<MasterState0, Plus<s0>>::Value;
using UberState1 = Compound<MasterState1, Plus<s1>>::Value;

using RepeatedState1 = Compound<simple::En<simple::s0>, Lo<s1>, simple::En<simple::s1>>::Value;
using RepeatedState2 = Compound<simple::En<simple::s1>, Lo<s0>, simple::En<simple::s1>>::Value;

using ForceRepeatedState1 = Compound<forced::En<forced::s0>, Lo<s1>, forced::En<forced::s1>>::Value;
using ForceRepeatedState2 = Compound<forced::En<forced::s1>, Lo<s0>, forced::En<forced::s1>>::Value;

TEST_GROUP(MetaStateChange) {};

TEST(MetaStateChange, Repeat)
{
	MOCK(En)::EXPECT(1);
	MOCK(Lo)::EXPECT(0);
	apply<RepeatedState1, RepeatedState2>();

	MOCK(En)::EXPECT(0);
	MOCK(Lo)::EXPECT(1);
	apply<RepeatedState2, RepeatedState1>();
}

TEST(MetaStateChange, RepeatForced)
{
	MOCK(En)::EXPECT(1);
	MOCK(Lo)::EXPECT(0);
	MOCK(En)::EXPECT(1);
	apply<ForceRepeatedState1, ForceRepeatedState2>();

	MOCK(En)::EXPECT(0);
	MOCK(Lo)::EXPECT(1);
	MOCK(En)::EXPECT(1);
	apply<ForceRepeatedState2, ForceRepeatedState1>();
}


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
