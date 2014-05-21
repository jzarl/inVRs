/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

#include "IdPool.h"

#include <assert.h>
#include <algorithm>

#include "DebugOutput.h"

IdPool::IdPool(unsigned minIdx, unsigned maxIdx) {
	assert(maxIdx> minIdx);
	this->minIdx = minIdx;
	this->maxIdx = maxIdx;
	for (unsigned i = minIdx; i <= maxIdx; i++) {
		freeIdList.push_back(i);
	} // for
	hasAllocatedIds = false;
} // IdPool

IdPool::~IdPool() {
	while (subPoolList.size() > 0) {
		if (!freeSubPool(subPoolList[0])) {
			printd(ERROR, "IdPool destructor: deallocation of subpool (%u,%u) failed!\n",
					subPoolList[0]->minIdx, subPoolList[0]->maxIdx);
			delete subPoolList[0];
			subPoolList.erase(subPoolList.begin());
		} // if
	} // while
} // ~IdPool

IdPool* IdPool::allocSubPool(unsigned size) {

	unsigned startId = 0;
	unsigned endId = startId + size - 1;

	if (hasAllocatedIds) {
		printd(ERROR,
				"IdPool::allocSubPool(): Can't allocate SubPool of IdPool which has already allocated Ids!\n");
		return NULL;
	} // if

	std::vector<IdPool*>::iterator subPoolIterator = subPoolList.begin();
	IdPool* subPool;

	// find proper Id range which does not collide with other reserved IdPools
	while (subPoolIterator != subPoolList.end()) {
		subPool = *subPoolIterator;

		// 1. check if startId + range enters range of subPool
		// 2. check if startId lies within subPool
		if ( (startId < subPool->minIdx && endId >= subPool->minIdx)
				|| (startId >= subPool->minIdx && startId <= subPool->maxIdx) ) {
			startId = subPool->maxIdx + 1;
			endId = startId + size - 1;
			subPoolIterator = subPoolList.begin();
		} // if
		else {
			++subPoolIterator;
		} // else
	} // while

	return allocSubPool(startId, size);
} // allocSubPool

IdPool* IdPool::allocSubPool(unsigned startIdx, unsigned size) {
	IdPool* result;
	unsigned endIdx = startIdx + size - 1;

	if (hasAllocatedIds) {
		printd(ERROR,
				"IdPool::allocSubPool(): Can't allocate SubPool of IdPool which has already allocated Ids!\n");
		return NULL;
	} // if

	if (startIdx < minIdx) {
		printd(
				ERROR,
				"IdPool::allocSubPool(): Can't allocate SubPool with start Index %u in IdPool with Id-range from %u to %u!\n",
				startIdx, minIdx, maxIdx);
		return NULL;
	} // if

	if (startIdx + size - 1 > maxIdx) {
		printd(
				ERROR,
				"IdPool::allocSubPool(): Could not satisfy allocation request of size %u, startIdx = %u, maxIdx = %u\n",
				size, startIdx, maxIdx);
		return NULL;
	} // if

	//TODO: avoid duplicate execution of this check when calling other allocSubPool method!!!
	std::vector<IdPool*>::iterator subPoolIterator;
	IdPool* subPool;

	for (subPoolIterator = subPoolList.begin(); subPoolIterator != subPoolList.end(); ++subPoolIterator) {
		subPool = *subPoolIterator;

		// 1. check if startId + range enters range of subPool
		// 2. check if startId lies within subPool
		if ( (startIdx < subPool->minIdx && endIdx >= subPool->minIdx)
				|| (startIdx >= subPool->minIdx && startIdx <= subPool->maxIdx) ) {
			printd(
					ERROR,
					"IdPool::allocSubPool(): Could not satisfy allocation request because Ids from %u to %u are already allocated by SubPool (from %u to %u)!\n",
					startIdx, endIdx, subPool->minIdx, subPool->maxIdx);
			return NULL;
		} // if
	} // while

	result = new IdPool(startIdx, endIdx);
	subPoolList.push_back(result);
	return result;
} // allocSubPool

unsigned IdPool::allocEntry(bool* succeeded) {

	unsigned result;

	if (subPoolList.size() > 0) {
		printd(ERROR,
				"IdPool::allocEntry(): Can't allocate Id in pool which has registered SubPools!\n");
		if (succeeded) {
			*succeeded = false;
		} // if
		return 0;
	} // if

	if (freeIdList.size() == 0) {
		printd(ERROR, "IdPool::allocEntry(): No more free Ids available!\n");
		if (succeeded) {
			*succeeded = false;
		} // if
		return 0;
	} // if

	result = freeIdList.front();
	freeIdList.pop_front();

	if (!hasAllocatedIds)
		hasAllocatedIds = true;
	if (succeeded)
		*succeeded = true;
	return result;
} // allocEntry

bool IdPool::allocEntryAt(unsigned id) {

	if (subPoolList.size() > 0) {
		printd(ERROR,
				"IdPool::allocEntryAt(): Can't allocate Id in pool which has registered SubPools!\n");
		return false;
	} // if

	if (id < minIdx || id > maxIdx) {
		printd(
				ERROR,
				"IdPool::allocEntryAt(): requested Id %u is out of the IdPools Id-range from %u to %u!\n",
				id, minIdx, maxIdx);
		return false;
	} // if

	std::deque<unsigned>::iterator index = std::find(freeIdList.begin(), freeIdList.end(), id);
	if (index == freeIdList.end()) {
		printd(ERROR, "IdPool::allocEntryAt(): requested Id %u is already in use!\n", id);
		return false;
	} // if

	freeIdList.erase(index);

	if (!hasAllocatedIds)
		hasAllocatedIds = true;

	return true;
} // allocEntryAt


void IdPool::freeEntry(unsigned id) {
	assert(subPoolList.size() == 0);
	//TODO: maybe increase performance by using an additional bitfield for storing free/used IDs
	std::deque<unsigned>::iterator it = std::find(freeIdList.begin(), freeIdList.end(), id);
	if (it == freeIdList.end()) {
		freeIdList.push_back(id);
	} // if
	else {
		printd(ERROR, "IdPool::freeEntry(): ID %u was not allocated!\n", id);
	} // else
} // freeEntry


bool IdPool::freeSubPool(IdPool* pool) {

	assert(!hasAllocatedIds);

	std::vector<IdPool*>::iterator index = std::find(subPoolList.begin(), subPoolList.end(), pool);

	if (index == subPoolList.end()) {
		printd(ERROR,
				"IdPool::freeSubPool(): couldn't find pool with range (%u, %u) in subPoolList\n",
				pool->minIdx, pool->maxIdx);
		return false;
	} // if

	subPoolList.erase(index);
	delete pool;
	return true;
} // freeSubPool


unsigned IdPool::getUnallocatedSubPoolIdx() {

	//TODO: add support for already freed SubPools!
	assert(!hasAllocatedIds);

	unsigned result = 0;

	std::vector<IdPool*>::iterator it;
	IdPool* subPool;

	for (it = subPoolList.begin(); it != subPoolList.end(); ++it) {
		subPool = *it;

		if (result <= subPool->maxIdx) {
			result = subPool->maxIdx + 1;
		} // if
	} // for

	return result;
} // getUnallocatedSubPoolIdx

unsigned IdPool::getMinIdx() {
	return minIdx;
} // getMinIdx

unsigned IdPool::getMaxIdx() {
	return maxIdx;
} // getMaxIdx

