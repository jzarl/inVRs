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

#ifndef _TRANSFORMATIONMODIFIERFACTORY_H
#define _TRANSFORMATIONMODIFIERFACTORY_H

#include "../ArgumentVector.h"
#include "../ClassFactory.h"
#include "TransformationModifier.h"
#include "../DataTypes.h"

/**
 * The Factory-class for TransformationModifier.
 *
 * For each subclass of TransformationModifier, there has to be a
 * corresponding TransformationModifierFactory class. If you want to create
 * your own modifier you must therefore provide a factory class inheriting
 * from TransformationModifierFactory.
 *
 * When writing your own modifier class, the corresponding factory class needs to:
 *	- inherit from TransformationModifierFactory
 *	- set the className member accordingly in the constructor
 *	- override one of needInstanceForEachPipe(), needInstanceForEachPipeConfiguration(), needSingleton() (see remarks)
 *	- override createInternal()
 *
 * A factor has to be registered before loading the config file (i.e. before TransformationManager::loadModifier() is invoked).
 *
 * @see TransformationModifier
 */
class INVRS_SYSTEMCORE_API TransformationModifierFactory
{
public:
	/**
	 * Constructor.
	 * When inheriting, don't forget to set the className here!
	 */
	TransformationModifierFactory();
	
	/**
	 * Delete the factory itself, and all created TransformationModifer.
	 */
	virtual ~TransformationModifierFactory();

	/**
	 * Create (or reuse) a TransformationModifier matching the PipeSelector.
	 * If an existing modifier is reused, the ArgumentVector will be ignored.
	 *
	 * @param args contains parameters read from the Configuration file.
	 * @param sel the configuration of the TransformationPipe, in which the modifier will be executed.
	 *
	 * @see needInstanceForEachPipe()
	 * @see needInstanceForEachPipeConfiguration()
	 * @see needSingleton()
	 */
	virtual TransformationModifier* create(ArgumentVector* args, PipeSelector* sel);

	/**
	 * The class name of the TransformationModifier.
	 * <em>NOT</em> the class name of the factory!
	 * This is used by the TransformationModifier to know its own name.
	 */
	std::string getClassName();

	/**
	 * Mark the given TransformationModifier as no longer needed.
	 * Depending on the (re)usage pattern, the modifier may or may not be deleted
	 * when calling this function.
	 *
	 * @note Do <em>NOT</em> delete a modifier yourself. This will wreak havoc, if it is a
	 *       shared instance! (If it is not shared, you gain nothing by deleting it yourself and
	 *       not using this method.)
	 *
	 * @see ~TransformationModifierFactory
	 */
	virtual bool releaseModifier(TransformationModifier* modifier);

protected:
	/**
	 * Create a TransformationModifier instance.
	 * @param args can be used to pass configuration-data for the modifier.
	 *
	 * Here is a sample xml-snippet showing how to pass configuration-data for a modifier:
	 * \verbatim
<modifier type="ConfiguredTransformationModifier">
  <arguments>
    <arg key="intArg"    type="int"    value="1"/>
    <arg key="floatArg"  type="float"  value="0.125"/>
    <arg key="stringArg" type="string" value="a string"/>
  </arguments>
</modifier>
<modifier type="UnconfiguredTransformationModifier"/>
\endverbatim
	 */
	virtual TransformationModifier* createInternal(ArgumentVector* args) = 0;

	/**
	 * the following three methods specify when a separate instance the modifier has to be created
	 * by default each method returns false
	 * at least one of the methods has to be overriden and has to return true
	 */

	/// \todo make these three functions an enumeration:
	/// create a modifier instance for each pipe
	virtual bool needInstanceForEachPipe();
	/// create a modifier instance for each group of pipes with equal configuration
	virtual bool needInstanceForEachPipeConfiguration();
	/// create one global modifier instance
	virtual bool needSingleton();

	struct GROUPMODIFIER
	{
		TransformationModifier* modifier;
		PipeSelector sel;
	};

	std::string className;
	TransformationModifier* singleton;
	std::vector<GROUPMODIFIER> groupModifiersList;

}; // TransformationModifierFactory

#endif // _TRANSFORMATIONMODIFIERFACTORY_H
