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

#ifndef ALLINONETRANSLATIONBUTTONMODEL_
#define ALLINONETRANSLATIONBUTTONMODEL_

#include "TranslationModels.h"

class AllInOneTranslationButtonModel : public TranslationModel
{
protected:
	unsigned bttnIdx[4];
	float frontBackSpeed;
	float upDownSpeed;
public:
	AllInOneTranslationButtonModel(unsigned frontIdx, unsigned backIdx, unsigned upIdx, unsigned downIdx, float frontBackSpeed, float upDownSpeed);
	virtual ~AllInOneTranslationButtonModel() {};
	virtual void getTranslation(ControllerInterface* controller, const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt);
};

class AllInOneTranslationButtonModelFactory : public TranslationModelFactory
{
public:
	virtual ~AllInOneTranslationButtonModelFactory();
	virtual TranslationModel* create(std::string className, ArgumentVector* args = NULL);
};


#endif /*ALLINONETRANSLATIONBUTTONMODEL_*/
