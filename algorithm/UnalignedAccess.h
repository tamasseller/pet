#ifndef PET_ALGORITHM_UNALIGNEDACCESS_H_
#define PET_ALGORITHM_UNALIGNEDACCESS_H_

namespace pet {

template<class T>
inline void unalignedWrite(void* addr, const T& v)
{
	struct __attribute__((packed)) D {
		T v;
	};

	((D*)addr)->v = v;
}

template<class T>
inline auto unalignedRead(const void* addr)
{
	struct __attribute__((packed)) D {
		T v;
	};

	return ((D*)addr)->v;
}

}

#endif /* PET_ALGORITHM_UNALIGNEDACCESS_H_ */
