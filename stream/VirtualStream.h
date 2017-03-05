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

class VirtualStream: public IoStream<VirtualStream> {
	template<class, class>
	friend class ::InputStreamLogic;

	template<class, class>
	friend class ::OutputStreamLogic;

	friend IoStream<VirtualStream>;
	virtual pet::GenericError nextBlock(ReadLength* actLen, char* &buff, uint16_t length) = 0;
	virtual pet::GenericError nextBlock(WriteLength* actLen, char* &buff, uint16_t length) = 0;
	virtual pet::GenericError blockDone(ReadLength* actLen, char* &buffer, uint32_t &offset) = 0;
	virtual pet::GenericError blockDone(WriteLength* actLen, char* &buffer, uint32_t &offset) = 0;
public:
	inline virtual ~VirtualStream () {}
};

}

#endif /* INDIRECTSTREAM_H_ */
