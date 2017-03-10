/*
 * VirtualizedStream.h
 *
 *  Created on: 2017.03.03.
 *      Author: tooma
 */

#ifndef VIRTUALIZEDSTREAM_H_
#define VIRTUALIZEDSTREAM_H_

#include "VirtualStream.h"

namespace pet {

template<class Backend>
class VirtualizedStream: public VirtualStream, public Backend {
	virtual pet::GenericError nextBlock(StreamState* state, uint16_t lengthHint, bool growAsNeeded) {
		return Backend::nextBlock(state, lengthHint, growAsNeeded);
	}

	virtual pet::GenericError blockDone(StreamState* state) {
		return Backend::blockDone(state);
	}

public:
	template<class... T>
	inline VirtualizedStream(T... args): Backend(args...) {}
};

}

#endif /* VIRTUALIZEDSTREAM_H_ */
