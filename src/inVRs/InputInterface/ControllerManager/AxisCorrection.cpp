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

#include "AxisCorrection.h"

const AxisCorrection DefaultAxisCorrection = AxisCorrection();

AxisCorrection::AxisCorrection(float offset, float scale) :
	offset(offset),
	scale(scale)
{}

bool AxisCorrection::operator==(const AxisCorrection& rhs) const {
	return (this->scale == rhs.scale &&
			this->offset == rhs.offset);
} // operator==

float AxisCorrection::correctAxis(float srcValue) const {
	srcValue += offset;
	srcValue *= scale;
	return srcValue;
} // correctAxis

void AxisCorrection::setOffset(float offset) {
	this->offset = offset;
} // setOffset

void AxisCorrection::setScale(float scale) {
	this->scale = scale;
} // setScale

float AxisCorrection::getOffset() const {
	return offset;
} // getOffset

float AxisCorrection::getScale() const {
	return scale;
} // getScale
