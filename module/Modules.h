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

#ifndef MODULES_H_
#define MODULES_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"

namespace pet {

class Modules {
        typedef LinkedPtrList<ModuleInterface*> InitList;

        static inline bool recurse(InitList& initList, ModuleInterface* current);
    public:
        static inline bool init();
};


inline bool Modules::recurse(InitList& initList, ModuleInterface* current)
{
    if(current->locked)
        return false;

    if(current->visited)
        return true;

    current->locked = true;

    ModuleInterface* const * dependencies;
    int n = current->requires(dependencies);

    for(int i = 0; i < n; i++) {
        if(!recurse(initList, dependencies[i]))
            return false;
    }

    current->visited = true;
    initList.addBack(current);

    current->locked = false;
    return true;
}

inline bool Modules::init()
{
    InitList initList;

    for(auto it = Registry<ModuleInterface>::iterator(); it.current(); it.step()) {
        if(!recurse(initList, it.current()))
            return false;
    }

    for(auto it = initList.iterator(); it.current(); it.step()) {
        it.current()->init();
    }

    return true;
}

}

#endif /* MODULES_H_ */
