#include "StrangeDevice.h"

#include <assert.h>
#include <memory.h>
#include "../../SystemCore/UserDatabase/User.h"
#include "../../SystemCore/UserDatabase/UserDatabase.h"
#include "../../SystemCore/ComponentInterfaces/NetworkInterface.h"
#include "../../SystemCore/DebugOutput.h"
#include "../../SystemCore/SystemCore.h"
#include "../../SystemCore/MessageFunctions.h"
#include "../../SystemCore/Configuration.h"


InputDevice* StrangeDeviceFactory::create(std::string className, ArgumentVector* args)
{
	if (className != "StrangeDevice")
			return NULL;

		return new StrangeDevice;
} //create

StrangeDevice::StrangeDevice() :
yawScale(1.f),
yawOffset(0.f),
pitchScale(1.f),
pitchOffset(0.f)
{
		nAxes = 5;
		nButtons = 1;
		nSensors = 2;

		axisState = new float[nAxes];
		buttonState = new int[nButtons];
		sensorState = new SensorData[nSensors];

		memset(axisState, 0, sizeof(float)*nAxes);
		memset(buttonState, 0, sizeof(int)*nButtons);
		memset(sensorState, 0, sizeof(SensorData)*nSensors);

		netInt = NULL;

		if(Configuration::contains("StrangeDevice.yawScale"))
		{
			yawScale = Configuration::getFloat("StrangeDevice.yawScale");
		}

		if(Configuration::contains("StrangeDevice.yawOffset"))
		{
			yawOffset = Configuration::getFloat("StrangeDevice.yawOffset");
		}

		if(Configuration::contains("StrangeDevice.pitchScale"))
		{
			pitchScale = Configuration::getFloat("StrangeDevice.pitchScale");
		}

		if(Configuration::contains("StrangeDevice.pitchOffset"))
		{
			pitchOffset = Configuration::getFloat("StrangeDevice.pitchOffset");
		}
}

StrangeDevice::~StrangeDevice()
{

}
void StrangeDevice::update()
{
	{
		std::vector<NetMessage*> deviceStatesList;
	// 	User* localUser = UserDatabase::getLocalUser();

		DEVICESTATE deviceState;
		NetMessage* lastMsg;
		int i;

		memset(&deviceState, 0, sizeof(deviceState));

	 //	printd(INFO, "UDPDevice::update(): entering method\n");

	// 	assert(localUser);
		if (!netInt)
			netInt = (NetworkInterface*)SystemCore::getModuleByName("Network");

		if(!netInt)
		{
			printd("StrangeDevice::update(): failed to retrieve network interface!\n");
			return;
		}

		netInt->popAll(networkChannel, &deviceStatesList);
		if(deviceStatesList.size()==0) return;

		printd(INFO, "UDPDevice::update(): found network message!\n");

		printd(INFO, "strange device ---------- update\n");

		lastMsg = deviceStatesList[deviceStatesList.size()-1];
		decodeStateFromBinaryMessage(&deviceState, lastMsg);

		if (deviceState.buttonValues[0] != buttonState[0]) {
			buttonState[0] = deviceState.buttonValues[0];
			sendButtonChangeNotification(0, buttonState[0]);
		} //if

		//if(nAxes != 5)
		//	printd(WARNING, "StrangeDevice::update(): number of strange device's axis is not 5!");

		for(i=0;i<nAxes;i++)
			axisState[i] = deviceState.axisValues[i];

		//todo: calculate sensorValue from axisValues 4 and 5
		SensorData sensorDat;

		float yaw = (float)(deviceState.axisValues[3] + yawOffset) * yawScale * M_PI * -0.5f;
		float pitch = (float)(deviceState.axisValues[4] + pitchOffset) * pitchScale * M_PI * -0.5f;

		gmtl::Matrix44f yawMat = gmtl::make<gmtl::Matrix44f>(gmtl::AxisAnglef(yaw, 0.0f, 1.0f, 0.0f));
		gmtl::Matrix44f pitchMat = gmtl::make<gmtl::Matrix44f>(gmtl::AxisAnglef(pitch, 1.0f, 0.0f, 0.0f));

		gmtl::Matrix44f fullTrans;
		gmtl::identity(fullTrans);

		fullTrans = yawMat * pitchMat;

		gmtl::set(sensorDat.orientation, fullTrans);


		printd(INFO, "angles: %f, %f, %f\n", sensorDat.orientation[0], sensorDat.orientation[1], sensorDat.orientation[2]);
		sensorDat.position = gmtl::Vec3f(0,0,-8);

		//sensorDat.orientation =
		sensorState[1] = sensorDat;

		/**for(i=0;i<nSensors;i++)
			//todo: calculate sensorValue from axisValues 4 and 5
			sensorState[i] = deviceState.sensorValues[i];**/

		for (i=0; i < (int)deviceStatesList.size(); i++)
			delete deviceStatesList[i];
	} // update
}

