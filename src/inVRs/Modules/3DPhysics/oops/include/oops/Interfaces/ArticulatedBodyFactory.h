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
 * ArticulatedBodyFactory.h
 *
 *  Created on: Nov 25, 2008
 *      Author: rlander
 */

#ifndef ARTICULATEDBODYFACTORY_H_
#define ARTICULATEDBODYFACTORY_H_

#include "../ArticulatedBody.h"
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/XmlElement.h>

namespace oops {

class ArticulatedBodyFactory : ClassFactory<ArticulatedBody, const XmlElement*>
{
public:
	virtual ~ArticulatedBodyFactory() {};

	virtual ArticulatedBody* create(std::string className, const XmlElement* element) = 0;

}; // ArticulatedBodyFactory

} // oopps

#endif /* ARTICULATEDBODYFACTORY_H_ */
