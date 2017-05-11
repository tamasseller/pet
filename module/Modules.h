/*
 * Modules.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULES_H_
#define MODULES_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"
#include "Tag.h"

namespace pet {

class Modules {
        typedef LinkedList<ModuleInterface> InitList;

        static ModuleInterface* findProvider(const Tag* service);
        static bool recurse(InitList& initList, ModuleInterface* current);

    public:
        static bool initAll();
};

}

#endif /* MODULES_H_ */
