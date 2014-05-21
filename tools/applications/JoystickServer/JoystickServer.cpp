#include <stdlib.h>

#include "JoystickStateBlockDistributer.h"
#include <inVRs/InputInterface/ControllerManager/UdpDevice.h>
#include <irrXML.h>
#include <vector>

#include <SDL/SDL.h>

OSG_USING_NAMESPACE

struct AXISMAPPING
{
	int devIdx;
	int contIdx;
	int minVal;
	int maxVal;
};

struct BUTTONMAPPING
{
	int devIdx;
	int contIdx;
};

using namespace irr;
using namespace io;

DEVICESTATE state;
JoystickStateBlockDistributer sender;
SDL_Joystick *joystick = NULL;
SDL_Surface *screen = NULL, *image = NULL;
SDL_Event event;
int jnum = 0;
std::vector<AXISMAPPING> axisMappingList;
std::vector<BUTTONMAPPING> buttonMappingList;
IrrXMLReader* xml = NULL;
int joyIdx = -1;
int numJoystickButtons = 0;
int numJoystickAxis = 0;
int networkChannel = UDPDEVICE_NETWORKCHANNEL;

bool init(char* cfgPath);
void cleanup();
void pollJoystickState();

int main(int argc, char** argv)
{
	int port;
	int localPort = 15123;

	if(argc < 4)
	{
		printf("usage: server <config-file> <client-ip> <client-port> [local-port]\n");
		return -1;
	} // if
	else if(argc == 5)
		localPort = atoi(argv[4]);

	memset(&state, 0, sizeof(DEVICESTATE));

	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) == -1)
	{
		printf("Can't init SDL:  %s\n", SDL_GetError());
		cleanup();
		exit(1);
	} // if

	if (!init(argv[1]))
	{
		printf("Can't load config-File %s\n", argv[1]);
		cleanup();
		return 0;
	} // if

	jnum = SDL_NumJoysticks(); // Weist der Ganzzahl Variable "jnum" die Anzahl der Joysticks zu
	if (jnum == 0)
	{
		printf("couldn't find any connected joystick!\n"); // Falls kein Joystick gefunden wurde
		cleanup();
		return 0;
	} // if

	joystick = SDL_JoystickOpen(joyIdx); // ffnet den ersten Joystick
	if (joystick == NULL)
	{
		printf("failed to open joystick with index %d\n", joyIdx);
		cleanup();
		return 0;
	} // if
	else
	{
		numJoystickButtons = SDL_JoystickNumButtons(joystick);
		numJoystickAxis = SDL_JoystickNumAxes(joystick);
		printf("\nname of device: %s", SDL_JoystickName(0));
		printf("\nnumber of axis: %i", SDL_JoystickNumAxes(joystick));
		printf("\nnumber of trackballs: %i", SDL_JoystickNumBalls(joystick));
		printf("\nnumber of buttons: %i\n", SDL_JoystickNumButtons(joystick));
	} // else

	screen = SDL_SetVideoMode(100, 100, 0, SDL_SWSURFACE | SDL_RESIZABLE);
   	if (screen == NULL)
   	{
    	printf("Can't set video mode: %s\n", SDL_GetError());
		cleanup();
       	exit(1);
   	} // if

	image = SDL_LoadBMP("joystick.bmp");
	if (image == NULL)
	{
		printf("Can't load backgroundimage: %s\n", SDL_GetError());
	} // if
	else
	{
		SDL_BlitSurface(image, NULL, screen, NULL);
		SDL_FreeSurface(image);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	} // else

	port = atoi(argv[3]);
	osgInit(argc, argv);

	sender.init(argv[2], (unsigned short)port, networkChannel, (unsigned short)localPort);

	bool done = false;
	bool stateChanged = false;
	while (!done)
	{
		stateChanged = false;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = true;
					break;

				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE)
						done = true;
					break;

				case SDL_JOYAXISMOTION: // Bei Bewegung des Joysticks
    			case SDL_JOYBUTTONDOWN: // Bei drcken einer Joysticktaste
    			case SDL_JOYBUTTONUP: 	// Beim loslassen einer Joysticktaste
					stateChanged = true;
					break;
			} // switch
		} // while
		if (stateChanged) pollJoystickState();
		sender.send();
		SDL_Delay(50);
	} // while
	cleanup();
	return -1;
}

