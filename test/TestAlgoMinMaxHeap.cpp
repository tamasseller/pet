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

#include "algorithm/MinMaxHeap.h"

using namespace pet;

#include <array>
#include <algorithm>

TEST_GROUP(MinMaxHeap) {};

TEST(MinMaxHeap, HeapSort_Stress) {
    std::array<int, 10> data, check;

    srand(1);

    for(int n = 0; n < 10000; n++) {
        for(int i = 0; i<data.size(); i++)
            check[i] = data[i] = rand();

        std::sort(check.begin(), check.end());
        heapSort(&data.front(), data.size());
        CHECK(data == check);
    }
}

TEST(MinMaxHeap, FindMedianStress) {
    std::array<int, 11> data, check;

    srand(1);

    for(int n = 0; n < 10000; n++) {
        for(int i = 0; i<data.size(); i++)
            check[i] = data[i] = rand() % 100;

        std::sort(check.begin(), check.end());
        CHECK(findMedian(&data.front(), data.size()) == check[check.size()/2]);
    }
}
