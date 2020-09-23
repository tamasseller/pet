#include "data/ExtremeSetFilter.h"
#include "1test/Test.h"
using namespace pet;


TEST_GROUP(ExtremeSetFilter)
{
	static inline bool comp(const int &a, const int &b) {
		return a < b;
	}

	pet::ExtremeSetFilter<int, 5, &comp> uut;

	void pop(int exp)
	{
		CHECK(!uut.isEmpty());
		CHECK(uut.extreme() == exp);

		int v;
		CHECK(uut.pop(v));
		CHECK(v == exp);
	}

	void popNothing()
	{
		int v;
		CHECK(uut.isEmpty());
		CHECK(!uut.pop(v));
	}
};

TEST(ExtremeSetFilter, EmptyIsEmpty)
{
	popNothing();
}

TEST(ExtremeSetFilter, NotFullSingleRun)
{
	uut.insert(1);
	uut.insert(3);
	uut.insert(2);
	uut.sort();
	pop(1);
	pop(2);
	pop(3);
	popNothing();
}

TEST(ExtremeSetFilter, OverFillSingleRunIncreasing)
{
	for(int i = 0; i < 10; i++)
		uut.insert(pet::move(i));

	uut.sort();

	for(int i = 0; i < 5; i++)
		pop(i);

	popNothing();
}

TEST(ExtremeSetFilter, OverFillSingleRunDecreasing)
{
	for(int i = 9; i >= 0; i--)
		uut.insert(pet::move(i));

	uut.sort();

	for(int i = 0; i < 5; i++)
		pop(i);

	popNothing();
}

TEST(ExtremeSetFilter, FillSortPopAddAgainStory)
{
	for(int i = 20; i > 10; i--)
	{
		uut.insert(pet::move(i));
	}

	uut.sort();
	pop(11);
	pop(12);

	uut.insert(21);
	uut.insert(1);

	uut.sort();
	pop(1);
	pop(13);

	uut.insert(22);
	uut.insert(2);
	uut.insert(100);

	uut.sort();
	pop(2);
	pop(14);

	uut.insert(23);
	uut.insert(3);
	uut.insert(200);

	uut.sort();
	pop(3);
	pop(15);
	pop(21);
	pop(22);
	pop(23);
	popNothing();
}

