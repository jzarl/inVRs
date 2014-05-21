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
 * rlander:                                                                  *
 *   Created on: Jul 29, 2009                                                *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef BUTTONCORRECTION_H_
#define BUTTONCORRECTION_H_

#include "ControllerManagerSharedLibraryExports.h"

/******************************************************************************
 * Stores correction values for controller button data
 */
class CONTROLLERMANAGER_API ButtonCorrection {
public:

	/**
	 *
	 */
	explicit ButtonCorrection(bool invert=false);

	/**
	 *
	 */
	int correctButton(int buttonValue) const;

	/**
	 *
	 */
	void setInvert(bool invert);

	/**
	 *
	 */
	bool getInvert() const;

private:
	bool invert;
}; // ButtonCorrection

extern const ButtonCorrection DefaultButtonCorrection;

#endif /* BUTTONCORRECTION_H_ */
