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

#include "managed/RefCnt.h"
#include "MockAllocator.h"

#include "1test/Test.h"

TEST_GROUP(RefCnt)
{
    struct Target: pet::RefCnt<Target, Allocator>
    {
        inline Target(int x, char y) {
            MOCK(Target)::CALL(Ctor).withParam(x).withParam(y);
        }

        inline void f()
        {
            MOCK(Target)::CALL(f);
        }

        inline virtual ~Target() {
            MOCK(Target)::CALL(Dtor);
        }
    };

    struct FirstSuperclass
    {
    	inline virtual ~FirstSuperclass() {
    		MOCK(Target)::CALL(FirstSuperclassDtor);
    	}
    };

    struct NonFirstSuperClass: pet::RefCnt<NonFirstSuperClass, Allocator> {
    	inline virtual ~NonFirstSuperClass() = default;
    };

    struct SubTarget: FirstSuperclass, NonFirstSuperClass
    {
    	SubTarget::Ptr<SubTarget> strg;
        inline SubTarget()
        {
        	MOCK(Target)::CALL(SubTargetCtor);
        }

        inline virtual ~SubTarget() = default;
    };

    static_assert(sizeof(Target) == sizeof(void*) + sizeof(void*));
    static_assert(sizeof(Target::Ptr<Target>) == sizeof(void*));

    struct MockTracer: Allocator::ReferenceTracer
    {
		inline virtual void acquistion(void* refLoc, void* trg) override {
			MOCK(RefTrace)::CALL(acquire);
		}

		inline virtual void release(void* refLoc, void* trg) override {
			MOCK(RefTrace)::CALL(release).withParam(trg);
		}
    };

    static inline MockTracer tracer;
};

