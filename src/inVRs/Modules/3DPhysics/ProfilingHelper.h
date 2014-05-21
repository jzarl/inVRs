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

/*
 * ProfilingHelper.h
 *
 *  Created on: Aug 13, 2008
 *      Author: landi
 */

#ifndef PROFILINGHELPER_H_
#define PROFILINGHELPER_H_

#include <string>
#include <vector>
#include <map>
#include <inVRs/SystemCore/Timer.h>

using namespace inVRsUtilities;

class ProfilingHelper {
public:
	ProfilingHelper(std::string logFile, int numValues,
			char** dataNames);
	virtual ~ProfilingHelper();

	virtual void start(Timer& timer);
	virtual void dt(unsigned index, Timer& timer);
	virtual void stop(Timer& timer);

protected:
	FILE* logFile;
	double lastTime;
	int numValues;
	double* data;
	double sumValues;

	virtual void initialize(std::string logFile, int numValues,
			char** dataNames);
};

#endif /* PROFILINGHELPER_H_ */
