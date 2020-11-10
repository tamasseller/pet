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
#include "1test/Mock.h"

#include "module/Module.h"
#include "module/Modules.h"

#define MODULE(n, ...)                                     	\
struct n: public pet::Module<n, pet::Requires<__VA_ARGS__>> {                        \
    virtual void init() {                                   \
        instance.installModule();                           \
        MOCK(modules)::CALL(n);                             \
    }                                                       \
}

MODULE(M1);
MODULE(M2);
MODULE(M3);
MODULE(M4, M1, M2);
MODULE(M5, M2, M3);
MODULE(M6, M1, M3);
MODULE(M7, M5);

TEST(Modules) {
    MOCK(modules)::EXPECT(M2);
    MOCK(modules)::EXPECT(M3);
    MOCK(modules)::EXPECT(M5);
    MOCK(modules)::EXPECT(M7);
    MOCK(modules)::EXPECT(M1);
    MOCK(modules)::EXPECT(M6);
    MOCK(modules)::EXPECT(M4);
    CHECK(pet::Modules::init());
}

