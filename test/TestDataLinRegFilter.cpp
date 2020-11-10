/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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

#include "data/LinRegFilter.h"

using namespace pet;

TEST_GROUP(LinRegFilter) {};

TEST(LinRegFilter, Sanity1)
{
    LinRegFilter<2, 0> filt;

    int32_t nA, nB, d;
    filt.update(0, 2, nA, nB, d);
    filt.update(1, 3, nA, nB, d);

    CHECK(nA / d == 1);
    CHECK(nB / d == 2);
}

TEST(LinRegFilter, Sanity2)
{
    LinRegFilter<3, 0> filt;

    int32_t nA, nB, d;
    filt.update(0, 2, nA, nB, d);
    filt.update(1, 3, nA, nB, d);
    filt.update(2, 4, nA, nB, d);

    CHECK(nA / d == 1);
    CHECK(nB / d == 2);
}

TEST(LinRegFilter, Sanity3)
{
    LinRegFilter<3, 0> filt;

    int32_t nA, nB, d;
    filt.update(0, 2, nA, nB, d);
    filt.update(1, 3, nA, nB, d);
    filt.update(2, 4, nA, nB, d);

    CHECK(nA / d == 1);
    CHECK(nB / d == 2);

    filt.update(3, 5, nA, nB, d);

    CHECK(nA / d == 1);
    CHECK(nB / d == 2);

    filt.update(4, 6, nA, nB, d);

    CHECK(nA / d == 1);
    CHECK(nB / d == 2);
}

TEST(LinRegFilter, WithNoise1)
{
    LinRegFilter<3, 0> filt;

    int32_t nA, nB, d;
    filt.update(0, 2 - 1, nA, nB, d);
    filt.update(1, 3 + 1, nA, nB, d);
    filt.update(2, 4 - 1, nA, nB, d);
    filt.update(3, 5, nA, nB, d);
    filt.update(4, 6 - 1, nA, nB, d);

    CHECK(nA / d == 1);
}

TEST(LinRegFilter, WithShift1)
{
    LinRegFilter<2, 5> filt;

    int32_t nA, nB, d;
    filt.update(0 * 1024, 2 * 1024, nA, nB, d);
    filt.update(1 * 1024, 3 * 1024, nA, nB, d);

    CHECK((nA * 1024) / d == 1 * 1024);
    CHECK((nB * 1024) / d == 2 * 1024);
}

TEST(LinRegFilter, WithShift2)
{
    LinRegFilter<3, 5> filt;

    int32_t nA, nB, d;
    filt.update(0 * 1024 / 2, 4 * 1024 / 2, nA, nB, d); // 0 -> 2
    filt.update(1 * 1024 / 2, 5 * 1024 / 2, nA, nB, d); // 0,5 -> 2,5
    filt.update(2 * 1024 / 2, 6 * 1024 / 2, nA, nB, d); // 1 -> 3

    CHECK((nA * 1024) / d == 1 * 1024);
    CHECK((nB * 1024) / d == 2 * 1024);
}

TEST(LinRegFilter, WithShift3)
{
    LinRegFilter<3, 5> filt;

    int32_t nA, nB, d;
    filt.update(0 * 1024 / 2, - 4 * 1024 / 4, nA, nB, d); // 0 -> -1
    filt.update(1 * 1024 / 2, - 5 * 1024 / 4, nA, nB, d); // 0,5 -> -1.25
    filt.update(2 * 1024 / 2, - 6 * 1024 / 4, nA, nB, d); // 1 -> -1.5

    CHECK(d / nA == - 2);
    CHECK((nB * 1024) / d == - 1024);
}
