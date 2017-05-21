/*
 * Module.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"
#include "Requires.h"

namespace pet {

template<class Child, class Reqs = Requires<>>
class Module: public Registry<ModuleInterface>::StaticElement<Child>, public Reqs {
    protected:
        inline void installModule() {}

        inline virtual unsigned int requires(ModuleInterface* const * &reqs) {
            reqs = Reqs::deps;
            return Reqs::nDeps;
        }

        template<class...> friend class Requires;
        using Registry<ModuleInterface>::StaticElement<Child>::instance;
    public:
        inline virtual ~Module() {}
};

}

#endif /* MODULE_H_ */
