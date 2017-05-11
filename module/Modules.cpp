/*
 * Modules.cpp
 *
 *      Author: tamas.seller
 */

#include "Modules.h"

using namespace pet;

ModuleInterface* Modules::findProvider(const Tag* service)
{
    ModuleInterface* ret = nullptr;

    for(auto it = Registry<ModuleInterface>::iterator(); it.current(); it.step()) {
        const Tag* const * services;
        int n = it.current()->provides(services);

        for(int i = 0; i < n; i++) {
            if(services[i] == service) {
                if(ret)
                    return nullptr;
                ret = it.current();
            }
        }
    }

    return ret;
}

bool Modules::recurse(InitList& initList, ModuleInterface* current)
{
    if(current->locked)
        return false;

    if(current->visited)
        return true;

    current->locked = true;

    const Tag* const * dependencies;
    int n = current->requires(dependencies);

    for(int i = 0; i < n; i++) {
        ModuleInterface *target;
        if(!(target = findProvider(dependencies[i])))
            return false;

        if(!recurse(initList, current))
            return false;
    }

    current->visited = true;
    initList.add(current);

    current->locked = false;
    return true;
}

bool Modules::initAll()
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


