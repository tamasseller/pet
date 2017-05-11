/*
 * Module.h
 *
 *      Author: tamas.seller
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"
#include "Deps.h"

namespace pet {

template<class Child, class Reqs = Requires<>, class Prov = Provides<>>
class Module: public Registry<ModuleInterface>::StaticElement<Child>, public Reqs, public Prov {
    protected:
        inline void installModule() {}

        inline virtual unsigned int requires(const Tag* const * &reqs) {
            reqs = Reqs::deps;
            return Reqs::nDeps;
        }

        inline virtual unsigned int provides(const Tag* const * &serv) {
            serv = Prov::services;
            return Prov::nServices;
        }

    public:
        inline virtual ~Module() {}
};

}

#endif /* MODULE_H_ */
