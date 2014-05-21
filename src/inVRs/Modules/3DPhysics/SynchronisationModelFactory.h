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

#ifndef _SYNCHRONISATIONMODELFACTORY_H
#define _SYNCHRONISATIONMODELFACTORY_H

#include <inVRs/SystemCore/ClassFactory.h>
#include "SynchronisationModel.h"

/** Class template for SynchronisationModel Factory classes.
 * This class is a template which defines the class structure of a factory
 * class which creates SynchronisationModels. Every SynchronisationModel
 * has to have a factory which creates an instance of it. Therefore a factory
 * class has to be implemented which derives from this class.
 */
class SynchronisationModelFactory : public ClassFactory<SynchronisationModel>
{
public:

//*********************************************//
// PUBLIC METHODS DERIVED FROM: ClassFactory: *//
//*********************************************//

	virtual SynchronisationModel* create(std::string className, void* args = NULL) = 0;

}; // SynchronisationModelFactory

#endif // _SYNCHRONISATIONMODELFACTORY_H
