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

#include "meta/Configuration.h"
#include "meta/TypeWrappers.h"

using namespace pet;

namespace {
    int oneDummy, otherDummy;

    class Class1;
    class Class2;

    template<class> struct Template1 {static constexpr int n = 1;};
    template<class> struct Template2 {static constexpr int n = 2;};
    template<class> struct Template3 {static constexpr int n = 3;};
	template<class, class> struct IrregularTemplate {static constexpr const char* other = "4";};
	template<class X> struct Unwrap{static constexpr auto value = X::value;};
	template<class X, class Y> struct Adder{static constexpr auto value = X() + Y();};
	template<class X> struct Negate{static constexpr auto value = !X();};


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

    template<template<class...> class Template>
    struct X1: public ConfigTemplate<X1, Template> {};

    template<template<class...> class Template>
    struct X2: public ConfigTemplate<X2, Template> {};

    template<template<class...> class Template>
    struct X3: public ConfigTemplate<X3, Template> {};

    template<class... Args>
    struct Configurable {
            static constexpr auto v1 = V1<1>::extract<Args...>::value;
            static constexpr auto v2 = V2<'2'>::extract<Args...>::value;
            static constexpr auto v3 = V3<&oneDummy>::extract<Args...>::value;
            using t1 = typename T1<void>::extract<Args...>::type;
            using t2 = typename T2<char>::extract<Args...>::type;
            using t3 = typename T3<Class1>::extract<Args...>::type;

            template<class... X>
            using x1 = typename X1<Template1>::extract<Args...>::template typeTemplate<X...>;

            template<class... X>
            using x2 = typename X2<Template2>::extract<Args...>::template typeTemplate<X...>;

            template<class... X>
            using x3 = typename X3<Template3>::extract<Args...>::template typeTemplate<X...>;
    };

    template<class A, class B> struct TypeEquals {
        static constexpr bool value = false;
    };

    template<class A> struct TypeEquals<A, A> {
        static constexpr bool value = true;
    };

    template<template<class...> class A, template<class...> class B> struct TemplateEquals {
        static constexpr bool value = false;
    };

    template<template<class...> class A> struct TemplateEquals<A, A> {
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
    typedef Configurable<X1<Template2>> C4;
    CHECK(C1::v1 == 1);
    CHECK(C2::v1 == -1);
    CHECK(TypeEquals<C1::t1, void>::value);
    CHECK(TypeEquals<C3::t1, int>::value);
    CHECK(C1::x1<int>::n == 1);
    CHECK(C4::x1<int>::n == 2);

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

    CHECK(TypeEquals<C1::t1, int>::value);
    CHECK(TypeEquals<C2::t1, long long int>::value);
    CHECK(TypeEquals<C3::t1, void>::value);

    CHECK(TypeEquals<C1::t2, Class2>::value);
    CHECK(TypeEquals<C2::t2, char>::value);
    CHECK(TypeEquals<C3::t2, double>::value);

    CHECK(TypeEquals<C1::t3, Class1>::value);
    CHECK(TypeEquals<C2::t3, const volatile char*>::value);
    CHECK(TypeEquals<C3::t3, void*>::value);
}

TEST(MetaConfiguration, MultiTemplate) {
    typedef Configurable<X1<Template2>, X2<Template3>> C1;
    typedef Configurable<X3<Template1>, X1<Template3>> C2;
    typedef Configurable<X2<Template1>, X3<Template2>> C3;

    CHECK(C1::x1<int>::n == 2);
    CHECK(C2::x1<int>::n == 3);
    CHECK(C3::x1<int>::n == 1);

    CHECK(C1::x2<int>::n == 3);
    CHECK(C2::x2<int>::n == 2);
    CHECK(C3::x2<int>::n == 1);

    CHECK(C1::x3<int>::n == 3);
    CHECK(C2::x3<int>::n == 1);
    CHECK(C3::x3<int>::n == 2);
}

TEST(MetaConfiguration, TemplateAdditional) {
	const char *c = Configurable<X1<IrregularTemplate>>::x1<int, void>::other;
    CHECK(*c == '4');

    CHECK(Configurable<X1<Unwrap>>::x1<Value<int, 42>>::value == 42);

    CHECK(Configurable<X1<Adder>>::x1<Value<int, 2>, Value<int, 3>>::value == 5);

    CHECK(Configurable<X1<Negate>>::x1<True>::value == false);
    CHECK(Configurable<X1<Negate>>::x1<False>::value == true);
}
