#ifndef _JOYSTICKSTATEBLOCKDISTRIBUTER_H
#define _JOYSTICKSTATEBLOCKDISTRIBUTER_H

#include <stdio.h>
#include <stdlib.h>

#include <OpenSG/OSGSocket.h>
#include <OpenSG/OSGStreamSocket.h>
#include <OpenSG/OSGDgramSocket.h>
#include <OpenSG/OSGSocketAddress.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGBinaryMessage.h>

#include <inVRs/InputInterface/ControllerManager/UdpDevice.h>


class JoystickStateBlockDistributer
{
	public:

	JoystickStateBlockDistributer();
	~JoystickStateBlockDistributer();

	bool init(char* serverIP, unsigned short port, unsigned channelIdx, unsigned short localPort = 15123);

	void send();

	protected:

	void encodeMessage(DEVICESTATE* src, OSG::BinaryMessage& dst);

	bool bStarted;
	unsigned short port;
	unsigned channelIdx;
	char* ip;
	OSG::DgramSocket server;

};


#endif