void StrangeDevice::setYawScale(float scale) {
	yawScale = scale;
} // setYawScale

void StrangeDevice::setYawOffset(float offset) {
	yawOffset = offset;
} // setYawOffset

void StrangeDevice::setPitchScale(float scale) {
	pitchScale = scale;
} // setPitchScale

void StrangeDevice::setPitchOffset(float offset) {
	pitchOffset = offset;
} // setPitchOffset

float StrangeDevice::getYawScale() {
	return yawScale;
} // getYawScale

float StrangeDevice::getYawOffset() {
	return yawOffset;
} // getYawOffset

float StrangeDevice::getPitchScale() {
	return pitchScale;
} // getPitchScale

float StrangeDevice::getPitchOffset() {
	return pitchOffset;
} // getPitchOffset


void StrangeDevice::decodeButtonState(DEVICESTATE* deviceState, uint8_t* buttonData, uint8_t numButtons)
{
	unsigned buttonNumber;
	unsigned dataIndex;
	unsigned byteIndex;
	uint8_t mask;

	for (buttonNumber=0; buttonNumber < numButtons; buttonNumber++)
	{
		dataIndex = buttonNumber/8;
		byteIndex = buttonNumber%8;
		mask = 1 << byteIndex;
		deviceState->buttonValues[buttonNumber] = buttonData[dataIndex] & mask;
	} // for
} // decodeButtonState

void StrangeDevice::decodeStateFromBinaryMessage(DEVICESTATE* deviceState, NetMessage* msg)
{
	int i;
	uint8_t numButtons, numAxis, numSensors;
	uint8_t numBytes;
	uint8_t *buttonData;


	msg->getUInt8(numButtons);

	//if(numButtons != 1)
//		printd(WARNING, "StrangeDevice::update(): number of strange device's buttons is not 1!\n");

	if (numButtons > 0)
		numBytes = ((numButtons-1)/8)+1;
	else
		numBytes = 0;

	if (numBytes > 0)
	{
		buttonData = new uint8_t[numBytes];
		for (i=0; i < numBytes; i++)
		{
			msg->getUInt8(buttonData[i]);
		} // for
		decodeButtonState(deviceState, buttonData, numButtons);
		delete buttonData;
	} // if

	msg->getUInt8(numAxis);
// 	printd(INFO, "UDPDevice::decodeStateFromBinaryMessage(): found %u axes!\n", numAxis);
	for (i=0; i < numAxis; i++)
	{
		msg->getReal32(deviceState->axisValues[i]);
	} // for

	msg->getUInt8(numSensors);
// 	printd(INFO, "UDPDevice::decodeStateFromBinaryMessage(): found %u sensors!\n", numSensors);
	for (i=0; i < numSensors; i++)
	{
		msgFunctions::decode(deviceState->sensorValues[i], msg);
	} // for
} // decodeStateFromBinaryMessage
