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

#ifndef MANAGED_REFCNT_H_
#define MANAGED_REFCNT_H_

#include "platform/Atomic.h"
#include "meta/Resettable.h"
#include "meta/Utility.h"
#include <stdint.h>

namespace pet
{

template<class Target, class Allocator>
class RefCnt
{
    Atomic<uintptr_t> counter = 0;

    class PtrBase
    {
    protected:
        Target* target = nullptr;

        really_inline void acquire(Target* newTarget)
        {
            if((target = newTarget) != nullptr)
            {
                Allocator::traceReferenceAcquistion(this, newTarget);

                target->RefCnt::counter([](auto o, auto &n) { n = o + 1; return true;});
            }
        }

        really_inline void release(Target* trg)
        {
            if(1 == trg->RefCnt::counter([](auto o, auto &n) { n = o - 1; return true;}))
            {
                delete trg;
            }
        }

        inline PtrBase& operator =(const PtrBase &other)
        {
            auto oldTarget = target;

            if(oldTarget)
            {
                Allocator::traceReferenceRelease(this, oldTarget);
            }

            acquire(other.target);

            if(oldTarget)
            {
                release(oldTarget);
            }

            return *this;
        }

        inline PtrBase& operator =(nullptr_t)
        {
            auto oldTarget = target;
            target = nullptr;

            if(oldTarget)
            {
                Allocator::traceReferenceRelease(this, oldTarget);
                release(oldTarget);
            }

            return *this;
        }

        inline PtrBase& operator =(PtrBase &&other)
        {
            auto oldTarget = target;

            if(oldTarget)
            {
                Allocator::traceReferenceRelease(this, target);
            }

            if(other.target)
            {
                target = other.target;
                Allocator::traceReferenceAcquistion(this, target);
                Allocator::traceReferenceRelease(&other, target);
                other.target = nullptr;
            }
            else
            {
                target = nullptr;
            }

            if(oldTarget)
            {
                release(oldTarget);
            }

            return *this;
        }

        inline ~PtrBase()
        {
            if(target)
            {
                Allocator::traceReferenceRelease(this, target);
                release(target);
            }
        }

        really_inline bool operator ==(const PtrBase& o) const {
            return target == o.target;
        }

        really_inline bool operator ==(nullptr_t) const {
            return !target;
        }

        really_inline bool operator !=(const PtrBase& o) const {
            return target != o.target;
        }

        really_inline bool operator !=(nullptr_t) const {
            return target;
        }

        really_inline bool operator !() const {
            return target == nullptr;
        }

        really_inline operator bool() const {
            return target != nullptr;
        }

    public:
        really_inline PtrBase() = default;

        inline PtrBase(PtrBase &&other): target(other.target)
        {
            if(target)
            {
                Allocator::traceReferenceAcquistion(this, target);
                Allocator::traceReferenceRelease(&other, target);
            }

            other.target = nullptr;
        }

        inline PtrBase(const PtrBase &other)
        {
            acquire(other.target);
        }
    };

public:
    template<class T = Target>
    class Ptr: public PtrBase
    {
        friend RefCnt;

        struct Disambiguator {};

        really_inline Ptr(const Disambiguator&, Target* target) {
            this->acquire(target);
        }

    public:
        using PtrBase::PtrBase;
        using PtrBase::operator=;
        using PtrBase::operator==;
        using PtrBase::operator!=;
        using PtrBase::operator!;
        using PtrBase::operator bool;

        template<class U>
        really_inline Ptr(const Ptr<U>& other): PtrBase(other) {}

        template<class U>
        really_inline Ptr(Ptr<U>&& other): PtrBase(pet::move(other)) {}

        really_inline Ptr(nullptr_t): PtrBase() {}

        really_inline T* operator ->() const {
            return static_cast<T*>(this->target);
        }

        really_inline T* get() const {
            return static_cast<T*>(this->target);
        }

        really_inline T& operator *() const {
            return *static_cast<T*>(this->target);
        }
    };

public:
    inline void operator delete(void* ptr) {
        Allocator::free(ptr);
    }

    template<class T = Target, class... Args>
    really_inline static Ptr<T> make(Args&&... args) {
        return Ptr<T>(typename Ptr<T>::Disambiguator{}, new (Allocator::template allocFor<T>(), NewOperatorDisambiguator()) T(pet::forward<Args>(args)...));
    }

    template<class T=Target>
    really_inline Ptr<T> self() {
        return Ptr<T>(typename Ptr<T>::Disambiguator{}, static_cast<Target*>(this));
    }
};

}

#endif /* MANAGED_REFCNT_H_ */
