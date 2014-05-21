#include <stdio.h>
#include <stdlib.h>

#include "DeviceStateSender.h"
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#define WII

DeviceStateSender sender;

void initNetwork() {
	sender.init("localhost", 15123, 0);
} // initNetwork

void runTest01(int argc, char** argv) {
	gmtl::AxisAnglef axAng;

#ifdef WII
	DeviceState deviceState;
#else
	DEVICESTATE deviceState;
#endif
	deviceState.numButtons = 10;
	deviceState.numAxis = 3;
	deviceState.numSensors = 2;
#ifdef WII
	deviceState.buttonValues = new bool[deviceState.numButtons];
	deviceState.axisValues = new float[deviceState.numAxis];
	deviceState.sensorValues = new SensorData[deviceState.numSensors];
#endif
	deviceState.buttonValues[0] = 1;
	deviceState.buttonValues[1] = 0;
	deviceState.buttonValues[2] = 0;
	deviceState.buttonValues[3] = 1;
	deviceState.buttonValues[4] = 1;
	deviceState.buttonValues[5] = 1;
	deviceState.buttonValues[6] = 1;
	deviceState.buttonValues[7] = 0;
	deviceState.buttonValues[8] = 1;
	deviceState.buttonValues[9] = 0;

	deviceState.axisValues[0] = 3.1415f;
	deviceState.axisValues[1] = -4.234;
	deviceState.axisValues[2] = -111111.f;
	
	deviceState.sensorValues[0].position = gmtl::Vec3f(-25.3, 12.5, 0);
	axAng = gmtl::AxisAnglef(M_PI/2, gmtl::Vec3f(1, 0, 0));
	gmtl::set(deviceState.sensorValues[0].orientation, axAng);

	deviceState.sensorValues[1].position = gmtl::Vec3f(3158.12535, 0.0001, -23.342);
	axAng = gmtl::AxisAnglef(M_PI/2, gmtl::Vec3f(1, 0, 0));
	gmtl::set(deviceState.sensorValues[0].orientation, axAng);

#ifdef WII
	FILE* file = fopen("runTest01_wii.log", "w");
#else
	FILE* file = fopen("runTest01_std.log", "w");
#endif
		
	sender.dump(&deviceState, file);
	
	fclose(file);
} // runTest01

int main(int argc, char** argv) {
	initNetwork();
	runTest01(argc, argv);
	return 0;
} // main
