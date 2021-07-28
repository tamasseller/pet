/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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
#include "1test/FailureInjector.h"

using namespace pet;

static int counter = 0;

TEST_GROUP(FailureInjector)
{
	int x = 0;
};

TEST(FailureInjector, Sanity)
{
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Multiple)
{
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, CheckFailSynthetic)
{
    CHECK(FailureInjector::shouldSimulateError());
}

TEST(FailureInjector, Check)
{
    CHECK(!FailureInjector::shouldSimulateError());
}

TEST(FailureInjector, CheckAlways)
{
    CHECK_ALWAYS(FailureInjector::shouldSimulateError());
}

TEST(FailureInjector, OnOff)
{
	FailureInjector::disable();
    FailureInjector::shouldSimulateError();

    FailureInjector::enable();
    FailureInjector::shouldSimulateError();

    FailureInjector::disable();
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Reinit)
{
	CHECK_ALWAYS(x==0);
	x = 1;
	FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Mock)
{
    if(FailureInjector::shouldSimulateError())
    	MOCK(FailureInjector)::CALL(unexpected);
}

TEST(FailureInjector, MockAlways)
{
    if(FailureInjector::shouldSimulateError())
    	MOCK(FailureInjector)::CALL_ALWAYS(unexpected);
}


TEST_GROUP(FailureInjectorCleanup)
{
	struct Y {
		Y() {counter++;}
		~Y() {counter--;}
	} y;
};

TEST(FailureInjectorCleanup, Destroy)
{
	CHECK_ALWAYS(counter==1);
	FailureInjector::shouldSimulateError();
}
