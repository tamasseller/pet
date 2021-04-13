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

#ifndef BUDDY_H_
#define BUDDY_H_

#include <stddef.h>
#include <stdint.h>

#include "platform/Clz.h"

namespace pet {

/*
 *          +---------------------------------------------------------------------------------------+
 *          |                                         block 1                                       |   level 0
 *          +-------------------------------------------.-------------------------------------------+
 *          |                   block 2                 |                   block 3                 |   level 1
 *          +---------------------.---------------------+---------------------.---------------------+
 *          |        block 4      |        block 5      |        block 6      |        block 7      |   level 2
 *          +----------.----------+----------.----------+----------.----------+----------.----------+
 *          | block 8  | block 9  | block 10 | block 11 | block 12 | block 13 | block 14 | block 15 |   level 3
 *          +----------'----------'----------'----------'----------'----------'----------'----------+
 *          ^
 *          start
 *
 *                                    1
 *                              /         \
 *                           2               3
 *                        /     \         /     \
 *                       4       5       6       7
 *                      / \     / \     / \     / \
 *                     8   9   10  11  12  13  14  15
 *
 *           Node state: free (F)/partially used(P), fully used(U)
 *
 *
 *  Allocate node:
 *
 *   1. Find free node at requested level, whose parent are not used (can be either free or partially used)
 *   2. Mark node as used, all parents as partially used.
 *
 *         F                    P                  P                    P
 *      /     \             /     \             /     \              /     \
 *     F       F    >>>    P      >F<    >>>   P       U     >>>    P       U
 *    / \     / \         / \     / \         / \     / \          / \     / \
 *  >F<  F   F   F       U   F   F   F       U  >F<  F   F        U   U   F   F
 *
 *   |     free      |   |---|  free     |   |---|fre|-------|    |---|---|-------|
 *   ^   ^   ^   ^       ^   ^   ^   ^       ^   ^   ^   ^        ^   ^   ^   ^
 *
 *  Free
 *
 *   1. Find bottom node based on address, go up until used node is found
 *   (1.5) Check if address of found node is consistent with the parameter
 *   2. Mark node as free.
 *   3. Check sibling:
 *    - if not free (U/P) -> exit
 *    - if free:
 *      - if not at the root already -> move one up and restart from 2.
 *      - if at the root -> exit
 *
 *         P                   P                   P                    F
 *      /     \             /     \             /     \              /     \
 *     P       U    >>>    P      >U<   >>>    P       F    >>>     F       F
 *    / \     / \         / \     / \         / \     / \          / \     / \
 *  >U<   U   F   F      F   U   F   F       F  >U<  F   F        F<  F   F   F
 *
 *   |---|---|-------|   |fre|---|-------|   |fre|---|  free |    |     free      |
 *   ^   ^   ^   ^       ^   ^   ^   ^       ^   ^   ^   ^        ^   ^   ^   ^
 */

template<uint32_t minBlockSizeLog, uint32_t maxAlignBits>
class BuddyAllocator
{
    static constexpr auto minBlockSize = 1 << minBlockSizeLog;
    static constexpr auto nBitsPerCell = 2;
    static constexpr auto nCellsPerByte = 8 / nBitsPerCell;
    static constexpr uint32_t nBytesPerWord = sizeof(uint32_t);
    static constexpr auto nCellsPerWord = nBytesPerWord * nCellsPerByte;
    static constexpr auto cellMask = (1 << nBitsPerCell)  - 1;

    char *start, *end;
    uint32_t maxLevel = 0;
    uint32_t *tree;

    enum class NodeState: uint32_t {
        Free = 0,
        Partial = 1,
        Used = 2
    };

    static inline constexpr auto idx2wordIndex(uint32_t idx) {
        return idx / nCellsPerWord;
    }

    static inline constexpr auto idx2bitShift(uint32_t idx) {
        return (idx % nCellsPerWord) * nBitsPerCell;
    }

