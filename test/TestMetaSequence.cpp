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

#include "meta/Sequence.h"

using namespace pet;

TEST_GROUP(Sequence) {
    template<int... idx> void test(int min, int max, const Sequence<idx...>&) {
        int arr[] = {idx...};

        CHECK((sizeof(arr)/sizeof(arr[0])) == (max - min));

        for(int i = 0; i < max - min; i++)
            arr[i] = min + i;
    }
};

TEST(Sequence, Sanity) {
    test(0, 5, sequence<0, 5>());
    test(5, 10, sequence<5, 10>());
}

TEST(Sequence, Single) {
    test(10, 11, sequence<10, 11>());
}

TEST(Sequence, Empty) {
    test(11, 11, sequence<11, 11>());
}

