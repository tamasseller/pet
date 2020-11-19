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

TEST_GROUP(Group) {
    bool x = false;
    bool y = false;

    void f(bool ok)
    {
        CHECK(ok);
    }

    TEST_SETUP() {
        x = true;
    }

    TEST_TEARDOWN() {
        f(x && y);
    }
};

TEST(Group, AllOk) {
    y = true;
};

TEST(Group, TeardownNotOk) {
};

TEST(Group, BodyNotOk) {
    f(x && x);
};

TEST_GROUP(OtherGroup) {
    bool x = false;
    bool y = false;

    TEST_SETUP() {
        FAIL("User message");
    }
};

TEST(OtherGroup, SetupNotOk) {

};
