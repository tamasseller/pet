/*
 * Modules.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULES_H_
#define MODULES_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"

namespace pet {

class Modules {
        typedef LinkedList<ModuleInterface> InitList;

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