TEST(RefCnt, Sanity)
{
	Allocator::tracer = &tracer;

    {
        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        MOCK(RefTrace)::EXPECT(acquire);
        auto ptr = Target::make(1, 'a');

        CHECK(true == (bool)ptr);
        CHECK(false == !ptr);
        CHECK(false == (ptr == nullptr));
        CHECK(true == (ptr != nullptr));

        CHECK(Allocator::count == 1);

        MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, Empty)
{
	Allocator::tracer = &tracer;

	Target::Ptr<Target> ptr;
    CHECK(false == (bool)ptr);
    CHECK(true == !ptr);
    CHECK(true == (ptr == nullptr));
    CHECK(false == (ptr != nullptr));

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, CopyCtor)
{
    struct {
        inline void f(Target::Ptr<Target> q)
        {
            q->f();
            MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        }
    } s;

	Allocator::tracer = &tracer;

    {
	    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        MOCK(RefTrace)::EXPECT(acquire);
        auto ptr = Target::make(1, 'a');

        CHECK(Allocator::count == 1);

        MOCK(RefTrace)::EXPECT(acquire);
        MOCK(Target)::EXPECT(f);
        s.f(ptr);

        CHECK(Allocator::count == 1);

        MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, CopyEmpty)
{
	Allocator::tracer = &tracer;
    Target::Ptr<Target> p;
    Target::Ptr<Target> q(p);
    Allocator::tracer = nullptr;
}

TEST(RefCnt, CopyEq1)
{
	Allocator::tracer = &tracer;
    {
        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        MOCK(RefTrace)::EXPECT(acquire);
        auto q = Target::make(1, 'a');
        CHECK(Allocator::count == 1);

        {
            MOCK(Target)::EXPECT(Ctor).withParam(2).withParam('b');
            MOCK(RefTrace)::EXPECT(acquire);
            auto ptr = Target::make(2, 'b');
            CHECK(Allocator::count == 2);

            MOCK(RefTrace)::EXPECT(release).withParam(q.get());
            MOCK(RefTrace)::EXPECT(acquire);
            MOCK(Target)::EXPECT(Dtor);
            CHECK(q != ptr);
            q = ptr;
            CHECK(q == ptr);
            CHECK(Allocator::count == 1);

            MOCK(Target)::EXPECT(f);
            ptr->f();
            MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
        }

        CHECK(Allocator::count == 1);

        MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, CopyEq2)
{
	Allocator::tracer = &tracer;
    {
		Target::Ptr<Target> q;
        CHECK(Allocator::count == 0);

        {
            MOCK(Target)::EXPECT(Ctor).withParam(3).withParam('c');
            MOCK(RefTrace)::EXPECT(acquire);
            auto ptr = Target::make(3, 'c');
            CHECK(Allocator::count == 1);

            MOCK(RefTrace)::EXPECT(acquire);
            CHECK(q != ptr);
            q = ptr;
            CHECK(q == ptr);
            CHECK(Allocator::count == 1);

            MOCK(Target)::EXPECT(f);
            ptr->f();
            MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
        }

        CHECK(Allocator::count == 1);
        MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}


TEST(RefCnt, CopyEq3)
{
	Allocator::tracer = &tracer;
    {
        MOCK(Target)::EXPECT(Ctor).withParam(4).withParam('d');
        MOCK(RefTrace)::EXPECT(acquire);
        auto q = Target::make(4, 'd');
        CHECK(Allocator::count == 1);

        {
            Target::Ptr<Target> ptr;
            CHECK(Allocator::count == 1);

            MOCK(RefTrace)::EXPECT(release).withParam(q.get());
            MOCK(Target)::EXPECT(Dtor);
            CHECK(q != ptr);
            q = ptr;
            CHECK(q == ptr);
            CHECK(Allocator::count == 0);
            CHECK(!q);
        }
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, CopyEq4)
{
	Allocator::tracer = &tracer;
    {
        Target::Ptr<Target> q;

        {
            Target::Ptr<Target> ptr;
            CHECK(q == ptr);
            q = ptr;
            CHECK(q == ptr);
        }
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, MoveCtor1)
{
	Allocator::tracer = &tracer;
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
    MOCK(RefTrace)::EXPECT(acquire);
    Target::Ptr<Target> ptr = Target::make(1, 'a');
    CHECK(Allocator::count == 1);

    {
        MOCK(RefTrace)::EXPECT(acquire);
        MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
        Target::Ptr<Target> q(pet::move(ptr));
        CHECK(q != ptr);

        MOCK(Target)::EXPECT(f);
        (*q).f();
        CHECK(Allocator::count == 1);
        MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, MoveCtor2)
{
	Allocator::tracer = &tracer;
    Target::Ptr<Target> ptr;
    CHECK(Allocator::count == 0);

    {
        Target::Ptr<Target> q(pet::move(ptr));
        CHECK(q == ptr);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, MoveEq1)
{
	Allocator::tracer = &tracer;
    {
        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        MOCK(RefTrace)::EXPECT(acquire);
        auto q = Target::make(1, 'a');
        CHECK(Allocator::count == 1);

        {
            MOCK(Target)::EXPECT(Ctor).withParam(2).withParam('b');
            MOCK(RefTrace)::EXPECT(acquire);
            auto ptr = Target::make(2, 'b');
            CHECK(Allocator::count == 2);

            MOCK(RefTrace)::EXPECT(release).withParam(q.get());
            MOCK(RefTrace)::EXPECT(acquire);
            MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
            MOCK(Target)::EXPECT(Dtor);
            q = pet::move(ptr);
            CHECK(Allocator::count == 1);
            CHECK(!ptr);

            MOCK(Target)::EXPECT(f);
            q->f();
        }

        CHECK(Allocator::count == 1);

        MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, MoveEq2)
{
	Allocator::tracer = &tracer;
    {
        Target::Ptr<Target> q;
        CHECK(Allocator::count == 0);

        {
            MOCK(Target)::EXPECT(Ctor).withParam(3).withParam('c');
            MOCK(RefTrace)::EXPECT(acquire);
            auto ptr = Target::make(3, 'c');
            CHECK(Allocator::count == 1);

            MOCK(RefTrace)::EXPECT(acquire);
            MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
            q = pet::move(ptr);
            CHECK(Allocator::count == 1);
            CHECK(!ptr);

            MOCK(Target)::EXPECT(f);
            q->f();
        }

        CHECK(Allocator::count == 1);
        MOCK(RefTrace)::EXPECT(release).withParam(q.get());
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, MoveEq3)
{
	Allocator::tracer = &tracer;
    {
        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        MOCK(RefTrace)::EXPECT(acquire);
        auto q = Target::make(1, 'a');
        CHECK(Allocator::count == 1);

        {
            Target::Ptr<Target> ptr;
            CHECK(Allocator::count == 1);

            MOCK(RefTrace)::EXPECT(release).withParam(q.get());
            MOCK(Target)::EXPECT(Dtor);
            q = pet::move(ptr);

            CHECK(Allocator::count == 0);
        }
    }

    CHECK(Allocator::allFreed());
    Allocator::tracer = nullptr;
}

TEST(RefCnt, Access)
{
	Allocator::tracer = &tracer;

    Target::Ptr<Target> ptr;

    {
		MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = Target::make(1, 'a');

		MOCK(RefTrace)::EXPECT(acquire);
		MOCK(RefTrace)::EXPECT(acquire);
		MOCK(RefTrace)::EXPECT(release).withParam(a.get());
		ptr = a->self();

		MOCK(RefTrace)::EXPECT(release).withParam(a.get());
    }

    MOCK(Target)::EXPECT(f);
    ptr->f();

    MOCK(Target)::EXPECT(f);
    (*ptr).f();

    MOCK(Target)::EXPECT(f);
    ptr.get()->f();

    MOCK(Target)::EXPECT(Dtor);
    Allocator::tracer = nullptr;
}

TEST(RefCnt, Reset)
{
	Allocator::tracer = &tracer;

    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');

	MOCK(RefTrace)::EXPECT(acquire);
    auto ptr = Target::make(1, 'a');
	MOCK(RefTrace)::EXPECT(acquire);
    auto copy = ptr;

    CHECK(ptr);
	MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
    ptr = nullptr;
    CHECK(!ptr);
    ptr = nullptr;

    CHECK(copy);

	MOCK(RefTrace)::EXPECT(release).withParam(copy.get());
    MOCK(Target)::EXPECT(Dtor);
	copy = nullptr;

    CHECK(!copy);
    Allocator::tracer = nullptr;
}

TEST(RefCnt, Compare)
{
	Allocator::tracer = &tracer;
	{
		MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
		MOCK(RefTrace)::EXPECT(acquire);

		auto ptr = Target::make(1, 'a');
		Target::Ptr<Target> empty;

		CHECK(ptr);
		CHECK(!empty);

		CHECK(ptr != empty);
		CHECK(ptr == ptr);
		CHECK(empty == empty);
		CHECK(ptr != nullptr);
		CHECK(!(ptr == nullptr));
		CHECK(empty == nullptr);
		CHECK(!(empty != nullptr));

		MOCK(RefTrace)::EXPECT(release).withParam(ptr.get());
		MOCK(Target)::EXPECT(Dtor);
	}
    Allocator::tracer = nullptr;
}

TEST(RefCnt, SelfRefLoopClear)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	*strga = nullptr;

    Allocator::tracer = nullptr;
}

TEST(RefCnt, SelfRefLoopCopy)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	SubTarget::Ptr<SubTarget> empty;
	*strga = empty;

    Allocator::tracer = nullptr;
}

TEST(RefCnt, SelfRefLoopMove)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	SubTarget::Ptr<SubTarget> empty;
	*strga = pet::move(empty);

    Allocator::tracer = nullptr;
}

TEST(RefCnt, TwoObjectRingClear)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto b = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = b;

		MOCK(RefTrace)::EXPECT(acquire);
		b->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)b.get());
		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get()->strg.get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	*strga = nullptr;

    Allocator::tracer = nullptr;
}

TEST(RefCnt, TwoObjectRingCopy)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto b = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = b;

		MOCK(RefTrace)::EXPECT(acquire);
		b->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)b.get());
		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get()->strg.get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	SubTarget::Ptr<SubTarget> empty;
	*strga = empty;

    Allocator::tracer = nullptr;
}

TEST(RefCnt, TwoObjectRingMove)
{
	Allocator::tracer = &tracer;
	SubTarget::Ptr<SubTarget> *strga;

	{
		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto a = SubTarget::make<SubTarget>();

		MOCK(Target)::EXPECT(SubTargetCtor);
		MOCK(RefTrace)::EXPECT(acquire);
		auto b = SubTarget::make<SubTarget>();

		MOCK(RefTrace)::EXPECT(acquire);
		a->strg = b;

		MOCK(RefTrace)::EXPECT(acquire);
		b->strg = a;

		strga = &a->strg;

		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)b.get());
		MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)a.get());
	}

	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get());
	MOCK(RefTrace)::EXPECT(release).withParam((NonFirstSuperClass*)strga->get()->strg.get());
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	MOCK(Target)::EXPECT(FirstSuperclassDtor);
	SubTarget::Ptr<SubTarget> empty;
	*strga = pet::move(empty);

    Allocator::tracer = nullptr;
}
