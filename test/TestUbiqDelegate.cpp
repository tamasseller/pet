/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#include "ubiquitous/Delegate.h"

TEST_GROUP(Delegate)
{
    struct P
    {
        int state = 123;

        int f()
        {
            MOCK(P)::CALL(f);
            return state;
        }

        void g(int x)
        {
            MOCK(P)::CALL(g).withParam(x);
        }

        void h(P& p, int y)
        {
            MOCK(P)::CALL(h).withParam(&p).withParam(y);
            state = p.state + y;
        }

        int c() const
        {
            return -state;
        }
    } p1, p2;

    static inline auto add(int a, int b) {
        MOCK(S)::CALL(add).withParam(a).withParam(b);
        return a + b;
    }

    static inline auto apply(int x, pet::Delegate<int(int)> f) {
        return f(x);
    }
};

TEST(Delegate, Sanity)
{
    auto f = pet::delegate<&P::f>(&p1);
    auto g = pet::delegate<&P::g>(&p1);

    MOCK(P)::EXPECT(f);
    MOCK(P)::EXPECT(g).withParam(123);

    g(f());
}

TEST(Delegate, DifferentInstances)
{
    auto f1 = pet::delegate<&P::f>(&p1);
    auto f2 = pet::delegate<&P::f>(&p2);
    auto g1 = pet::delegate<&P::g>(&p1);
    auto g2 = pet::delegate<&P::g>(&p2);
    auto h1 = pet::delegate<&P::h>(&p1);
    auto h2 = pet::delegate<&P::h>(&p2);

    MOCK(P)::EXPECT(h).withParam(&p2).withParam(321);
    MOCK(P)::EXPECT(f);
    MOCK(P)::EXPECT(g).withParam(444);
    MOCK(P)::EXPECT(h).withParam(&p1).withParam(321);
    MOCK(P)::EXPECT(f);
    MOCK(P)::EXPECT(g).withParam(765);

    h1(p2, 321);
    g1(f1());
    h2(p1, 321);
    g2(f2());
}

TEST(Delegate, StaticFunction)
{
    auto a = pet::delegate<&add>();

    MOCK(S)::EXPECT(add).withParam(1).withParam(2);
    a(1, 2);
}

TEST(Delegate, FreeStandingLambda)
{
    auto l = pet::delegate([](int x, char c, void* v){
        MOCK(L)::CALL(op).withParam(x).withParam(c).withParam(v);
    });

    MOCK(L)::EXPECT(op).withParam(1).withParam('d').withParam(&l);
    l(1, 'd', &l);
}

TEST(Delegate, MemberCallInLambda)
{
    struct Test
    {
        auto m(int x)
        {
            return x*x;
        }

        auto getTheD()
        {
            return pet::delegate([this](int x, int y){
                return m(x) + m(y);
            });
        }
    } t;

    auto d = t.getTheD();

    CHECK(d(2, 3) == 13);
}

TEST(Delegate, PassAsArgument)
{
    CHECK(apply(1, [](int x){ return 2 * x; }) == 2);
    CHECK(apply(2, [](int x){ return 3 * x; }) == 6);
}

TEST(Delegate, ConstInstance)
{
    const auto* c = &p1;
    CHECK(c->c() == -123);

    auto d = pet::delegate<&P::c>(c);
    CHECK(d() == -123);
}
