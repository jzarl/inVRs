#ifndef _DEVICESTATESENDER_H
#define _DEVICESTATESENDER_H
#include <stdio.h>
#include <stdlib.h>
#include <gmtl/VecOps.h>
#include <gmtl/QuatOps.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>

#include <OpenSG/OSGThreadManager.h>

struct SensorData
{
	gmtl::Quatf orientation;
	gmtl::Vec3f position;
};

struct DeviceState
{
	unsigned  numButtons;
	bool* buttonValues;
	unsigned numAxis;
	float* axisValues;
	unsigned numSensors;
	SensorData* sensorValues;
};

class DeviceStateSender
{
public:

	DeviceStateSender();
	~DeviceStateSender();

	bool init(const char* hostname, unsigned short port, unsigned channelIdx);

	void send(DeviceState* state);
	void dump(DeviceState* state, FILE* file);

protected:

	unsigned char* generateMessage(DeviceState* src, unsigned& bufferSize);


	void putUInt32(unsigned char* &dst, unsigned value);
	void putUInt8(unsigned char* &dst, unsigned char value);
	void putReal32(unsigned char* &dst, float value);

	bool isConfigured;
	std::string hostname;
	unsigned short port;
	unsigned char channelIdx;

	int netsocket;  /* file descriptor for network connection */
	struct sockaddr_in serverAddr;

};


#endif
