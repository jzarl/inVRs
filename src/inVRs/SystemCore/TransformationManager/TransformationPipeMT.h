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

#ifndef _TRANSFORMATIONPIPEMT_H
#define _TRANSFORMATIONPIPEMT_H

// fix problems with Visual Studio 2008
#ifdef WIN32
#include <stdio.h>
#endif
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include "TransformationPipe.h"

/******************************************************************************
 * Thread-safe TransformationPipe.
 * The class is used when different threads have to access the same pipe.
 */
class INVRS_SYSTEMCORE_API TransformationPipeMT : public TransformationPipe {
public:
	/**
	 * Constructor initializes Lock.
	 */
	TransformationPipeMT(uint64_t pipeId, User* owner);

	/**
	 * Empty destructor.
	 */
	virtual ~TransformationPipeMT();

	virtual TransformationData getTransformation(unsigned index);
	virtual void push_back(TransformationData& data);
	virtual int size();
	virtual void flush();

protected:
	/**
	 * Lock for access to Pipe from different Threads
	 */
#if OSG_MAJOR_VERSION >= 2
	//OpenSG 2.0 prevents getting a raw C Pointer, also threading/sync objects are well ref counted
	OSG::LockRefPtr pipeLock;
#else //OpenSG1:
	OSG::Lock* pipeLock;
#endif

}; // TransformationPipeMT

#endif // _TRANSFORMATIONPIPEMT_H
