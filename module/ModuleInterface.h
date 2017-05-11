/*
 * ModuleInterface.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULEINTERFACE_H_
#define MODULEINTERFACE_H_

namespace pet {

#include "Tag.h"

class ModuleInterface {
    friend class Modules;
    virtual void init() = 0;

    friend class LinkedList<ModuleInterface>;
    ModuleInterface *next = 0;
    bool visited = false, locked = false;

    virtual unsigned int requires(const Tag* const * &) = 0;
    virtual unsigned int provides(const Tag* const * &) = 0;
public:
    inline virtual ~ModuleInterface() {}
};

}

#endif /* MODULEINTERFACE_H_ */
