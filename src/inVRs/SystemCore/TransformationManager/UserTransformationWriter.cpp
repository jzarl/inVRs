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

#include "UserTransformationWriter.h"

#include <iostream>
#include <sstream>

#include <gmtl/Xforms.h>

#include "../WorldDatabase/WorldDatabase.h"
#include "../DataTypes.h"
#include "../DebugOutput.h"

using namespace std;

UserTransformationWriter::UserTransformationWriter(bool useLocalUser) {
	this->useLocalUser = useLocalUser;
} // UserTransformationWriter

TransformationData UserTransformationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	User* user;

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	// 	User* user = currentPipe->getOwner();
	//	printd("UserTransformationWriter::execute(): USER = %u:\n", user->getId());
	//  dumpTransformation(*resultLastStage);
	user->setNavigatedTransformation(*resultLastStage);

	return *resultLastStage;
} // execute

UserTransformationWriterFactory::UserTransformationWriterFactory() {
	className = "UserTransformationWriter";
}

TransformationModifier* UserTransformationWriterFactory::createInternal(ArgumentVector* args) {
	bool useLocalUser;

	if (args && args->get("useLocalUser", useLocalUser))
		return new UserTransformationWriter(useLocalUser);

	return new UserTransformationWriter();
} // create

bool UserTransformationWriterFactory::needInstanceForEachPipe() {
	return true;
}
