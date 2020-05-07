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

#ifndef BUDDY_H_
#define BUDDY_H_

#include <stddef.h>
#include <stdint.h>

#include "ubiquitous/Compiler.h"

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

template<uintptr_t minBlockSizeLog, uintptr_t maxAlignBits>
class BuddyAllocator
{
    static constexpr auto minBlockSize = 1 << minBlockSizeLog;
    static constexpr auto nBitsPerCell = 2;
    static constexpr auto nCellsPerByte = 8 / nBitsPerCell;
    static constexpr uintptr_t nBytesPerWord = sizeof(size_t);
    static constexpr auto nCellsPerWord = nBytesPerWord * nCellsPerByte;
    static constexpr auto cellMask = (1 << nBitsPerCell)  - 1;

    char *start, *end;
    size_t maxLevel = 0;
    size_t *tree;

    enum class NodeState: uintptr_t {
        Free = 0,
        Partial = 1,
        Used = 2
    };

    static inline constexpr auto idx2wordIndex(uintptr_t idx) {
        return idx / nCellsPerWord;
    }

    static inline constexpr auto idx2bitShift(uintptr_t idx) {
        return (idx % nCellsPerWord) * nBitsPerCell;
    }

    inline NodeState getState(size_t idx) const {
        auto byteIdx = idx2wordIndex(idx);
        auto bitShift = idx2bitShift(idx);
        return static_cast<NodeState>((tree[byteIdx] >> bitShift) & cellMask);
    }

    inline void setState(size_t idx, NodeState state) {
        auto byteIdx = idx2wordIndex(idx);
        auto bitShift = idx2bitShift(idx);
        const auto cleared = tree[byteIdx] & ~(cellMask << bitShift);
        tree[byteIdx] = cleared | ((uintptr_t)state) << (bitShift);
    }

    inline size_t ptr2idx(char* ptr) {
        return (1 << maxLevel) + ((ptr - start) >> minBlockSizeLog);
    }

    static inline auto baseIdx(size_t level) {
        return 1 << level;
    }

    static inline auto level(size_t idx) {
        return 31 - clz(idx);
    }

    inline char* idx2ptr(size_t idx)
    {
        auto leaf = idx << (maxLevel - level(idx));
        auto leafBlockOffset = leaf - (1 << maxLevel);
        return start + (leafBlockOffset << minBlockSizeLog);
    }

    static inline auto parent(size_t idx) {
        return idx >> 1;
    }

    inline void indicateUsed(size_t idx)
    {
        setState(idx, NodeState::Used);

        while((idx = parent(idx)) != 0)
            setState(idx, NodeState::Partial);
    }

    inline auto checkParent(size_t idx)
    {
        while(idx)
        {
            idx = parent(idx);
            if(getState(idx) == NodeState::Used)
                return idx;
        }

        return idx;
    }

    inline auto findActual(char* ptr)
    {
        auto idx = ptr2idx(ptr);

        while(getState(idx) != NodeState::Used)
            idx = parent(idx);

        return idx;
    }

    static inline auto sibling(size_t idx) {
        return idx ^ 1;
    }

    inline void indicateUnused(size_t idx)
    {
        do
        {
            setState(idx, NodeState::Free);
            idx = parent(idx);
        }
        while(idx && getState(sibling(idx)) == NodeState::Free);
    }

    template<class T>
    constexpr static inline T* align(T* ptr, size_t nBytes) {
        return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) & ~(nBytes - 1));
    }

public:
    inline bool init(void* start, void* end)
    {
        // Sanity check
        if(start >= end || align(start, maxAlignBits) != start)
            return false;

        this->start = reinterpret_cast<char*>(start);
        auto *e = reinterpret_cast<char*>(end);

        auto size = e - this->start;
        auto sBits = 31 - clz(size);

        // Check if largest allocation is greater then smallest
        if(sBits <= minBlockSizeLog)
            return false;

        maxLevel = sBits - minBlockSizeLog;
        auto nNodeCount = 1 << (maxLevel + 1);
        auto nNodeWords = (nNodeCount + nCellsPerWord - 1) / nCellsPerWord;

        tree = reinterpret_cast<decltype(tree)>(align(e - nNodeWords * nBytesPerWord, nBytesPerWord));
        this->end = reinterpret_cast<decltype(this->end)>(align(tree, minBlockSize));

        for(int i = 0; i < nNodeWords; i++)
            tree[i] = 0;

        for(auto p = static_cast<char*>(this->end); p < end; p += minBlockSize)
            indicateUsed(ptr2idx(p));

        return true;
    }

    inline void* allocate(size_t x)
    {
        if(x)
        {
            auto sBits = 32 - clz(x - 1);

            auto invLevel = (sBits <= minBlockSizeLog) ? 0 : sBits - minBlockSizeLog;

            if(invLevel > maxLevel)
                return nullptr;

            auto searchLevel = maxLevel - invLevel;
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

    inline void free(void* ptr)
    {
        // TODO check if start <= ptr && ptr < end

        auto idx = findActual(reinterpret_cast<char*>(ptr));

        // TODO check if idx2ptr(idx) == ptr

        indicateUnused(idx);
    }
};

}

#endif /* BUDDY_H_ */
