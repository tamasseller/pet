/*******************************************************************************
 *
 * Copyright (c) 2021 Tamás Seller. All rights reserved.
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

#ifndef PET_MANAGED_UNIQUE_H_
#define PET_MANAGED_UNIQUE_H_

#include "meta/Utility.h"
#include "meta/Resettable.h"
#include "platform/Compiler.h"

namespace pet
{

template<class Target, class Allocator>
class Unique
{
    class PtrBase
    {
    protected:
        Target* target = nullptr;

        really_inline void acquire(Target* newTarget)
        {
            if((target = newTarget) != nullptr)
            {
                Allocator::traceReferenceAcquistion(this, newTarget);
            }
        }

        inline PtrBase& operator =(nullptr_t)
        {
            auto oldTarget = target;
            target = nullptr;

            if(oldTarget)
            {
                Allocator::traceReferenceRelease(this, oldTarget);
                delete oldTarget;
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

            delete oldTarget;
            return *this;
        }

        inline ~PtrBase()
        {
            if(target)
            {
                Allocator::traceReferenceRelease(this, target);
                delete target;
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
        PtrBase(const PtrBase &) = delete;

        inline PtrBase(PtrBase &&other): target(other.target)
        {
            if(target)
            {
                Allocator::traceReferenceAcquistion(this, target);
                Allocator::traceReferenceRelease(&other, target);
            }

            other.target = nullptr;
        }
    };

public:
    template<class T = Target>
    class Ptr: public PtrBase
    {
        friend Unique;

        struct Disambiguator {};

        really_inline Ptr(const Disambiguator&, Target* target) {
            this->acquire(target);// GCOV_EXCL_LINE
        }

    public:
        using PtrBase::operator==;
        using PtrBase::operator!=;
        using PtrBase::operator!;
        using PtrBase::operator bool;

        really_inline Ptr() = default;
        really_inline Ptr(nullptr_t): PtrBase() {}

        template<class U> Ptr(const Ptr<U>& other) = delete;

        template<class U> really_inline Ptr(Ptr<U>&& other): PtrBase(pet::move(other)) {}

        template<class U> Ptr &operator=(const Ptr<U>& other) = delete;

        template<class U>
        really_inline Ptr &operator=(Ptr<U>&& other)
        {
            PtrBase::operator =(pet::move(other));
            return *this;
        }

        really_inline Ptr &operator=(nullptr_t)
        {
            PtrBase::operator =(nullptr);
            return *this;
        }

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
    really_inline static Ptr<T> make(Args&&... args)
    {
        if(const auto ptr = Allocator::template allocFor<T>())
        {
            return Ptr<T>(typename Ptr<T>::Disambiguator{}, new (ptr, NewOperatorDisambiguator()) T(pet::forward<Args>(args)...));
        }
        else
        {
            return {};
        }
    }
};

}

#endif /* PET_MANAGED_UNIQUE_H_ */
