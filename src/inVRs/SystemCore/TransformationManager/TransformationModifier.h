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

#ifndef _TRANSFORMATIONMODIFIER_H
#define _TRANSFORMATIONMODIFIER_H

#include <string>

#include "TransformationPipe.h"

class TransformationModifierFactory;

/******************************************************************************
 * A TransformationModifier represents a stage in the TransformationPipe.
 * When the TransformationPipe is executed, it calls executeInternal() on
 * each stage in turn. Each stage gets the result of the previous stage, and
 * a reference to the TransformationPipe, based on which it calculates a new
 * result.
 *
 * A TransformationModifier is always created by its corresponding TransformationModifierFactory.
 *
 * When writing a TransformationModifier, the execute() method should be overloaded
 * to do the calculation described above. The TransformationPipe parameter can be used,
 * if the calculation needs results from previous executions as well (e.g. for calculating
 * the mean or for extrapolation).
 *
 * <h3>Naming conventions for TransformationModifiers</h3>
 *  - *Modifier: for modifiers without side-effects.
 *  - *Writer: for modifiers, with side-effects
 *  - *Interrupter: for modifiers interrupting the pipe-execution based on certain criteria.
 *
 * @see TransformationModifierFactory
 */
class INVRS_SYSTEMCORE_API TransformationModifier {
public:
	/**
	 * Possible behaviours, when executeInternal() is called by the TransformationPipe.
	 */
	enum OUTPUT {
		OUTPUT_PASSTHROUGH /** Call execute(), but ignore its return value (and use the result of the last stage instead). */,
		OUTPUT_RESULT
	/** Use the return value of execute(). */
	};

	/**
	 * Constructor.
	 * TransformationModifiers are only created by their respective factories.
	 */
	TransformationModifier();
	virtual ~TransformationModifier();

	/**
	 *
	 * This method is called by the TransformationModifierFactory upon
	 * creation of the TransformationModifier.
	 * \todo make this protected and make the factory class a friend?
	 */
	virtual void setFactory(TransformationModifierFactory* factory); // invoked by the factory during modifier creation
	
	/**
	 * Set the output mode to either <em>passthrough</em> or <em>result</em>.
	 */
	void setOutputMode(OUTPUT mode);
	
	/**
	 * The TransformationModifierFactory which created this instance.
	 */
	virtual TransformationModifierFactory* getFactory();
	
	/**
	 * The class-name of the TransformationModifier.
	 */
	virtual std::string getClassName();
	
	/**
	 * Calculate the new transformation.
	 * @param resultLastStage the result of the previous TransformationModifier
	 * @param currentPipe the Pipe, by which we are executed.
	 */
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	struct ManagerPrivateData {
		uint64_t anymaskPipeId;
		uint64_t testPipeId;
	};
	ManagerPrivateData managerPrivateData;
	OUTPUT outputMode;
	TransformationModifierFactory* factory;

	TransformationData executeInternal(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
	virtual bool interrupt(TransformationData* resultLastStage, TransformationPipe* currentPipe);

	friend class TransformationManager;
	friend class TransformationPipe;
};

#endif
