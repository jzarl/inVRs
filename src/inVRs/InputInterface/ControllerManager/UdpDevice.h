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

#ifndef _UDPDEVICE_H
#define _UDPDEVICE_H

#include <vector>

#include "InputDeviceBase.h"
#include "../../SystemCore/NetMessage.h"
#include "../../SystemCore/ComponentInterfaces/NetworkInterface.h"

#define UDPDEVICE_MAX_SENSORS		32
#define UDPDEVICE_MAX_AXIS 			64
#define UDPDEVICE_MAX_BUTTONS		256
#define UDPDEVICE_NETWORKCHANNEL	255

/**
 * message protocol:
 * 		UInt8 numButtons
 *		UInt8* buttonValues (one UInt8 containts 8 buttonState-bits)
 *		UInt8 numAxes
 *		Real32* axisValues
 *		UInt8 numSensors
 *		SensorData* sensorValues
 */
struct DEVICESTATE {
	unsigned  numButtons;
	bool buttonValues[UDPDEVICE_MAX_BUTTONS];
	unsigned numAxes;
	float axisValues[UDPDEVICE_MAX_AXIS];
	unsigned numSensors;
	SensorData sensorValues[UDPDEVICE_MAX_SENSORS];
};

/******************************************************************************
 * InputDevice for receiving input data via UDP messages over the network.
 * This class receives network messages with the message protocol defined by
 * the DEVICESTATE struct and decodes it into button, axis and sensor values.
 */
class CONTROLLERMANAGER_API UdpDevice : public InputDeviceBase {
public:

	/**
	 * Constructor
	 */
	UdpDevice(unsigned networkChannel = UDPDEVICE_NETWORKCHANNEL);

	/**
	 * Destructor
	 */
	virtual ~UdpDevice();

	/**
	 * Updates the input data. This method requests all incoming network
	 * messages from the network module and decodes the device states.
	 */
	virtual void update();

protected:

	/**
	 * Decodes the device state from the passed NetMessage
	 */
	static void decodeStateFromBinaryMessage(DEVICESTATE* deviceState, NetMessage* msg);

	/**
	 * Helper-method for decoding the buttons state from the message
	 */
	static void decodeButtonState(DEVICESTATE* deviceState, uint8_t* buttonData, uint8_t numButtons);

	unsigned networkChannel;	// network channel on which the UDP-packets arrive (must match with sender)
	NetworkInterface* netInt;	// pointer to the network module
}; // UdpDevice

/******************************************************************************
 * Factory class for the UdpDevice. This class creates a UdpDevice with the
 * following optional arguments:
 *
 * networkChannel .. network channel where the UDP-packets arrive (must match
 *                   with the channel defined at the sender). Take care to not
 *                   conflict with other network channels (like used by modules
 *                   = MODULE-ID or by main application = 0). The default value
 *                   for the networkChannel is 255
 */
class UdpDeviceFactory : public InputDeviceFactory {
public:
	/**
	 * Destructor
	 */
	virtual ~UdpDeviceFactory(){};

	/**
	 * Creates a new UdpDevice if the className matches
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // UdpDeviceFactory


#endif // _UDPDEVICE_H
