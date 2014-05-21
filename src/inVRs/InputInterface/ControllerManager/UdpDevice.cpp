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

#include "UdpDevice.h"

//#include <assert.h>
//#include <memory.h>

//#include "SystemCore/UserDatabase/User.h"
//#include "SystemCore/UserDatabase/UserDatabase.h"
//#include "SystemCore/ComponentInterfaces/NetworkInterface.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/SystemCore.h"
#include "../../SystemCore/MessageFunctions.h"

UdpDevice::UdpDevice(unsigned networkChannel) :
	networkChannel(networkChannel),
	netInt(NULL) {

	nButtons = UDPDEVICE_MAX_BUTTONS;
	nAxes = UDPDEVICE_MAX_AXIS;
	nSensors = UDPDEVICE_MAX_SENSORS;

	buttonState = new int[nButtons];
	axisState = new float[nAxes];
	sensorState = new SensorData[nSensors];

	memset(buttonState, 0, sizeof(int) * nButtons);
	memset(axisState, 0, sizeof(float) * nAxes);
	memset(sensorState, 0, sizeof(SensorData) * nSensors);
} // UdpDevice

UdpDevice::~UdpDevice() {
	delete[] buttonState;
	delete[] axisState;
	delete[] sensorState;
} // ~UdpDevice

void UdpDevice::update() {
	std::vector<NetMessage*> deviceStatesList;
	DEVICESTATE deviceState;
	NetMessage* message;
	int i;

	memset(&deviceState, 0, sizeof(deviceState));

	if (!netInt)
		netInt = (NetworkInterface*)SystemCore::getModuleByName("Network");

	if (!netInt) {
		printd("UdpDevice::update(): failed to retrieve network interface!\n");
		return;
	}

	netInt->popAll(networkChannel, &deviceStatesList);
	if (deviceStatesList.size() == 0)
		return;

	message = deviceStatesList[deviceStatesList.size() - 1];
	decodeStateFromBinaryMessage(&deviceState, message);

	for (i = 0; i < nButtons; i++) {
		if (buttonState[i] != deviceState.buttonValues[i]) {
			buttonState[i] = deviceState.buttonValues[i];
			sendButtonChangeNotification(i, buttonState[i]);
		} // if
	} // for

	for (i = 0; i < nAxes; i++)
		axisState[i] = deviceState.axisValues[i];

	for (i = 0; i < nSensors; i++)
		sensorState[i] = deviceState.sensorValues[i];

	for (i = 0; i < (int)deviceStatesList.size(); i++)
		delete deviceStatesList[i];
} // update

void UdpDevice::decodeStateFromBinaryMessage(DEVICESTATE* deviceState, NetMessage* msg) {
	int i;
	uint8_t numButtons, numAxes, numSensors;
	uint8_t numBytes;
	uint8_t *buttonData;

	msg->getUInt8(numButtons);
	if (numButtons > 0)
		numBytes = ((numButtons - 1) / 8) + 1;
	else
		numBytes = 0;

	if (numBytes > 0) {
		buttonData = new uint8_t[numBytes];
		for (i = 0; i < numBytes; i++) {
			msg->getUInt8(buttonData[i]);
		} // for
		decodeButtonState(deviceState, buttonData, numButtons);
		delete buttonData;
	} // if

	msg->getUInt8(numAxes);
	for (int i = 0; i < numAxes; i++) {
		msg->getReal32(deviceState->axisValues[i]);
	} // for

	msg->getUInt8(numSensors);
	for (i = 0; i < numSensors; i++) {
		msgFunctions::decode(deviceState->sensorValues[i], msg);
	} // for
} // decodeStateFromBinaryMessage

void UdpDevice::decodeButtonState(DEVICESTATE* deviceState, uint8_t* buttonData, uint8_t numButtons) {
	unsigned buttonNumber;
	unsigned dataIndex;
	unsigned byteIndex;
	uint8_t mask;

	for (buttonNumber = 0; buttonNumber < numButtons; buttonNumber++) {
		dataIndex = buttonNumber / 8;
		byteIndex = buttonNumber % 8;
		mask = 1 << byteIndex;
		deviceState->buttonValues[buttonNumber] = buttonData[dataIndex] & mask;
	} // for
} // decodeButtonState


InputDevice* UdpDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "UdpDevice")
		return NULL;

	unsigned networkChannel = UDPDEVICE_NETWORKCHANNEL;

	if (args && args->keyExists("networkChannel")) {
		args->get("networkChannel", networkChannel);
	} // if
	return new UdpDevice(networkChannel);
} // create
