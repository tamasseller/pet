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

#ifndef PET_UBIQUITOUS_RTTIO_H_
#define PET_UBIQUITOUS_RTTIO_H_

#include <stddef.h>
#include <stdint.h>

#include "meta/Sequence.h"
#include "platform/Compiler.h"

namespace pet {

class RttBufferDescriptor
{
	const char* const name;
	char* const data;
	const uint32_t size;
	volatile uint32_t wIdx = 0, rIdx = 0;
	uint32_t flag = 0;

public:
	template<size_t n> RttBufferDescriptor(const char* name, char (&buffer)[n]):
		name(name), data(buffer), size(n) {}

	class Iterator
	{
		friend RttBufferDescriptor;
		uint32_t idx;

		inline Iterator(uint32_t idx): idx(idx) {}
	};

	inline Iterator startReading() const {
		return {rIdx};
	}

	inline const char* readAccess(Iterator &it)
	{
		if(it.idx != wIdx)
		{
			auto ret = data + it.idx;

			if(++it.idx == size)
				it.idx = 0;

			return ret;
		}

		return nullptr;
	}

	inline void commitRead(const Iterator &it) {
		rIdx = it.idx;
	}

	inline Iterator startWriting() const {
		return {wIdx};
	}

	inline char* writeAccess(Iterator &it)
	{
		auto newIdx = (it.idx + 1);

		if(newIdx == size)
		{
			newIdx = 0;
		}

		if(newIdx != rIdx)
		{
			auto ret = data + it.idx;
			it.idx = newIdx;
			return ret;
		}

		return nullptr;
	}

	inline void commitWrite(const Iterator &it) {
		wIdx = it.idx;
	}

	bool write(const char* data, size_t length);
};


template<class> struct InvertedString;

template<int... idx>
class InvertedString<pet::Sequence<idx...>>
{
public:
	static constexpr auto length = sizeof...(idx);
	constexpr inline InvertedString(const char (&in)[length]): value{(char)~in[idx]...} {}
	really_inline constexpr char operator[](size_t n) const { return value[n]; }

private:
	const char value[length];
};

struct alignas(256) RttControlBlock
{
	static constexpr char nominalId[] = "real-time-xfer";
	static constexpr inline auto obfuscatedId = InvertedString<pet::sequence<0, sizeof(nominalId)>>(nominalId);

	volatile char id[16];
	int nUpBuffers = 1;
	int nDownBuffers = 1;
	RttBufferDescriptor up, down;

	template<size_t nUp, size_t nDown>
	inline RttControlBlock(char (&upBuffer)[nUp], char (&downBuffer)[nDown]):
		up("up", upBuffer), down("down", downBuffer)
	{
		for(auto i = 0u; i < sizeof(id); i++)
		{
			id[i] = i < obfuscatedId.length ? ~obfuscatedId[i] : 0;
		}
	}

	void setupOpenocdRttSink();
	~RttControlBlock();
};

template<size_t upBufferSize, size_t downBufferSize>
class RttIo
{
	RttControlBlock controlBlock;
	char upBuffer[upBufferSize], downBuffer[downBufferSize];

public:
	static constexpr auto upBufferLength = upBufferSize;
	static constexpr auto downBufferLength = downBufferSize;

	inline RttIo(): controlBlock(upBuffer, downBuffer) {}

	really_inline bool write(const char* data, size_t length) {
		return controlBlock.up.write(data, length);
	}

	really_inline auto beginUpTransfer() {
		return controlBlock.up.startWriting();
	}

	really_inline auto beginDownTransfer() {
		return controlBlock.down.startReading();
	}

	really_inline auto accessUpTransfer(RttBufferDescriptor::Iterator& it) {
		return controlBlock.up.writeAccess(it);
	}

	really_inline auto accessDownTransfer(RttBufferDescriptor::Iterator& it) {
		return controlBlock.down.readAccess(it);
	}

	really_inline auto commitUpTransfer(RttBufferDescriptor::Iterator& it) {
		return controlBlock.up.commitWrite(it);
	}

	really_inline auto commitDownTransfer(RttBufferDescriptor::Iterator& it) {
		return controlBlock.down.commitRead(it);
	}

	inline void openocdInit() {
		controlBlock.setupOpenocdRttSink();
	}
};

}

#endif /* PET_UBIQUITOUS_RTTIO_H_ */
