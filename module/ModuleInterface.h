/*
 * ModuleInterface.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULEINTERFACE_H_
#define MODULEINTERFACE_H_

namespace pet {

class ModuleInterface {
    bool visited = false;
	bool locked = false;

    friend class LinkedList<ModuleInterface>;
    ModuleInterface *next = nullptr;

    friend class Modules;
    virtual void init() = 0;
    virtual unsigned int requires(ModuleInterface* const * &) = 0;
public:
    inline virtual ~ModuleInterface() {}
};

}

#endif /* MODULEINTERFACE_H_ */
