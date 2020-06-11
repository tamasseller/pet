/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef IDENTIFIERREGISTRY_H_
#define IDENTIFIERREGISTRY_H_

#include "data/AvlTree.h"
#include "algorithm/Fnv.h"
#include "algorithm/Str.h"
#include "pool/StringCollector.h"

namespace pet {

/**
 * Definition order and visibility workaround.
 *
 * This namespace contains classes that should have been nested into the IdentifierRegistry class.
 */
namespace IdentifierRegistryInternals{
	/**
	 * String header with hash.
	 *
	 * Used for a special storage solution, in which a HString precedes
	 * an actual null-terminated character string. Contains the hash
	 * of the string following it.
	 */
	class Hstring {
		friend class IdentifierRegistryBase;
		friend class HstringTest;
		unsigned int hashres;
	public:
		/** Initialize as invalid */
		inline Hstring();

		/**
		 * Initialize with hash.
		 *
		 * Initializes with the hash of the provided string.
		 *
		 * @param	str The string that is to be used for initial hash calculation.
		 */
		inline Hstring(const char* str);

		/** Placement new operator */
		inline void* operator new(size_t, void*);

		/**
		 * The string after the header.
		 *
		 * @return	The string that this header belongs for.
		 */
		inline const char* str() const;

		/**
		 * Hash based comparison
		 *
		 * @return	-1, 0 or 1 according the ordering to the hash (or to the strings if the hashes match).
		 */
		inline int operator - (const Hstring&) const;
	};

	/**
	 * Storage for the associated IDs.
	 *
	 * Serves as a header before the actual string in the storage.
	 * Contains the ids along with pointers required by the container.
	 */
	class Node: public AvlTree::Node {
	public:
		unsigned int id; //!< The associated numeric id.
		Hstring hstr;    //!< The header of the stored string.

		/** Initialize with hash and invalid numeric ID */
		Node(): id(-1), hstr((char*)this + sizeof(Node)) {}

		/** Placement new operator */
		inline void* operator new( size_t sz, void* here ) {return here;}
	};
}

/**
 * String to numeric ID converter.
 *
 * Stores unique strings along with an associated unique numeric ID.
 * Provides two way lookup. It is optimized for string to number lookups.
 *
 * The memory management related methods are not implemented in this base.
 *
 * @note This class is **not supposed to be used directly** by the end user.
 *
 * @see IdenteifierRegistryBase for the fully usable implementation.
 */

class IdentifierRegistryBase: 	protected pet::AvlTree,
								protected pet::IterativeSearch<
									IdentifierRegistryBase,
									IdentifierRegistryInternals::Node>::Decorator{
protected:
	/** Internalized IdentifierRegistryInternals::Node */
	typedef IdentifierRegistryInternals::Node Node;

	/** Comparator for the binary tree storage */
	static int comparator(BinaryTree::Node* node, const IdentifierRegistryInternals::Hstring &key);
public:
	/**
	 * Iterator over the stored IDs.
	 *
	 * Walks the binary tree used for storing the assignments
	 * according the ordering of hashes of the names.
	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class Iterator: public pet::BinaryTree::Iterator {
		typedef pet::BinaryTree Tree;
		friend IdentifierRegistryBase;
		inline Iterator(const Tree::Iterator& it): Tree::Iterator(it) {}
	public:
		/** @copydoc pet::BinaryTree::Iterator::current() const */
		inline Node* current() const {
			if(Tree::Node* ret = Tree::Iterator::current())
				return (Node*) ret;

			return 0;
		}

		/** @copydoc pet::BinaryTree::Iterator::step() */
		inline void step() {
			Tree::Iterator::step();
		}
	};

	/** @copydoc pet::BinaryTree::iterator() */
	inline Iterator iterator() const;
};

/**
 * String to numeric ID converter.
 *
 * Stores unique strings along with an associated unique numeric ID.
 * Provides two way lookup. It is optimized for string to number lookups,
 * and does them in logarithmic time. Lookup in the other way has linear
 * complexity. Uses the provided low-level Allocator for memory management.
 *
 * There is always an active name, that can be appended to.
 * When done appending it has to be closed, then:
 *
 *	1. A new active name is started.
 *	2. A unique numeric ID is returned, that can be:
 * 	    - the same as the one returned for any previous invocation on the same name,
 * 	    - or if there was none, then the next free number available.
 *
 * @tparam	Allocator The allocator te be used.
 * @tparam	Average anticipated name length.
 * @tparam	The number of average sized names to be put in the same memory frame.
 *
 * @note This is the actual full class supposed to be used by the end user.
 */
