/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

TEST_GROUP(FailureInjector) {
};


TEST(FailureInjector, Sanity) {
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Multiple) {
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Check) {
    CHECK(FailureInjector::shouldSimulateError());
}

TEST(FailureInjector, CheckAlways) {
    FailureInjector::shouldSimulateError();
    CHECK_ALWAYS(false);
}