bool init(char* cfgPath)
{
	xml = createIrrXMLReader(cfgPath);
	int udpDeviceIndex;
	int devIdx = 0;
	int contIdx = 0;
	int minVal = -1, maxVal = 1;
	AXISMAPPING axisMapping;
	BUTTONMAPPING buttonMapping;
	bool encounteredFirstJoystick = false;
	int maxAxisIndex = 0;
	int maxButtonIndex = 0;

	if(!xml) return false;

	while(xml && xml->read()){
		switch (xml -> getNodeType()){
			case EXN_TEXT:
				break;
			case EXN_ELEMENT:
				if (!strcmp("UDPDeviceIndex", xml->getNodeName()))
				{
					udpDeviceIndex = xml->getAttributeValueAsInt("value");
					if (udpDeviceIndex < 0)
					{
						printf("ERROR: udpDeviceIndex has unsupported value %i! Using index 0 instead!\n", udpDeviceIndex);
						udpDeviceIndex = 0;
					} // if
					networkChannel = UDPDEVICE_NETWORKCHANNEL - udpDeviceIndex;
				} // if
				else if(!strcmp("joystick", xml->getNodeName()))
				{
					if(encounteredFirstJoystick) return true;
					encounteredFirstJoystick = true;
					printf("parsing joystick settings ..\n");
					joyIdx = xml->getAttributeValueAsInt("index");

				} else if(!strcmp("axis", xml->getNodeName()))
				{
					devIdx = xml->getAttributeValueAsInt("devIdx");
					contIdx = xml->getAttributeValueAsInt("contIdx");
					minVal = xml->getAttributeValueAsInt("min");
					maxVal = xml->getAttributeValueAsInt("max");

					if (contIdx > maxAxisIndex)
						maxAxisIndex = contIdx;

					axisMapping.devIdx = devIdx;
					axisMapping.contIdx = contIdx;
					axisMapping.minVal = minVal;
					axisMapping.maxVal = maxVal;
					axisMappingList.push_back(axisMapping);
					if(contIdx >= UDPDEVICE_MAX_AXIS)
					{
						fprintf(stderr, "contIdx %d exceeds UDPDEVICE_MAX_AXIS (=%u)\n", contIdx, UDPDEVICE_MAX_AXIS);
					}
					printf("axis joystick %d, devIdx %d, contIdx %d, minVal %d, maxVal %d\n", joyIdx, devIdx, contIdx, minVal, maxVal);

				} else if(!strcmp("button", xml->getNodeName()))
				{
					devIdx = xml->getAttributeValueAsInt("devIdx");
					contIdx = xml->getAttributeValueAsInt("contIdx");

					if (contIdx > maxButtonIndex)
						maxButtonIndex = contIdx;

					buttonMapping.devIdx = devIdx;
					buttonMapping.contIdx = contIdx;
					buttonMappingList.push_back(buttonMapping);

					printf("button joystick %d, devIdx %d, contIdx %d\n", joyIdx, devIdx, contIdx);
				}
			case EXN_ELEMENT_END:
				break;
			default:
				break;
		} // switch
	} // while

	state.numButtons = maxButtonIndex+1;
	state.numAxes = maxAxisIndex+1;

	return true;
}

void cleanup()
{
	printf("CleanUp!\n");
	if(xml) delete xml;
	if (joystick) SDL_JoystickClose(joystick); // Schliet Joystick
	SDL_Quit();
}

void pollJoystickState()
{
	int i;
	AXISMAPPING axisMapping;
	BUTTONMAPPING buttonMapping;
	int* axisValue;

	axisValue = new int[numJoystickAxis];

	printf("dev:");
	for(i=0;i<numJoystickAxis;i++)
	{
		axisValue[i] = SDL_JoystickGetAxis(joystick, i);
		printf("\t%i", axisValue[i]);
	}
	printf("\n");

	for(i=0;i<(int)axisMappingList.size();i++)
	{
		axisMapping = axisMappingList[i];
		if(axisMapping.devIdx >= numJoystickAxis)
		{
			printf("axis mapping %d: index exceeds number of joystick axis\n", i);
		} else
		{
			state.axisValues[axisMapping.contIdx] = -1.0f +  2.0*((float)axisValue[axisMapping.devIdx]-(float)axisMapping.minVal)/(float)(axisMapping.maxVal - axisMapping.minVal);
			if(state.axisValues[axisMapping.contIdx]<-1.0) state.axisValues[axisMapping.contIdx] = -1.0;
			if(state.axisValues[axisMapping.contIdx]>1.0) state.axisValues[axisMapping.contIdx] = 1.0;
		} // else
	} // for

	for(i=0;i<(int)buttonMappingList.size();i++)
	{
		buttonMapping = buttonMappingList[i];
		if(buttonMapping.devIdx >= numJoystickButtons)
		{
			printf("button mapping %d: index exceeds number of joystick buttons\n", i);
		} else
		{
			state.buttonValues[buttonMapping.contIdx] = SDL_JoystickGetButton(joystick, i);
		} // else
	} // for
}

