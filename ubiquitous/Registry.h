/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "data/LinkedList.h"

namespace pet {

template<class Interface>
class Registry {
public:
	class ElementBase: public Interface {
		friend LinkedList<ElementBase>;
		ElementBase *next;
	protected:
		ElementBase(): next(0) {}
	};

private:
	typedef LinkedList<ElementBase> List;
    static List list;
public:
	template <class Child>
	class StaticElement: public ElementBase {
	protected:
		StaticElement() {
			Registry::add(this);
		}

		static Child instance;
	};

	static void add(ElementBase* item) {
	    list.add(item);
	}

	static typename LinkedList<ElementBase>::Iterator iterator() {
		return list.iterator();
	}
};

template<class Interface>
typename Registry<Interface>::List Registry<Interface>::list;

template <class Interface>
template <class Child>
Child Registry<Interface>::StaticElement<Child>::instance;

}

#endif /* REGISTRY_H_ */
