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

#ifndef ALLINONETRANSLATIONMODEL_H_
#define ALLINONETRANSLATIONMODEL_H_

#include "TranslationModels.h"

class AllInOneTranslationModel : public TranslationModel
{
protected:
	unsigned upDownIdx;
	unsigned speedIdx;
	float upDownSpeed;
	float forBackSpeed;
public:
	AllInOneTranslationModel(unsigned upDownIdx, unsigned speedIdx, float forBackSpeed, float upDownSpeed);
	virtual ~AllInOneTranslationModel() {};
	virtual void getTranslation(ControllerInterface* controller, const gmtl::Quatf& rotationChange,
			gmtl::Vec3f& result, float dt);
};

class AllInOneTranslationModelFactory : public TranslationModelFactory
{
public:
	virtual ~AllInOneTranslationModelFactory();
	virtual TranslationModel* create(std::string className, ArgumentVector* args = NULL);
};


#endif /*ALLINONETRANSLATIONMODEL_H_*/