    inline NodeState getState(uint32_t idx) const {
        auto byteIdx = idx2wordIndex(idx);
        auto bitShift = idx2bitShift(idx);
        return static_cast<NodeState>((tree[byteIdx] >> bitShift) & cellMask);
    }

    inline void setState(uint32_t idx, NodeState state) {
        auto byteIdx = idx2wordIndex(idx);
        auto bitShift = idx2bitShift(idx);
        const auto cleared = tree[byteIdx] & ~(cellMask << bitShift);
        tree[byteIdx] = cleared | ((uint32_t)state) << (bitShift);
    }

    inline uint32_t ptr2idx(void* ptr) {
        return (1 << maxLevel) + ((reinterpret_cast<char*>(ptr) - start) >> minBlockSizeLog);
    }

    static inline auto baseIdx(uint32_t level) {
        return 1 << level;
    }

    static inline auto level(uint32_t idx) {
        return 31 - clz(idx);
    }

    inline char* idx2ptr(uint32_t idx)
    {
        auto leaf = idx << (maxLevel - level(idx));
        auto leafBlockOffset = leaf - (1 << maxLevel);
        return start + (leafBlockOffset << minBlockSizeLog);
    }

    static inline auto parent(uint32_t idx) {
        return idx >> 1;
    }

    inline void indicateUsed(uint32_t idx, uint32_t limit = 0)
    {
        setState(idx, NodeState::Used);

        if(idx != limit)
        {
            while((idx = parent(idx)) != limit)
            {
                setState(idx, NodeState::Partial);
            }
        }
    }

    inline auto checkParent(uint32_t idx)
    {
        while(idx)
        {
            idx = parent(idx);
            if(getState(idx) == NodeState::Used)
                return idx;
        }

        return idx;
    }

    inline auto findActual(void* ptr)
    {
        if(start <= ptr && ptr < end)
        {
            auto idx = ptr2idx(ptr);

            while(getState(idx) != NodeState::Used)
                idx = parent(idx);

            if(idx2ptr(idx) == ptr)
                return idx;
        }

        return decltype(ptr2idx(nullptr))(0);
    }

    static inline auto sibling(uint32_t idx) {
        return idx ^ 1;
    }

    inline uint32_t indicateUnused(uint32_t idx, uint32_t limit = 0)
    {
        do
        {
            setState(idx, NodeState::Free);

            if(getState(sibling(idx)) != NodeState::Free)
                return idx;

            idx = parent(idx);
        }
        while(idx != limit);

        return limit;
    }

    template<class T>
    constexpr static inline T* align(T* ptr, uintptr_t nBytes) {
        return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) & ~(nBytes - 1));
    }

    inline bool size2level(uint32_t &size, uint32_t &level)
    {
    	const uint32_t sBits = 32 - clz(size - 1);

        auto invLevel = (sBits <= minBlockSizeLog) ? 0 : sBits - minBlockSizeLog;

        size = 1 << (invLevel + minBlockSizeLog);

        if(invLevel > maxLevel)
            return false;

        level = maxLevel - invLevel;
        return true;
    }

    inline bool initStartAndMaxLevel(void* start, void* end, uint32_t &nNodeWords)
    {
        // Sanity check
    	auto aligned = align(start, 1 << maxAlignBits);
        if(start >= end || aligned != start)
            return false;

        this->start = reinterpret_cast<char*>(start);
        auto *e = reinterpret_cast<char*>(end);

        auto size = e - this->start;
        const uint32_t sBits = 31 - clz(size);

        // Check if largest allocation is greater then smallest
        if(sBits <= minBlockSizeLog)
            return false;

        this->maxLevel = sBits - minBlockSizeLog;
        auto nNodeCount = 1 << (maxLevel + 1);
        nNodeWords = (nNodeCount + nCellsPerWord - 1) / nCellsPerWord;
        return true;
    }

