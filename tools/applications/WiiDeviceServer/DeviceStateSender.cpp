#include "DeviceStateSender.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

DeviceStateSender::DeviceStateSender()
{
	isConfigured = false;
	netsocket = 0;
} // DeviceStateSender

DeviceStateSender::~DeviceStateSender()
{
	close(netsocket);
} // ~DeviceStateSender

bool DeviceStateSender::init(const char* hostname, unsigned short port, unsigned channelIdx)
{
	this->port = port;
	this->hostname = hostname;
	this->channelIdx = channelIdx;

    struct hostent *h;

	h = (struct hostent *) gethostbyname(hostname);

    if(h == NULL) {
        fprintf(stderr, "Unknown host: %s\n", hostname);
        return false;
    } // if

    serverAddr.sin_family = h->h_addrtype;
    memcpy((char *) &serverAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    serverAddr.sin_port = htons(port);

	if ((netsocket=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "Socket Error!!!!");
		return false;
	} // if

	isConfigured = true;

	return true;
} // init

void DeviceStateSender::dump(DeviceState* state, FILE* file)
{
	unsigned i;
	unsigned messageSize;
	unsigned char* message;

	message = generateMessage(state, messageSize);
	fprintf(stderr, "MessageSize = %u\n", messageSize);
	fprintf(file, "\nDeviceStateSender::dump():\n");
	fprintf(file, "----------------------------\n");
	for (i=0; i < messageSize; i++)
	{
		fprintf(file, "%i\n", message[i]);
	} // for
	fprintf(file, "----------------------------\n\n");
	delete message;
} // dump

void DeviceStateSender::send(DeviceState* state)
{
	unsigned messageSize = 0;
	unsigned char* message = NULL;

	if (!isConfigured)
	{
		printf("Server not initialized!\n");
		return;
	} // if

	assert(state);
	message = generateMessage(state, messageSize);
	sendto(netsocket, message, messageSize, 0, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr));
	delete message;
} // send

void DeviceStateSender::putUInt32(unsigned char* &dst, unsigned value)
{
	int i;
	unsigned char* pointer;
	unsigned netValue = htonl(value);

	pointer = (unsigned char*)&netValue;
	for (i=0; i < 4; i++)
	{
		*dst = *pointer;
		dst++;
		pointer++;
	} // for
} // putUInt32

void DeviceStateSender::putUInt8(unsigned char* &dst, unsigned char value)
{
	*dst = value;
	dst++;
} // putUInt8

void DeviceStateSender::putReal32(unsigned char* &dst, float value)
{
	int i;
	unsigned char* pointer;
	uint32_t tempValue = 0;
	uint32_t netValue = 0;
	tempValue = *((uint32_t*)&value);
	netValue = htonl(tempValue);

	pointer = (unsigned char*)&netValue;
	for (i=0; i < 4; i++)
	{
		*dst = *pointer;
		dst++;
		pointer++;
	} // for
} // putReal32

unsigned char* DeviceStateSender::generateMessage(DeviceState* state, unsigned& messageSize)
{
	unsigned char* message;
	unsigned char* messagePointer;
	char buttonData;
	char mask;
	unsigned i;
	assert(state);

	// 4 Bytes ... Message size
	// 4 Bytes ... Normal message tag
	// 1 Byte  ... Network channel
	// 1 Byte  ... Number of Buttons
	// 1 Byte per 8 Buttons
	// 1 Byte  ... Number of Axis
	// 4 Byte per 1 Axis
	// 1 Byte  ... Number of Sensors
	// 7 * 4 Byte per 1 Sensor (7 float values)
	messageSize = 4+4+1+1+(state->numButtons+7)/8+1+(4*state->numAxis)+1+(7*4*state->numSensors);

	fprintf(stderr, "MessageSize = %u\n", messageSize);

	message = new unsigned char[messageSize];
	messagePointer = message;


// WRITE MESSAGE SIZE
	putUInt32(messagePointer, messageSize);

//	dst.putUInt32(0);	// normal msg tag
	putUInt32(messagePointer, 0);
//	dst.putUInt8(networkChannel);
	putUInt8(messagePointer, channelIdx);

//	dst.putUInt8(src->numButtons);
	putUInt8(messagePointer, state->numButtons);
	buttonData = 0;
	for (i=0; i < state->numButtons; i++)
	{
		if (state->buttonValues[i])
		{
			mask = 1 << (i%8);
			buttonData = buttonData | mask;
		} // if
		if ( (i+1)%8 == 0 || i == (state->numButtons-1))
		{
//			dst.putUInt8(buttonData);
			putUInt8(messagePointer, buttonData);
			buttonData = 0;
		} // if
	} // for

//	dst.putUInt8(src->numAxis);
	putUInt8(messagePointer, state->numAxis);
	for (i=0; i < state->numAxis; i++)
	{
		float axisValue = state->axisValues[i];
		printf("%f(%i) ",axisValue, i);
		putReal32(messagePointer, axisValue);
	} // for
	printf("\n");

//	dst.putUInt8(src->numSensors);
	putUInt8(messagePointer, state->numSensors);
	for (i=0;  i < state->numSensors; i++)
	{
		putReal32(messagePointer, state->sensorValues[i].position[0]);
		putReal32(messagePointer, state->sensorValues[i].position[1]);
		putReal32(messagePointer, state->sensorValues[i].position[2]);
		putReal32(messagePointer, state->sensorValues[i].orientation[0]);
		putReal32(messagePointer, state->sensorValues[i].orientation[1]);
		putReal32(messagePointer, state->sensorValues[i].orientation[2]);
		putReal32(messagePointer, state->sensorValues[i].orientation[3]);
	} // for

	assert(messageSize == (unsigned)(messagePointer - message));
	return message;
} // encodeMessage
