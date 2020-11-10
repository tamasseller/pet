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

#include "data/Union.h"

using namespace pet;

struct T {
    static int n;
    T() {n++;}
    T(const T&) {n++;}
    ~T() {n--;}
};

struct U {
    static int n;
    U() {n++;}
    U(const U&) {n++;}
    ~U() {n--;}
};

struct V {
    static int n;
    V() {n++;}
    V(const V&) {n++;}
    ~V() {n--;}
};

int T::n=0, U::n=0, V::n=0;

inline void* operator new(size_t s, void* self){ return self; }

TEST_GROUP(Union) {};

TEST(Union, Simple) {
    Union<int, const short, char> u('c');

    CHECK(*u.accessAs<char>() == 'c');
    CHECK(u.accessAs<const short>() == nullptr);

    u.initAs<int>(1);
    CHECK(*u.accessAs<int>() == 1);
    CHECK(u.accessAs<char>() == nullptr);

    u.initAs<const short>(2);
    const short *x = u.accessAs<const short>();
    CHECK(*x == 2);
    CHECK(u.accessAs<int>() == nullptr);
}

TEST(Union, UnionAlignment) {
    struct alignas(4) A4 {};
    struct alignas(8) A8 {};
    struct alignas(16) A16 {};

    CHECK(alignof(Union<A4>) == 4);
    CHECK(alignof(Union<A8>) == 8);
    CHECK(alignof(Union<A8, A16>) == 16);
    CHECK(alignof(Union<A16, A4, A8>) == 16);
};

TEST(Union, ConstUnion) {
    const Union<int, char> u(123);

    CHECK(*u.accessAs<int>() == 123);
    CHECK(u.accessAs<char>() == nullptr);
}


TEST(Union, UnionCtorDtorSameType) {
    struct P { int c = 0; int i = 0; } p;

    struct C {
        int n;
        P* p;
        C(int n, P* p): n(n), p(p) {p->c += n; p->i++;}
        C(const C& other): n(other.n), p(other.p) {p->c += n; p->i++;}
        ~C() {if(p) p->c -= n;}
    };

    Union<C, int> u(C(3, &p));

    CHECK(p.c == 3);
    CHECK(p.i == 2);

    u.initAs<C>(2, &p);

    CHECK(p.c == 2);
    CHECK(p.i == 3);

    u.initAs<int>(42);

    CHECK(p.c == 0);
    CHECK(p.i == 3);

    C c(1, &p);

    CHECK(p.c == 1);
    CHECK(p.i == 4);

    u.initAs<C>(c);

    CHECK(p.c == 2);
    CHECK(p.i == 5);
}

TEST(Union, UnionCtorDtorDifferentType)
{
    Union<T, U, V> u;

    CHECK((T::n == 0) && (U::n == 0) && (V::n == 0));
    u.initAs<T>();
    CHECK((T::n == 1) && (U::n == 0) && (V::n == 0));
    u.initAs<U>();
    CHECK((T::n == 0) && (U::n == 1) && (V::n == 0));
    u.initAs<V>();
    CHECK((T::n == 0) && (U::n == 0) && (V::n == 1));
}


TEST(Union, BaseAccess)
{
    struct Base1 {int x;};
    struct Base2 {int y;};
    struct C1: Base1 {};
    struct C2: Base2, Base1 {};

    Union<C1, C2> u;

    u.initAs<C1>();
    u.accessAs<C1>()->x = 1;
    CHECK(u.accessAsBase<Base1>()->x == 1);
    u.accessAsBase<Base1>()->x = 2;
    CHECK(u.accessAs<C1>()->x == 2);

    u.initAs<C2>();

    u.accessAs<C2>()->x = 1;
    u.accessAs<C2>()->y = 2;
    CHECK(u.accessAsBase<Base1>()->x == 1);

    // Should produce error.
    // CHECK(u.accessAsBase<Base2>().y == 2);

    u.accessAsBase<Base1>()->x = 3;

    // Should produce error.
    //u.accessAsBase<Base2>().y = 4;

    CHECK(u.accessAs<C2>()->x == 3);
    CHECK(u.accessAs<C2>()->y == 2);
}

TEST(Union, ConstBaseAccess)
{
    struct Base1 {int x;};
    struct Base2 {int y;};
    struct C1: Base1 {};
    struct C2: Base2, Base1 {C2(int x, int y) {this->x = x; this->y = y;}};

    const Union<C1, C2> u(C2(1, 2));

    CHECK(u.accessAs<C2>()->x == 1);
    CHECK(u.accessAs<C2>()->y == 2);
    CHECK(u.accessAsBase<Base1>()->x == 1);
}

