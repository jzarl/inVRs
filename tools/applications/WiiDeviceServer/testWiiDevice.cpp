#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DeviceStateSender.h"

#define NUM_BUTTONS		0 	
#define NUM_AXIS 		0
#define NUM_SENSORS 	1

#define MAX_BUFFER_SIZE (4+1+1+(NUM_BUTTONS+7)/8+1+(4*NUM_AXIS)+1+(7*4*NUM_SENSORS))

#define CLIENTPORT 8082
#define CLIENTCHANNEL 255

DeviceState deviceState;
DeviceStateSender sender;

void calc(struct Point *firstPoint, struct Point *secondPoint);
int initNetwork(char* hostname, int port);
void interpretInput(char c);

int main(int argc, char *argv[])
{
	char c;
	deviceState.numButtons = NUM_BUTTONS;
	deviceState.numAxis = NUM_AXIS;
	deviceState.numSensors = NUM_SENSORS;
	deviceState.axisValues = NULL;
	deviceState.buttonValues = NULL;
	deviceState.sensorValues = new SensorData[NUM_SENSORS];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s HOSTNAME\n\n", argv[0]);
		return 1;
	} // if

	if (initNetwork(argv[1], CLIENTPORT)) {
		fprintf(stderr, "Error at network initialization\n");
		return -1;
	} // if

	while (c=getchar()) {
		interpretInput(c);
		sender.send(&deviceState);
		fflush(stdin);
	} // while

	return 0;
}

int initNetwork(char* hostname, int port) {
	sender.init(hostname, port, CLIENTCHANNEL);
	return 0;
} // initNetwork

void interpretInput(char c)
{
	if (c == '4')
		deviceState.sensorValues[0].position[0] -= 0.5f;
	else if (c == '6')
		deviceState.sensorValues[0].position[0] += 0.5f;
	else if (c == '2')
		deviceState.sensorValues[0].position[1] -= 0.5f;
	else if (c == '8')
		deviceState.sensorValues[0].position[1] += 0.5f;
	else if (c == '0')
		deviceState.sensorValues[0].position[2] -= 0.5f;
	else if (c == '1')
		deviceState.sensorValues[0].position[2] += 0.5f;
} // interpretInput

