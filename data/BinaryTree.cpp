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

#include "data/BinaryTree.h"

#include "ubiquitous/Trace.h"

using namespace container;

void BinaryTree::Iterator::step() {
	if(!currentNode)
		return;

	if(goDown && currentNode->big) {
		currentNode = currentNode->big;

		if(currentNode->small) {
			while(currentNode->small)
				currentNode = currentNode->small;

			if(!currentNode->big)
				goDown = false;
		}
	} else {
		while(true) {
			if(!currentNode->parent) {
				currentNode = 0;
				return;
			}

			if(currentNode->parent->big == currentNode) {
				currentNode = currentNode->parent;
			}else {
				BinaryTreeTrace::assert(currentNode->parent->small == currentNode);
				break;
			}
		}

		goDown = true;
		currentNode = currentNode->parent;
	}
}
