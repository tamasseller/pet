/*******************************************************************************
 *
 * Copyright (c) 2017 Seller Tamás. All rights reserved.
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

#ifndef ORDEREDDOUBLELIST_H_
#define ORDEREDDOUBLELIST_H_

#include "DoubleList.h"

namespace pet {

namespace OrderedDoubleListDetail {
	template<class Element>
	struct ComparatorHelper {
		static inline bool compare(const Element& small, const Element& big) {
			return small < big;
		}
	};
}

template<class Element, bool (*compare)(const Element&, const Element&) =
		OrderedDoubleListDetail::ComparatorHelper<Element>::compare >
class OrderedDoubleList: protected DoubleList<Element> {
public:
	using Iterator = typename DoubleList<Element>::Iterator;

	inline Iterator iterator();
	inline void add(Element* elem);
	inline void addLowest(Element* elem);
	inline void addHighest(Element* elem);
	inline Element* lowest();
	inline Element* highest();
	inline Element* popLowest();
	inline Element* popHighest();

    using OrderedDoubleList::DoubleList::remove;
	using DoubleList<Element>::contains;
};

template<class Element, bool (*compare)(const Element&, const Element&)>
void OrderedDoubleList<Element, compare>::add(Element* elem) {
	typename DoubleList<Element>::Iterator it = this->iterator();

	if(!highest() || !compare(*elem, *highest()))
		addHighest(elem);
	else {
		while(it.current() && !compare(*elem, *it.current()))
			it.step();

		this->insertBefore(elem, it);
	}
}

template<class Element, bool (*compare)(const Element&, const Element&)>
void OrderedDoubleList<Element, compare>::addLowest(Element* elem) {
	this->fastAddFront(elem);
}

template<class Element, bool (*compare)(const Element&, const Element&)>
void OrderedDoubleList<Element, compare>::addHighest(Element* elem) {
	this->fastAddBack(elem);
}

template<class Element, bool (*compare)(const Element&, const Element&)>
Element* OrderedDoubleList<Element, compare>::lowest() {
	return this->front();
}

template<class Element, bool (*compare)(const Element&, const Element&)>
Element* OrderedDoubleList<Element, compare>::highest() {
	return this->back();
}

template<class Element, bool (*compare)(const Element&, const Element&)>
Element* OrderedDoubleList<Element, compare>::popLowest() {
	return this->popFront();
}

template<class Element, bool (*compare)(const Element&, const Element&)>
Element* OrderedDoubleList<Element, compare>::popHighest() {
	return this->popBack();
}

template<class Element, bool (*compare)(const Element&, const Element&)>
inline typename OrderedDoubleList<Element, compare>::Iterator
OrderedDoubleList<Element, compare>::iterator() {
	return this->DoubleList<Element>::iterator();
}

}


#endif /* ORDEREDDOUBLELIST_H_ */
