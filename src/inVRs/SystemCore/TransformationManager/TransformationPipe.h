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

#ifndef _TRANSFORMATIONPIPE_H
#define _TRANSFORMATIONPIPE_H

#include <deque>

#include "../UserDatabase/UserDatabase.h"
#include "../NetMessage.h"

class TransformationModifier;
class TransformationMerger;

/**
 * Timestamped version of TransformationData.
 */
struct TransformationPipeData {
	TransformationData transf;
	unsigned timestamp;
};

/******************************************************************************
 * A TransformationPipe applies a number of operations on TransformationData.
 * There are two kinds of operations:
 *  - (zero or more) TransformationModifiers, and
 *  - (one or zero) TransformationMerger
 *
 * Upon executing the pipe, a TransformationData will first run through
 * the TransformationModifiers (aka stages), until it reaches the position
 * of the TransformationMerger. The merger is then executed.
 * If there are modifiers/stages after the merger, they are only executed upon
 * calling executeAfterMerger().
 *
 * @see TransformationModifier
 * @see TransformationMerger
 * \todo review interface of this class (maybe more stuff can be moved into the protected area?
 */
class INVRS_SYSTEMCORE_API TransformationPipe {
public:
	/**
	 * @see TransformationManager::packPipeId
	 * @see TransformationManager::unpackPipeId
	 * \todo make this compatible with the pipeId, so (un)packPipeId is not needed for conversion
	 */
	union MASK {
		struct {
			unsigned srcId;
			unsigned dstId;
			unsigned pipeType;
			unsigned objectClass;
			unsigned objectType;
			unsigned objectId;
			unsigned bFromNetwork;
		} individual;
		unsigned asArray[7];
	};

	/**
	 * Create a new TransformationPipe.
	 * @param pipeId, as calculated by TransformationManager::packPipeId
	 * @param owner the User owning the pipe.
	 */
	TransformationPipe(uint64_t pipeId, User* owner);
	virtual ~TransformationPipe();

	/**
	 * Put a TransformationData into the pipe.
	 * The TransformationData is wrapped in a TransformationPipeData, with
	 * the timestamp of the push_back operation.
	 */
	virtual void push_back(TransformationData& data);

	/**
	 * The number of TransformationDatas in the pipe.
	 */
	virtual int size();

	/**
	 * Direct access to a TransformationData by its index in the deque.
	 */
	virtual TransformationData getTransformation(unsigned index);

	/**
	 * Flush the TransformationPipe according to its flush strategy.
	 */
	virtual void flush();

	/**
	 * Add a new modifier as final stage.
	 */
	void addStage(TransformationModifier* stage);

	/**
	 * Set a merger for the pipe.
	 * Any stages added after this will only be executed, if executeAfterMerger() is called.
	 */
	bool setMerger(TransformationMerger* merger);

	/**
	 * Execute all stages up to the merger (if set).
	 * Stages after the merger won't be executed.
	 */
	TransformationData execute();

	/**
	 * Execute the remaining stages, after execute() has already been called.
	 * @param lastResult the TransformationData returned by execute().
	 */
	TransformationData executeAfterMerger(TransformationData lastResult);

	/**
	 * Return the owner of the pipe.
	 */
	User* getOwner();

	/**
	 * The pipe id.
	 * @see TransformationManager::packPipeId()
	 * @see TransformationManager::unpackPipeId()
	 */
	uint64_t getPipeId();

	/**
	 * The pipe priority.
	 * The TransformationManager executes pipes with a higher priority first.
	 */
	unsigned getPriority();

	/**
	 * Set the desired execution interval and time-offset of the pipe.
	 * The TransformationManager will try to execute each pipe approximately once per execution interval.
	 * @param sec mean time (in seconds) between execution.
	 * \todo remove offset, because it isn't honored by TransformationManager::execute()
	 */
	void setExecutionInterval(float sec, float offset = 0.0f);

	/**
	 * The execution interval.
	 * @see setExecutionInterval()
	 */
	float getExecutionInterval();

protected:
	/**
	 * THis determines, how many/which entries are removed from the pipe, if flush() is called.
	 */
	enum FLUSHSTRATEGY {
		FLUSHSTRATEGY_QUORUM /** remove TransformationData, until size() <= flushParam (default: 2) */,
		FLUSHSTRATEGY_TIMEOUT
	/** not implemented yet. */
	};

	unsigned priority; // determines execution order (higher value means higher priority)
	User* owner;
	unsigned flushParam;
	FLUSHSTRATEGY flushStrategy;
	std::deque<TransformationPipeData> transformationData;
	std::vector<TransformationModifier*> stages;
	uint64_t pipeId; // built from several properties:
	float executionInterval;
	float timeToNextExecution;
	TransformationMerger* merger;
	int mergerIndex;
	void setFlushStrategy(FLUSHSTRATEGY stratetgy, unsigned param);
	/**
	 * current layout (order of bit significance:)
	 * bit: (x-X is both inclusive)
	 * 0	msg received by network
	 * 1-7	srcId (7 bit)
	 * 8	unused
	 * 9-15	dstId (7 bit)
	 * 16-23	pipe type (8 bit)
	 * 24-31	object class (8 bit)
	 * 32-47	object type (16 bit)
	 * 48-63	object id (16 bit)
	 */

	friend class TransformationManager;
};

/******************************************************************************
 * A PipeSelector allows matching a pipeId with a selection mask.
 * @see TransformationManager::packPipeId()
 * @see TransformationPipe::MASK
 * XXX \todo write documentation for member-functions
 */
class INVRS_SYSTEMCORE_API PipeSelector {
public:
	uint64_t anyPipeMask;
	uint64_t testPipeId;

	bool selects(uint64_t pipeId) {
		return (anyPipeMask & pipeId) == testPipeId;
	}

	bool operator==(PipeSelector& other) {
		return (testPipeId == other.testPipeId) && (anyPipeMask == other.anyPipeMask);
	}

	/**
	 * Initialise the PipeSelector with a selection mask.
	 *\todo make this a constructor!
	 */
	void setup(TransformationPipe::MASK* mask);
};

INVRS_SYSTEMCORE_API std::string getUInt64AsString(uint64_t src);


#endif
