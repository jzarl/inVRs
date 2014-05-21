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

#include "AllInOneTranslationModel.h"
#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/DebugOutput.h>

AllInOneTranslationModel::AllInOneTranslationModel(unsigned upDownIdx, unsigned speedIdx, float forBackSpeed, float upDownSpeed)
{
	this->upDownIdx = upDownIdx;
	this->speedIdx = speedIdx;
	this->upDownSpeed = upDownSpeed;
	this->forBackSpeed = forBackSpeed;
}

void AllInOneTranslationModel::getTranslation(ControllerInterface* controller, const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt)
{
	gmtl::Vec3f temp;

	gmtl::Vec3f frontBackDir = gmtl::Vec3f(0, 0, 1);
	gmtl::Vec3f upDownDir = gmtl::Vec3f(0, 1, 0);

	result = gmtl::Vec3f(0, 0, 0);

	if(controller->getNumberOfAxes() <= (int)speedIdx) return;
	result += frontBackDir*controller->getAxisValue(speedIdx)*forBackSpeed;

	if(controller->getNumberOfAxes() <= (int)upDownIdx) return;
	result += upDownDir*controller->getAxisValue(upDownIdx)*upDownSpeed;

}

AllInOneTranslationModelFactory::~AllInOneTranslationModelFactory()
{

}

TranslationModel* AllInOneTranslationModelFactory::create(
		std::string className, ArgumentVector* args)
{
	if (className != "AllInOneTranslationModel")
	{
		return NULL;
	}
	unsigned upDownIdx = 0;
	unsigned speedIdx = 1;
	float upDownSpeed = 1;
	float forBackSpeed = 1;

	if (args && args->keyExists("upDownIdx"))
		args->get("upDownIdx", upDownIdx);
	else
		printd(WARNING,
				"AllInOneTranslationModelFactory::create(): WARNING: missing attribute upDownIdx, assuming default!\n");

	if (args && args->keyExists("speedIdx"))
		args->get("speedIdx", speedIdx);
	else
		printd(WARNING,
				"AllInOneTranslationModelFactory::create(): WARNING: missing attribute speedIdx, assuming default!\n");

	if (args && args->keyExists("upDownSpeed"))
		args->get("upDownSpeed", upDownSpeed);
	else
		printd(WARNING,
				"AllInOneTranslationModelFactory::create(): WARNING: missing attribute upDownSpeed, assuming default!\n");

	if (args && args->keyExists("forBackSpeed"))
		args->get("forBackSpeed", forBackSpeed);
	else
		printd(WARNING,
				"AllInOneTranslationModelFactory::create(): WARNING: missing attribute forBackSpeed, assuming default!\n");

	return new AllInOneTranslationModel(upDownIdx, speedIdx, forBackSpeed,
			upDownSpeed);
}