public:
    static inline int minimalTreeSize(size_t size)
    {
    	const uint32_t sBits = 31 - clz(size);

        if(sBits <= minBlockSizeLog)
            return -1;

        const auto maxLevel = sBits - minBlockSizeLog;
        const auto nNodeCount = 1 << (maxLevel + 1);
        return (nNodeCount + nCellsPerWord - 1) / nCellsPerWord * nBytesPerWord;
    }

    inline bool init(void* start, void* end)
    {
    	uint32_t nNodeWords;

    	if(!initStartAndMaxLevel(start, end, nNodeWords))
    		return false;

    	auto *e = reinterpret_cast<char*>(end);
    	this->tree = reinterpret_cast<decltype(tree)>(align(e - nNodeWords * nBytesPerWord, nBytesPerWord));
        this->end = reinterpret_cast<decltype(this->end)>(align(tree, minBlockSize));

        for(int i = 0; i < nNodeWords; i++)
        	this->tree[i] = 0;

        for(auto p = static_cast<char*>(this->end); p < end; p += minBlockSize)
            indicateUsed(ptr2idx(p));

        return true;
    }

    inline bool init(void* start, void* end, void* treeStart, size_t treeSize)
    {
    	uint32_t nNodeWords;

		if(!initStartAndMaxLevel(start, end, nNodeWords))
			return false;

		if(treeSize < nNodeWords * nBytesPerWord)
			return false;

		this->tree = reinterpret_cast<decltype(tree)>(treeStart);
        this->end = reinterpret_cast<decltype(this->end)>(align(end, minBlockSize));

        for(auto i = 0u; i < nNodeWords; i++)
            this->tree[i] = 0;

        return true;
    }

    inline void* allocate(uint32_t requested, uint32_t &actual)
    {
        if(requested)
        {
            uint32_t searchLevel;

            actual = requested;
            if(!size2level(actual, searchLevel))
                return nullptr;

            auto searchStart = baseIdx(searchLevel);
            auto searchEnd = searchStart << 1;

            for(int i = searchStart; i < searchEnd; i++)
            {
                if(getState(i) == NodeState::Free)
                {
                    if(auto failedAt = checkParent(i))
                    {
                        i = ((failedAt + 1) << (searchLevel - level(failedAt))) - 1;
                    }
                    else
                    {
                        indicateUsed(i);
                        return idx2ptr(i);
                    }
                }
            }
        }

        return nullptr;
    }

    inline void* allocate(uint32_t requested)
    {
    	uint32_t _;
    	return allocate(requested, _);
    }

    inline bool free(void* ptr)
    {
        if(auto idx = findActual(ptr))
        {
            indicateUnused(idx);
            return true;
        }
        else
        {
            return false;
        }
    }

    inline bool adjust(void* ptr, uint32_t requested, uint32_t &actual)
    {
        uint32_t newLevel;

        actual = requested;
        if(size2level(requested, newLevel))
        {
            if(auto idx = findActual(ptr))
            {
                auto oldLevel = level(idx);

                if(oldLevel > newLevel)
                {
                    // Grow

                    auto newIdx = idx >> (oldLevel - newLevel);
                    if(indicateUnused(idx, newIdx) == newIdx)
                    {
                        setState(newIdx, NodeState::Used);
                        return true;
                    }
                    else
                    {
                        indicateUsed(idx, newIdx >> 1);
                        return false;
                    }
                }
                else if(oldLevel < newLevel)
                {
                    // Shrink

                    auto newIdx = ptr2idx(ptr) >> (maxLevel - newLevel);
                    indicateUsed(newIdx, idx >> 1);
                    return true;
                }
                else
                {
                    return true;
                }
            }
        }

        return false;
    }

    inline bool adjust(void* ptr, uint32_t requested)
    {
        uint32_t _;
        return adjust(ptr, requested, _);
    }
};

}

#endif /* BUDDY_H_ */
