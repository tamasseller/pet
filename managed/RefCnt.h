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

    class PtrBase
    {
    protected:
    	Target* target = nullptr;

    	inline void acquire(Target* newTarget)
    	{
			if((target = newTarget) != nullptr)
			{
			   target->counter([](auto o, auto &n) { n = o + 1; return true;});
			}
    	}

    	inline void release(Target* trg)
    	{
    		if(trg && 1 == trg->counter([](auto o, auto &n) { n = o - 1; return true;}))
    			delete trg;
    	}

    	really_inline void release() {
    		release(target);
    	}

    	really_inline PtrBase() = default;

		really_inline PtrBase& operator =(nullptr_t) {
            *this = PtrBase();
            return *this;
        }

		really_inline PtrBase(const PtrBase &other) {
            acquire(other.target);
        }

        really_inline PtrBase& operator =(const PtrBase &other)
        {
        	auto oldTarget = target;
            acquire(other.target);
            release(oldTarget);
            return *this;
        }

        really_inline PtrBase(PtrBase &&other): target(other.target) {
        	new(&other, NewOperatorDisambiguator()) PtrBase();
        }

        template<class U>
        really_inline PtrBase& operator =(PtrBase &&other)
        {
            release();
            target = other.target;
            new(&other, NewOperatorDisambiguator()) Ptr<U>();
            return *this;
        }

        really_inline ~PtrBase() {
            release();
        }

        really_inline void reset()
        {
            release();
            new(this, NewOperatorDisambiguator()) PtrBase();
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
    };

public:
    template<class T = Target>
    class Ptr: public PtrBase
    {
    	friend RefCnt;

    	static inline Ptr init(Target* target)
    	{
    		Ptr ret;
    		ret.acquire(target);
    		return ret;
    	}

    public:
    	really_inline Ptr() = default;
    	really_inline Ptr(nullptr_t): PtrBase() {}
    	really_inline Ptr(const PtrBase &other): PtrBase(other) {}
    	really_inline Ptr(PtrBase &&other): PtrBase(pet::move(other)) {}

    	using PtrBase::operator=;
    	using PtrBase::reset;
    	using PtrBase::operator==;
    	using PtrBase::operator!=;
    	using PtrBase::operator!;
    	using PtrBase::operator bool;

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
        return Ptr<T>::init(new (Allocator::alloc(sizeof(T)), NewOperatorDisambiguator()) T(pet::forward<Args>(args)...));
    }

    template<class T=Target>
    really_inline Ptr<T> self() {
        return Ptr<T>::init(static_cast<Target*>(this));
    }
};

}

#endif /* MANAGED_REFCNT_H_ */
