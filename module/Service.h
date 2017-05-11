/*
 * Service.h
 *
 *      Author: tamas.seller
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include "Tag.h"

namespace pet {

template<class Child>
class Service: public Tag {
    public:
        static Service instance;
};

template<class Child>
Service<Child> Service<Child>::instance;

}


#endif /* SERVICE_H_ */
