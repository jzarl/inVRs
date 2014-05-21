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

#ifndef _SHAREDMANIPULATIONMERGER_H
#define _SHAREDMANIPULATIONMERGER_H

#include "InteractionSharedLibraryExports.h"
#include <inVRs/SystemCore/TransformationManager/TransformationMerger.h>

class INVRS_INTERACTION_API SharedManipulationMerger : public TransformationMerger {
public:

	SharedManipulationMerger();

	virtual TransformationData calculateResult();
	virtual std::string getName();
	virtual int getMaximumNumberOfInputPipes();

protected:

	bool singleUser;
	gmtl::Vec3f dEntityOrigin;
	gmtl::Quatf orientation;
	gmtl::Vec3f axis;
	gmtl::Vec3f scale;
};

class INVRS_INTERACTION_API SharedManipulationMergerFactory : public TransformationMergerFactory {
	virtual TransformationMerger* create(std::string className, void* args = NULL);
};

#endif
