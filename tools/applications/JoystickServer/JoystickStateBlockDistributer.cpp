#include "JoystickStateBlockDistributer.h"
#include <stdio.h>
#include <assert.h>

OSG_USING_NAMESPACE

extern DEVICESTATE state;
extern int networkChannel;

extern bool volatile* bShouldQuit;
extern bool volatile debugOutput;

JoystickStateBlockDistributer::JoystickStateBlockDistributer()
{
	bStarted = false;
}

JoystickStateBlockDistributer::~JoystickStateBlockDistributer()
{
	server.close();
}

bool JoystickStateBlockDistributer::init(char* serverIP, unsigned short port, unsigned channelIdx, unsigned short localPort)
{

	this->port = port;
	ip = serverIP;
	this->channelIdx = channelIdx;

	try
	{
		server.open();
		server.bind(SocketAddress(SocketAddress::ANY, localPort));
		bStarted = true;
	} catch(SocketException &e)
	{
		printf("JoystickStateBlockThread.init(): network error: %s\n", e.what());
		return false;
	}

	return true;
}

void JoystickStateBlockDistributer::send()
{
	BinaryMessage binMsg;

	if (!bStarted)
	{
		printf("Server not initialized!\n");
		return;
	}

	encodeMessage(&state, binMsg);

	try
	{
		server.sendTo(binMsg, SocketAddress(ip, port));
//		printf("JoystickStateBlockThread::run(): successfully send a packet!\n");
	} catch(SocketException &e)
	{
		printf("JoystickStateBlockThread.run(): network error: %s\n", e.what());
	}

}

void JoystickStateBlockDistributer::encodeMessage(DEVICESTATE* src, BinaryMessage& dst)
{
	int i;
	UInt8 buttonData;
	UInt8 mask;

	dst.putUInt32(0);	// normal msg tag
	dst.putUInt8(networkChannel);

	dst.putUInt8(src->numButtons);
	buttonData = 0;
	for (i=0; i < (int)src->numButtons; i++)
	{
		if (src->buttonValues[i])
		{
			mask = 1 << (i%8);
			buttonData = buttonData | mask;
		} // if
		if ( (i+1)%8 == 0 || i == (((int)src->numButtons)-1))
		{
			dst.putUInt8(buttonData);
			buttonData = 0;
		} // if
	} // for

	dst.putUInt8(src->numAxes);
	for (i=0; i < (int)src->numAxes; i++)
	{
		dst.putReal32(src->axisValues[i]);
	}

	dst.putUInt8(src->numSensors);
	assert(src->numSensors == 0);
	// No sensordata here to sync!
} // encodeMessage

