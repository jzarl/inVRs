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
 *   Added method correctAxis                                                *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef AXISCORRECTION_H_
#define AXISCORRECTION_H_

#include "ControllerManagerSharedLibraryExports.h"

/******************************************************************************
 * Stores correction values for controller axis data
 */
class CONTROLLERMANAGER_API AxisCorrection {
public:
	/**
	 * Constructor
	 */
	explicit AxisCorrection(float offset=0, float scale=1);

	/**
	 * Comparison operator
	 */
	bool operator==(const AxisCorrection& rhs) const;

	/**
	 * corrects the passed axis value and returns the result
	 */
	float correctAxis(float srcValue) const;

	/**
	 * Sets the offset value
	 */
	void setOffset(float offset);

	/**
	 * Sets the scale value
	 */
	void setScale(float scale);

	/**
	 * Returns the offset value
	 */
	float getOffset() const;

	/**
	 * Returns the scale value
	 */
	float getScale() const;

private:
	float scale; // scale value for axis
	float offset; // offset value for axis
}; // AxisCorrection

extern const AxisCorrection DefaultAxisCorrection;// = AxisCorrection();

#endif /* AXISCORRECTION_H_ */
