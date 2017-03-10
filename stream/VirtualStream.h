/*
 * IndirectStream.h
 *
 *  Created on: 2017.03.03.
 *      Author: tooma
 */

#ifndef INDIRECTSTREAM_H_
#define INDIRECTSTREAM_H_

#include "Stream.h"

namespace pet {

class VirtualStream: public Stream<VirtualStream> {
	friend Stream<VirtualStream>;
	virtual pet::GenericError nextBlock(StreamState* state, uint16_t lengthHint, bool growAsNeeded) = 0;
	virtual pet::GenericError blockDone(StreamState* state) = 0;
public:
	inline virtual ~VirtualStream () {}
};

}

#endif /* INDIRECTSTREAM_H_ */
