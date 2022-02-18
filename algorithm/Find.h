/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#ifndef FIND_H_
#define FIND_H_

namespace pet {

/**
 * Binary search.
 *
 * Searches through an array of (non-strictly) ordered elements with O(log(n)) complexity.
 * Provides an interval of matches.
 *
 * @note	Could be more abstract by lifting the requirement of array and
 * 			substituting it with anything that is random access capable.
 */
struct Bisect {
    /**
     * Result of a binary search.
     *
     * Represents the result of the search with maximal fidelity.
     * Passes over everything piece of information that was gathered.
     */
    class Result {
        friend Bisect;
        static const unsigned short invalid = -1;

        unsigned short min=invalid, max;
    public:
        /**
         * Is the element present?
         *
         * @return 	True if a matching element was found.
         */
        inline bool present() const{
            return min != invalid;
        }

        /**
         * Index to insert the element.
         *
         * If there was no match where should the queried element be inserted to preserve ordering.
         *
         * @return 	True if a matching element was found.
         *
         * @note 	Valid only if there was no match (ie. *present* returns *false*).
         */
        inline unsigned short insertionIndex() const{
            return max;
        }

        /**
         * First index of the matched interval.
         *
         * @return The first index of the interval.
         *
         * @note 	Valid only if there was a match (ie. *present* returns *true*).
         */
        inline unsigned short first() const{
            return min;
        }

        /**
         * Last index of the matched interval.
         *
         * @return The last index of the interval.
         *
         * @note 	Valid only if there was a match (ie. *present* returns *true*).
         */
        inline unsigned short last() const{
            return max;
        }

        /**
         * Count of matched elements.
         *
         * @return The number of elements matched.
         *
         * @note 	Valid only if there was a match (ie. *present* returns *true*).
         */
        inline unsigned short count() const{
            return min-max+1;
        }

        /**
         * Is it a single match.
         *
         * @return true if the number of matched elements is one, false otherwise.
         *
         * @note 	Valid only if there was a match (ie. *present* returns *true*).
         */
        inline bool single() const{
            return count() == 1;
        }
    };

    /**
     * Default operator backed comparator.
     *
     * This comparator is provided as a default, it uses the operators > and ==.
     * Also serves as a template for user provided comparator implementations.
     *
     * @tparam Element 	The type of the elements.
     * @tparam Key		The type of the key.
     */
    template <class Element, class Key>
    class DefaultComparator {
        const Key& key;
    public:
        /**
         * Construct from key
         *
         * The comparator is instantiated (locally) by the find method,
         * to enable hacks that need the comparator to be stateful.
         *
         * @param	key The key passed to the find method.
         */
        inline DefaultComparator(const Key& key): key(key) {}

        /**
         * Strict ordering.
         *
         * Decide if the subject is greater than the key.
         *
         * @param subject The element in question.
         */
        inline bool greater(const Element& subject) {
            return subject > key;
        }

        /**
         * Equivalence test.
         *
         * Decide if the subject is equal to the key.
         *
         * @return True if the subject
         */
        inline bool matches(const Element& subject) {
            return subject == key;
        }
    };

    /**
     * Binary search.
     *
     * Searches through an array of (non-strictly) ordered elements with O(log(n)) complexity.
     * Provides an interval of matches.
     *
     * @tparam Element 		The type of the elements.
     * @tparam Key			The type of the key.
     * @tparam Comparator	The type of comparator to be used (and instantiated).
     */
    template <class Element, class Key, class Comparator=DefaultComparator<Element, Key>>
    static Result find(Element* haystack, unsigned int num, Key needle)
    {
        Result ret;

        Comparator comparator(needle);

        if(!num){
            ret.min = Result::invalid;
            ret.max = 0;
            return ret;
        }

        unsigned int bottom=0, top=num-1;

        while(bottom != top){
            unsigned int mid = (bottom + top + 1) / 2;
            if(comparator.greater(haystack[mid]))
                top = mid-1;
            else
                bottom = mid;
        }

        if(comparator.matches(haystack[bottom])){
            while(bottom > 0 && comparator.matches(haystack[bottom-1]))
                bottom--;

            ret.min = bottom;

            while(top < (num-1) && comparator.matches(haystack[top+1]))
                top++;

            ret.max = top;
        }else{
            ret.min = Result::invalid;
            if(top == 0 && comparator.greater(haystack[0]))
                ret.max = bottom;
            else
                ret.max = bottom+1;
        }

        return ret;
    }
};

}

#endif /* FIND_H_ */
