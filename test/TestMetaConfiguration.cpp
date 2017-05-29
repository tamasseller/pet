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

#include "meta/Configuration.h"

using namespace pet;

namespace {
    int oneDummy, otherDummy;

    class Class1;
    class Class2;

    template<int x>
    struct V1: public ConfigValue<int, V1, x> {};

    template<char x>
    struct V2: public ConfigValue<char, V2, x> {};

    template<int* x>
    struct V3: public ConfigValue<int*, V3, x> {};

    template<class Type>
    struct T1: public ConfigType<T1, Type> {};

    template<class Type>
    struct T2: public ConfigType<T2, Type> {};

    template<class Type>
    struct T3: public ConfigType<T3, Type> {};

    template<class... Args>
    struct Configurable {
            static constexpr auto v1 = V1<1>::extract<Args...>::value;
            static constexpr auto v2 = V2<'2'>::extract<Args...>::value;
            static constexpr auto v3 = V3<&oneDummy>::extract<Args...>::value;
            using t1 = typename T1<void>::extract<Args...>::type;
            using t2 = typename T2<char>::extract<Args...>::type;
            using t3 = typename T3<Class1>::extract<Args...>::type;
    };

    template<class A, class B> struct TypeEquals {
        static constexpr bool value = false;
    };

    template<class A> struct TypeEquals<A, A> {
        static constexpr bool value = true;
    };
}

TEST_GROUP(MetaConfiguration){};

TEST(MetaConfiguration, MetaMetaLol) {
    bool x = TypeEquals<int, void>::value;
    CHECK(!x);

    bool y = TypeEquals<void, void>::value;
    CHECK(y);

    struct A;
    struct B;

    bool z = TypeEquals<A, A>::value;
    CHECK(z);

    bool w = TypeEquals<A, B>::value;
    CHECK(!w);
}

TEST(MetaConfiguration, Sanity) {
    typedef Configurable<> C1;
    typedef Configurable<V1<-1>> C2;
    typedef Configurable<T1<int>> C3;
    CHECK(C1::v1 == 1);
    CHECK(C2::v1 == -1);
    CHECK((TypeEquals<C1::t1, void>::value));
    CHECK((TypeEquals<C3::t1, int>::value));
}

TEST(MetaConfiguration, MultiValue) {

    typedef Configurable<V1<10>, V2<'x'>> C1;
    typedef Configurable<V3<&otherDummy>, V1<10>> C2;
    typedef Configurable<V2<'x'>, V3<&otherDummy>> C3;

    CHECK(C1::v1 == 10);
    CHECK(C2::v1 == 10);
    CHECK(C3::v1 == 1);

    CHECK(C1::v2 == 'x');
    CHECK(C2::v2 == '2');
    CHECK(C3::v2 == 'x');

    CHECK(C1::v3 == &oneDummy);
    CHECK(C2::v3 == &otherDummy);
    CHECK(C3::v3 == &otherDummy);
}


TEST(MetaConfiguration, MultiType) {
    typedef Configurable<T1<int>, T2<Class2>> C1;
    typedef Configurable<T3<const volatile char*>, T1<long long int>> C2;
    typedef Configurable<T2<double>, T3<void*>> C3;

    CHECK((TypeEquals<C1::t1, int>::value));
    CHECK((TypeEquals<C2::t1, long long int>::value));
    CHECK((TypeEquals<C3::t1, void>::value));

    CHECK((TypeEquals<C1::t2, Class2>::value));
    CHECK((TypeEquals<C2::t2, char>::value));
    CHECK((TypeEquals<C3::t2, double>::value));

    CHECK((TypeEquals<C1::t3, Class1>::value));
    CHECK((TypeEquals<C2::t3, const volatile char*>::value));
    CHECK((TypeEquals<C3::t3, void*>::value));
}
