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

#include "AllInOneTranslationButtonModel.h"

#include <inVRs/SystemCore/ArgumentVector.h>
#include <inVRs/SystemCore/DebugOutput.h>

AllInOneTranslationButtonModel::AllInOneTranslationButtonModel(unsigned frontIdx, unsigned backIdx, unsigned upIdx, unsigned downIdx, float frontBackSpeed, float upDownSpeed)
{
	bttnIdx[0] = frontIdx;
	bttnIdx[1] = backIdx;
	bttnIdx[2] = upIdx;
	bttnIdx[3] = downIdx;
	this->frontBackSpeed = frontBackSpeed;
	this->upDownSpeed = upDownSpeed;
}

void AllInOneTranslationButtonModel::getTranslation(ControllerInterface* controller, const gmtl::Quatf& rotationChange, gmtl::Vec3f& result, float dt)
{

	int i;
	gmtl::Vec3f dirs[6];
	gmtl::Vec3f temp;

	dirs[0] = gmtl::Vec3f(0, 0, -1);
	dirs[1] = gmtl::Vec3f(0, 0, 1);
	dirs[2] = gmtl::Vec3f(0, 1, 0);
	dirs[3] = gmtl::Vec3f(0, -1, 0);

	result = gmtl::Vec3f(0, 0, 0);
	for (i=0; i < 2; i++)
	{
		if (controller->getNumberOfButtons() <= (int)bttnIdx[i]) break;
		if (controller->getButtonValue(bttnIdx[i]) != 0)
		{
			temp = dirs[i] * frontBackSpeed;
			result += temp;
		} // if
	} // for

	for (i=2; i < 4; i++)
		{
			if (controller->getNumberOfButtons() <= (int)bttnIdx[i]) break;
			if (controller->getButtonValue(bttnIdx[i]) != 0)
			{
				temp = dirs[i] * upDownSpeed;
				result += temp;
			} // if
		} // for

	//gmtl::normalize(result);
}

AllInOneTranslationButtonModelFactory::~AllInOneTranslationButtonModelFactory()
{

}

TranslationModel* AllInOneTranslationButtonModelFactory::create(std::string className,
		ArgumentVector* args)
{
	if(className != "AllInOneTranslationButtonModel")
	{
		return NULL;
	}

	ArgumentVector* argVec = (ArgumentVector*)args;
	unsigned frontIdx = 0;
	unsigned backIdx = 1;
	unsigned upIdx = 2;
	unsigned downIdx = 3;
	float frontBackSpeed = 1;
	float upDownSpeed = 1;

	if (argVec && argVec->keyExists("frontIdx"))
		argVec->get("frontIdx", frontIdx);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute frontIdx, assuming default!\n");

	if (argVec && argVec->keyExists("backIdx"))
		argVec->get("backIdx", backIdx);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute backIdx, assuming default!\n");

	if (argVec && argVec->keyExists("upIdx"))
		argVec->get("upIdx", upIdx);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute upIdx, assuming default!\n");

	if (argVec && argVec->keyExists("downIdx"))
		argVec->get("downIdx", downIdx);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute downIdx, assuming default!\n");

	if (argVec && argVec->keyExists("frontBackSpeed"))
		argVec->get("frontBackSpeed", frontBackSpeed);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute frontBackSpeed, assuming default!\n");

	if (argVec && argVec->keyExists("upDownSpeed"))
		argVec->get("upDownSpeed", upDownSpeed);
	else
		printd(
				WARNING,
				"AllInOneTranslationButtonModelFactory::create(): WARNING: missing attribute upDownSpeed, assuming default!\n");

	return new AllInOneTranslationButtonModel(frontIdx, backIdx, upIdx, downIdx, frontBackSpeed, upDownSpeed);
}
