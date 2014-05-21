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

#include <assert.h>

#include "TransformationModifierFactory.h"
#include "../DebugOutput.h"

TransformationModifierFactory::TransformationModifierFactory() {
	className = "nameless_modifier!!!";
	singleton = NULL;
}

TransformationModifierFactory::~TransformationModifierFactory() {
	int i;
	printd(INFO, "TransformationModifierFactory destructor: removing factory %s\n",
			className.c_str());

	if (singleton)
		delete singleton;
	for (i = 0; i < (int)groupModifiersList.size(); i++) {
		delete groupModifiersList[i].modifier;
	}
}

TransformationModifier* TransformationModifierFactory::create(ArgumentVector* args,
		PipeSelector* sel) {
	TransformationModifier* ret;

	if (needInstanceForEachPipe()) {
		printd(INFO, "TransformationModifier::create(): creating new instance of modifier %s\n",
				className.c_str());
		ret = createInternal(args);
		assert(ret);
		ret->setFactory(this);
		return ret;
	} else if (needSingleton()) {
		if (!singleton) {
			singleton = createInternal(args);
			assert(singleton);
			singleton->setFactory(this);
			printd(INFO, "TransformationModifier::create(): creating singleton modifier %s\n",
					className.c_str());
		}
		return singleton;
	} else if (needInstanceForEachPipeConfiguration()) {
		int i;
		GROUPMODIFIER groupModifier;
		for (i = 0; i < (int)groupModifiersList.size(); i++) {
			if (groupModifiersList[i].sel == *sel) {
				return groupModifiersList[i].modifier;
			}
		}

		printd(INFO,
				"TransformationModifier::create(): creating new group instance of modifier %s\n",
				className.c_str());
		groupModifier.modifier = createInternal(args);
		assert(groupModifier.modifier != NULL);
		groupModifier.modifier->setFactory(this);
		groupModifier.sel = *sel;
		groupModifiersList.push_back(groupModifier);
		return groupModifier.modifier;
	} else {
		assert(false);
		// one of needInstanceForEachPipe(), needSingleton, needInstanceForEachPipeConfiguration()
		// must return true;
	}

	return NULL;
}

std::string TransformationModifierFactory::getClassName() {
	assert(this->className != std::string("nameless_modifier!!!")); // forgot to set className in derived constructor

	return className;
}

bool TransformationModifierFactory::releaseModifier(TransformationModifier* modifier) {
	if (needInstanceForEachPipe()) {
		delete modifier;
	} else if (needSingleton() || needInstanceForEachPipeConfiguration()) {
		// delete modifiers when factory is being deleted
		// 2008-04-23 ZaJ: maybe delete here, when the last instance is released (needs reference counting)
	} else {
		assert(false);
	}

	return true; // 2008-04-23 ZaJ: maybe use return value to tell whether something has been deleted?
}

bool TransformationModifierFactory::needInstanceForEachPipe() {
	return false;
}

bool TransformationModifierFactory::needInstanceForEachPipeConfiguration() {
	return false;
}

bool TransformationModifierFactory::needSingleton() {
	return false;
}
