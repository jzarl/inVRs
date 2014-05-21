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

#include "VirtualHandCursorModel.h"

#include "User.h"

VirtualHandCursorModel::VirtualHandCursorModel() {

} // VirtualHandCursorModel

std::string VirtualHandCursorModel::getName() {
	return "VirtualHandCursorModel";
} // getName

TransformationData VirtualHandCursorModel::generateCursorTransformation(
		TransformationData userTransformation, User* user) {
	TransformationData result;
	TransformationData userHandTransformation = user->getUserHandTransformation();
	multiply(result, userTransformation, userHandTransformation);
	return result;
} // generateCursorTransformation

CursorTransformationModel* VirtualHandCursorModelFactory::create(std::string className, void* args) {
	if (className != "VirtualHandCursorModel")
		return NULL;

	return new VirtualHandCursorModel;
} // create

