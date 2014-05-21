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

#ifndef _MESSAGESIZECOUNTER_H_
#define _MESSAGESIZECOUNTER_H_

#include <vector>
#include <inVRs/SystemCore/NetMessage.h>

class Physics;

/** Counts the number of bytes per second for the physics simulation.
 * This class is used by the SynchronisationModels to count the number of bytes
 * which are transmitted per second for synchronisation purposes. 
 */
class MessageSizeCounter {
public:

//*****************//
// PUBLIC METHODS: //
//*****************//

	/** Constructor
	 */
	MessageSizeCounter(Physics* physics);
	
	/** Empty destructor
	 */
	virtual ~MessageSizeCounter();
	
	/** Sets the filename where the data is logged into
	 */
	void setLogFileName(std::string fileName);

	/** Adds the size of the passed message to the counter
	 */
	void countBytes(NetMessage* msg);
	
	/** Indicates that the current simulation step is over
	 */
	void stepFinished();
	
	/** Method dumps the counted number of bytes
	 */
	void dumpResults();
	
protected:

//********************//
// PROTECTED MEMBERS: //
//********************//

	Physics* physics;

	std::string logFileName;

	float stepSize;
	
	float stepCounter;
	
	std::vector<unsigned> bytesPerStep;
	
	std::vector<unsigned> bytesPerSecond;

};

#endif /*_MESSAGESIZECOUNTER_H_*/
