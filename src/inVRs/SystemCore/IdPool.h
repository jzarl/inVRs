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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _IDPOOL_H
#define _IDPOOL_H

#include <vector>
#include <deque>
#include "Platform.h"

/****************************************************************************** 
 * This class provides services for managing integer based ID numbers. It is
 * mainly used for IDs which belong to dynamically created objects (such as 
 * entities ...).
 * In case of a networked application where all instances are able to create 
 * objects it must be ensured that IDs assigned to newly created entities do not 
 * conflict. This class does not provide any synchronisation mechanisms by 
 * itself, it is intended to be used in conjunction with the IdPoolListener.
 * IdPools allways manage a continous range of IDs.
 *
 * @see IdPoolManager
 */
class INVRS_SYSTEMCORE_API IdPool {
public:

	/** 
	 * Allocates a pool of IDs begining at minIdx and ending at maxIdx, both 
	 * inclusive
	 * @param minIdx lowest ID belonging to this instance
	 * @param maxIdx highest ID belonging to this instance
	 */
	IdPool(unsigned minIdx, unsigned maxIdx); // both inclusive

	~IdPool();

	/** 
	 * Creates a new instance of IdPool (a 'subpool') containing size entries. The 
	 * IDs owned by the subpool are marked as reserved in its parent. In case the 
	 * method fails NULL is returned. Depending on the implementation it might 
	 * fail for various reasons, not necessarily only when it is running out of 
	 * available IDs.
	 * @param size minimum number of IDs of the subpool
	 * @return pointer to the subpool, NULL in case of failure
	 */
	IdPool* allocSubPool(unsigned size);

	/** 
	 * Tries to allocate a subpool with the specified range. Ohterwise it behaves 
	 * as allocSubPool() with size provided.
	 * @param size minimum number of IDs of the subpool
	 * @return pointer to the subpool, NULL in case of failure
	 */
	IdPool* allocSubPool(unsigned startIdx, unsigned size);

	/** 
	 * Allocates a single ID. If the allocated ID is not used anymore call 
	 * <code>freeEntry()</code> on it.
	 * @param succeeded if != NULL will indicated whether finding an unused ID was 
	 * successfull
	 * @return an unused ID, 0 on failure
	 */
	unsigned allocEntry(bool* succeeded = 0);

	/** 
	 * Marks a single ID as reserved. If the allocated ID is not used anymore call 
	 * <code>freeEntry()</code> on it.
	 * @param id requested ID
	 * @return returns true on success and false on failure
	 */
	bool allocEntryAt(unsigned id);

	/** 
	 * Releases a previousely reserved ID (by <code>allocEntry()</code> or by 
	 * <code>allocEntryAt()</code> for instance)
	 * @param id id which should be released
	 */
	void freeEntry(unsigned id);

	/** 
	 * Releases a previousely allocated subpool.
	 * @param pool pool which should be released
	 */
	bool freeSubPool(IdPool* pool);

	/** 
	 * Returns the lowest ID of a "very large" block of unused IDs The current 
	 * implementation starts allocating IDs at the bottom of its range and it 
	 * returns the lowest ID of a block which goes to the end of the allocated 
	 * range. The ID returned is not marked as reserved. This method is used by 
	 * the IdPoolListener in order to find an initial guess where all instances 
	 * could allow a subpool to be allocated.
	 * @return an ID (not reserved)
	 */
	unsigned getUnallocatedSubPoolIdx();

	/** 
	 * Returns minimal ID belonging to this pool
	 * @return minimal ID
	 */
	unsigned getMinIdx();

	/** 
	 * Returns maximum ID belonging to this pool
	 * @returns maximum ID
	 */
	unsigned getMaxIdx();

protected:

	std::vector<IdPool*> subPoolList;
	std::deque<unsigned> freeIdList;
	bool hasAllocatedIds;

	unsigned minIdx;
	unsigned maxIdx;
};

#endif
