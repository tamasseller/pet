/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#include <platform/Atomic.h>
#include <meta/Resettable.h>
#include <meta/Utility.h>
#include <stdint.h>

namespace pet
{

template<class Target, class Allocator>
class RefCnt
{
    Atomic<uintptr_t> counter = 0;

public:
    template<class T = Target>
    class Ptr: pet::Resettable<Ptr<T>>
    {
        friend pet::Resettable<Ptr<T>>;
        Target* target = nullptr;

        inline void acquire(Target* newTarget)
        {
            if((target = newTarget) != nullptr)
            {
                target->counter([](auto o, auto &n) { n = o + 1; return true;});
            }
        }

        inline void release()
        {
            if(target && 1 == target->counter([](auto o, auto &n) { n = o - 1; return true;}))
            {
                target->~Target();
                Allocator::free(target);
            }
        }

        friend RefCnt;
        inline Ptr(Target* target) {
            acquire(target);
        }

    public:
        inline Ptr() = default;

        inline Ptr(const Ptr &other) {
            acquire(other.target);
        }

        inline Ptr(nullptr_t): Ptr(static_cast<Target*>(nullptr)) {}

        inline Ptr(Ptr &&other): target(other.target) {
            other.Ptr::Resettable::reset();
        }

        inline Ptr& operator =(const Ptr &other)
        {
            release();
            acquire(other.target);
            return *this;
        }

        inline Ptr& operator =(nullptr_t) {
            *this = Ptr(nullptr);
            return *this;
        }

        inline Ptr& operator =(Ptr &&other)
        {
            release();
            target = other.target;
            other.Ptr::Resettable::reset();
            return *this;
        }

        inline ~Ptr() {
            release();
        }

        inline void reset()
        {
            release();
            this->Ptr::Resettable::reset();
        }

        inline T* operator ->() const {
            return static_cast<T*>(target);
        }

        inline T* get() const {
            return static_cast<T*>(target);
        }

        inline T& operator *() const {
            return *static_cast<T*>(target);
        }

        inline bool operator ==(const Ptr& o) const {
            return target == o.target;
        }

        inline bool operator ==(nullptr_t) const {
            return !target;
        }

        inline bool operator !=(const Ptr& o) const {
            return target != o.target;
        }

        inline bool operator !=(nullptr_t) const {
            return target;
        }

        inline bool operator !() const {
            return target == nullptr;
        }

        inline operator bool() const {
            return target != nullptr;
        }
    };

    template<class T = Target, class... Args>
    static inline Ptr<T> make(Args&&... args) {
        return new (Allocator::alloc(sizeof(T))) T(pet::forward<Args>(args)...);
    }

    template<class T=Target>
    inline Ptr<T> self() {
        return static_cast<T*>(this);
    }
};

}

#endif /* MANAGED_REFCNT_H_ */
