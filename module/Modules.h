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

        static bool recurse(InitList& initList, ModuleInterface* current);
    public:
        static bool init();
};

}

#endif /* MODULES_H_ */
