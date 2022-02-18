/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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
        friend LinkedPtrList<ElementBase*>;
        ElementBase *next;
    protected:
        ElementBase() {
            Registry::add(this);
        }
    };

private:
    typedef LinkedPtrList<ElementBase*> List;
    static List list;
public:
    template <class Child>
    class StaticElement: public ElementBase {
    protected:
        static Child instance;
    };

    static void add(ElementBase* item) {
        list.add(item);
    }

    static void clear() {
        list.clear();
    }

    static typename LinkedPtrList<ElementBase*>::Iterator iterator() {
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
