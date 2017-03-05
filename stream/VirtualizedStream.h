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

	virtual pet::GenericError nextBlock(ReadLength* actLen, char* &buff, uint16_t reqLen) {
		return Backend::nextBlock(actLen, buff, reqLen);
	}

	virtual pet::GenericError nextBlock(WriteLength* actLen, char* &buff, uint16_t reqLen) {
		return Backend::nextBlock(actLen, buff, reqLen);
	}

	virtual pet::GenericError blockDone(ReadLength* actLen, char* &buff, uint32_t &offset) {
		return Backend::blockDone(actLen, buff, offset);
	}

	virtual pet::GenericError blockDone(WriteLength* actLen, char* &buff, uint32_t &offset) {
		return Backend::blockDone(actLen, buff, offset);
	}

public:
	template<class... T>
	inline VirtualizedStream(T... args): Backend(args...) {}
};

}

#endif /* VIRTUALIZEDSTREAM_H_ */
