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

#include "ubiquitous/Delegate.h"

static_assert(sizeof(pet::Delegate<void()>) == 2 * sizeof(void*));
static_assert(sizeof(pet::Delegate<void()>) == sizeof(pet::Delegate<int()>));
static_assert(sizeof(pet::Delegate<void()>) == sizeof(pet::Delegate<int(int)>));
static_assert(sizeof(pet::Delegate<void()>) == sizeof(pet::Delegate<void(int)>));

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

    static inline auto applyByRef(int &x, pet::Delegate<int(int&)> f) {
        return f(x);
    }

    static inline auto applyByRvalRef(int &&x, pet::Delegate<int(int&&)> f) {
        return f(pet::move(x));
    }

    struct MovableTestTarget
    {
        bool live = true;
        const uint8_t gen = 0;
        const uint8_t id = 0;
        MovableTestTarget() = default;
        MovableTestTarget(int id): id(id) {}
        MovableTestTarget(MovableTestTarget &&o): live(o.live), gen(o.gen + 1), id(o.id) { o.live = false; }
        MovableTestTarget(const MovableTestTarget&) = delete;
        ~MovableTestTarget() {
            MOCK(TestTarget)::CALL(Dtor).withParam(live).withParam(gen);
        }
    };
};

TEST(Delegate, Sanity)
{
    auto f = pet::delegate([this](){return p1.f();});
    auto g = pet::delegate([this](int x) {p1.g(x); });

    MOCK(P)::EXPECT(f);
    MOCK(P)::EXPECT(g).withParam(123);

    g(f());
}

TEST(Delegate, DifferentInstances)
{
    auto f1 = pet::delegate([this](){return p1.f();});
    auto f2 = pet::delegate([this](){return p1.f();});
    auto g1 = pet::delegate([this](int x){p1.g(x);});
    auto g2 = pet::delegate([this](int x){p1.g(x);});
    auto h1 = pet::delegate([this](P& p, int y){p1.h(p, y);});
    auto h2 = pet::delegate([this](P& p, int y){p1.h(p, y);});

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
    auto a = pet::delegate([](int a, int b){return add(a, b);});

    MOCK(S)::EXPECT(add).withParam(1).withParam(2);
    CHECK(3 == a(1, 2));
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

    int x = 1;
    CHECK(applyByRef(x, [](int &x){ return x *= 2; }) == 2);
    CHECK(x == 2);
    CHECK(applyByRef(x, [](int &x){ return x *= 3; }) == 6);
    CHECK(x == 6);

    int y = 10;
    CHECK(applyByRvalRef(pet::move(y), [](int &&x){ return x = 0; }) == 0);
    CHECK(y == 0);
}

TEST(Delegate, ConstInstance)
{
    const auto* c = &p1;
    CHECK(c->c() == -123);

    auto d = pet::delegate([c](){return c->c();});
    CHECK(d() == -123);
}

TEST(Delegate, Empty)
{
    pet::Delegate<void()> empty;
    empty.clear();
}

TEST(Delegate, CtorDtorMoveOnlyTarget)
{
    MovableTestTarget t;

    {
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(1); // original lambda object
        auto d = pet::delegate([u{pet::move(t)}](){return u.live;});

        CHECK(!t.live);
        CHECK(d());
        MOCK(TestTarget)::EXPECT(Dtor).withParam(true).withParam(2); // lambda object captured inside delegate
    }

    MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(0); // local variable t
}

TEST(Delegate, DelegateMoving)
{
    MovableTestTarget t;

    {
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(1); // original lambda object
        auto d = pet::delegate([u{pet::move(t)}](){return u.live;});
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(2); // lambda object captured inside d
        auto e = pet::move(d);
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(3); // lambda object captured inside e
        auto f(pet::move(e));

        CHECK(!t.live);
        CHECK(f());
        MOCK(TestTarget)::EXPECT(Dtor).withParam(true).withParam(4); // lambda object captured inside f
    }

    MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(0); // local variable t
}

TEST(Delegate, DelegateOverwrite)
{
    MovableTestTarget t1(1), t2(2);

    {
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(1); // original lambda object
        auto d = pet::delegate([u{pet::move(t1)}](){return u.id;});
        MOCK(TestTarget)::EXPECT(Dtor).withParam(true).withParam(2); // overwritten lambda object
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(2); // new temporary lambda object
        MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(1); // original lambda object
        d = pet::delegate([u{pet::move(t2)}](){return u.id;});

        CHECK(d() == 2);
        MOCK(TestTarget)::EXPECT(Dtor).withParam(true).withParam(3); // lambda object captured inside d
    }

    MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(0); // local variable t2
    MOCK(TestTarget)::EXPECT(Dtor).withParam(false).withParam(0); // local variable t1
}

TEST(Delegate, DelegateArg)
{
    auto d = pet::delegate([](pet::Delegate<int(int)> f){return f(3);});
    CHECK(9 == d([](int x){ return x * x;}));

    auto e = pet::delegate([](int x){ return x + x ;});
    CHECK(6 == d(pet::move(e)));
}

TEST(Delegate, EmptyOverwrite)
{
	pet::Delegate<void()> a;
	a = pet::Delegate<void()>();
}

TEST(Delegate, EmptyMoveCtor)
{
	struct S{
		static void f(pet::Delegate<void()> a) {}
	};

	pet::Delegate<void()> a;
	S::f(pet::move(a));
}
