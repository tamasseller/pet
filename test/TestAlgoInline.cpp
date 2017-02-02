/*
 * Inline.cpp
 *
 *  Created on: 2016.11.23.
 *      Author: tamas.seller
 */

#include "data/LinkedList.h"

namespace {
	struct SimpleElement {
		SimpleElement* next;
		int integer;
		inline SimpleElement(int integer): next(0), integer(integer) {}
	};
}

extern "C" void noopt();

using namespace container;

extern "C" void iterate_with_iterator(LinkedList<SimpleElement> list) {
	for(LinkedList<SimpleElement>::Iterator it = list.iterator(); it.current(); it.step())
		noopt();
}

extern "C" void iterate_manually(SimpleElement* list) {
	for(SimpleElement* it = list; it; it = it->next)
		noopt();
}

extern "C" void find_with_iterator(LinkedList<SimpleElement> list)
{
	if(list.findByFields(0, &SimpleElement::integer))
		noopt();
}

extern "C" void find_manually(SimpleElement* list)
{
	for(SimpleElement* it = list; it; it = it->next)
		if(it->integer == 0)
			noopt();
}
