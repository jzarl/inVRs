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

#ifndef _JOINTINERACTIONMATH_H
#define _JOINTINERACTIONMATH_H

template <typename DATA_TYPE>
inline dQuaternion& convert(dQuaternion& result, const gmtl::Quat<DATA_TYPE>& quat) {
	result[0] = quat[3];
	result[1] = quat[0];
	result[2] = quat[1];
	result[3] = quat[2];
	return result;
}

template <typename DATA_TYPE>
inline gmtl::Quat<DATA_TYPE>& convert(gmtl::Quat<DATA_TYPE>& result, const dQuaternion& quat) {
	result[0] = quat[1];
	result[1] = quat[2];
	result[2] = quat[3];
	result[3] = quat[0];
	return result;
}

template <typename DATA_TYPE>
inline gmtl::Quat<DATA_TYPE>& convert(gmtl::Quat<DATA_TYPE>& result, const dReal* quat) {
	result[0] = quat[1];
	result[1] = quat[2];
	result[2] = quat[3];
	result[3] = quat[0];
	return result;
}

template<typename DATA_TYPE, unsigned SIZE>
inline gmtl::Vec<DATA_TYPE, SIZE>& convert (gmtl::Vec<DATA_TYPE, SIZE>& result, const dReal* vec) {
	result[0] = vec[0];
	result[1] = vec[1];
	result[2] = vec[2];
	return result;
}

#endif