template <class Allocator, unsigned int avgStrLen=8, unsigned int countPerFrame=8>
class IdentifierRegistry: 	private IdentifierRegistryBase,
							private pet::StringCollector<
								(sizeof(IdentifierRegistryBase::Node)+avgStrLen)*countPerFrame,
								sizeof(IdentifierRegistryBase::Node),
								Allocator>
{
	typedef pet::StringCollector<(sizeof(IdentifierRegistryBase::Node)+8)*8, sizeof(IdentifierRegistryBase::Node), Allocator> Store;

	unsigned int lastId = 0;
public:
	/**
	 * Append a zero-terminated string.
	 *
	 * Appends a null-terminated character string to the active name.
	 *
	 * @param	str The string to be appended.
	 * @return	True on succes, false if the name is too long.
	 */
	inline bool put(const char* str);

	/**
	 * Append a character.
	 *
	 * Appends a single character to the active name.
	 *
	 * @return	True on succes, false if the name is too long.
	 */
	inline bool put(char c);

	/**
	 * Close name.
	 *
	 * Closes the active name, returns the unique ID and starts a new empty active name.
	 *
	 * @return	The unqiue ID of the name.
	 */
	inline int done();

	/**
	 * Close with string append.
	 *
	 * Append zero-terminated character string before closing the name.
	 * Otherwise same as done.
	 *
	 * @param	str The final string to be appended.
	 * @return	The unqiue ID of the name or -1 on error.
	 */
	inline int final(const char *str);

	/**
	 * Reverse lookup.
	 *
	 * Looks up the name for a given numeric ID.
	 *
	 * @return	The name of unqiue ID or NULL on error.
	 */
	inline const char* get(unsigned int) const;

	/** Internalized IdentifierRegistryInternals::Iterator */
	typedef IdentifierRegistryBase::Iterator Iterator;
	using IdentifierRegistryBase::iterator;
};

//////////////////////////////////////////////////////////////////////////////

inline IdentifierRegistryInternals::Hstring::Hstring(): hashres(-1) {}

inline IdentifierRegistryInternals::Hstring::Hstring(const char* str){
	hashres = pet::Fnv::hash(str);
}

inline const char* IdentifierRegistryInternals::Hstring::str() const{
	return ((const char*)this)+sizeof(Hstring);
}

inline int IdentifierRegistryInternals::Hstring::operator - (const Hstring& o) const
{
	if(int ret = o.hashres - hashres)
		return ret;

	return pet::Str::cmp(str(), o.str());
}

inline void* IdentifierRegistryInternals::Hstring::operator new(size_t u, void* ret){
	return ret;
}

inline IdentifierRegistryBase::Iterator IdentifierRegistryBase::iterator() const {
	return pet::AvlTree::iterator();
}

inline int IdentifierRegistryBase::comparator(BinaryTree::Node* node, const IdentifierRegistryInternals::Hstring &key) {
	return ((Node*)node)->hstr - key;
}

template <class Allocator, unsigned int avgStrLen, unsigned int countPerFrame>
inline bool IdentifierRegistry<Allocator, avgStrLen, countPerFrame>::put(char c){
	return Store::put(c);
}

template <class Allocator, unsigned int avgStrLen, unsigned int countPerFrame>
inline bool IdentifierRegistry<Allocator, avgStrLen, countPerFrame>::put(const char *c){
	return Store::put(c);
}

template <class Allocator, unsigned int avgStrLen, unsigned int countPerFrame>
inline int IdentifierRegistry<Allocator, avgStrLen, countPerFrame>::final(const char *c){
	if(put(c))
		return done();

	return -1;
}

template <class Allocator, unsigned int avgStrLen, unsigned int countPerFrame>
inline int IdentifierRegistry<Allocator, avgStrLen, countPerFrame>::done(){
	IdentifierRegistryBase::Node* newNode = new(Store::peek()) Node();

	Position pos = seek<IdentifierRegistryInternals::Hstring, comparator>(newNode->hstr);

	if(pos.getNode()){
		Store::discard();
		return ((IdentifierRegistryBase::Node*)pos.getNode())->id;
	}else{
		Store::terminate();
		newNode->id = lastId;
		insert(pos, newNode);
		return lastId++;
	}
}

template <class Allocator, unsigned int avgStrLen, unsigned int countPerFrame>
inline const char* IdentifierRegistry<Allocator, avgStrLen, countPerFrame>::get(unsigned int id) const
{
	if(Node *ret = findByFields(id, &Node::id))
		return ret->hstr.str();

	return 0;
}

}

#endif /* IDENTIFIERREGISTRY_H_ */
