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

#ifndef _OOPSMATH_H
#define _OOPSMATH_H

#include <ode/ode.h>
#include <gmtl/QuatOps.h>

inline gmtl::Quatf& convert(dQuaternion& src, gmtl::Quatf& dst)
{
	dst[0] = src[1];
	dst[1] = src[2];
	dst[2] = src[3];
	dst[3] = src[0];
	return dst;
} // convert

inline dQuaternion& convert(gmtl::Quatf& src, dQuaternion& dst)
{
	dst[0] = src[3];
	dst[1] = src[0];
	dst[2] = src[1];
	dst[3] = src[2];
	return dst;
} // convert

inline dMatrix3& convert(gmtl::Quatf& src, dMatrix3& dst)
{
	dQuaternion tmpQuat;
	dQtoR(convert(src, tmpQuat), dst);
	return dst;
} // convert

#endif // _OOPSMATH_H
