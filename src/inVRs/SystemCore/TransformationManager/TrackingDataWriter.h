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

#ifndef _TRACKINGDATAWRITER_H
#define _TRACKINGDATAWRITER_H

#include "../ComponentInterfaces/NetworkInterface.h"
#include "TransformationModifierFactory.h"

/**
 * This writer sets the tracking data for a local user. The type of tracking
 * sensor depends on the data used. Head or hand sensors are implemented and can
 * be configured at pipe setup. The writer is to be applied on a local user only
 * The input transformation is passed on.
 */
class INVRS_SYSTEMCORE_API TrackingDataWriter : public TransformationModifier {
//public:
//	enum SENSORTYPE {
//		SENSORTYPE_HEAD = 1, // should not intefere with default value for args in factory
//		SENSORTYPE_HAND = 2
//	};
//
//protected:
//	SENSORTYPE sensorType;
//
public:
//	TrackingDataWriter(TrackingDataWriter::SENSORTYPE sensorType);
	TrackingDataWriter();
	virtual std::string getClassName();
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
};

class INVRS_SYSTEMCORE_API TrackingDataWriterFactory : public TransformationModifierFactory {
public:

	TrackingDataWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needSingleton();
};

#endif // _TRACKINGDATAWRITER_H
