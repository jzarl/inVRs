#ifndef STRANGEDEVICE_H_
#define STRANGEDEVICE_H_

#include "InputDevice.h"
#include "UdpDevice.h"
#include "../../SystemCore/NetMessage.h"
#include "../../SystemCore/ComponentInterfaces/NetworkInterface.h"
#include <vector>


class CONTROLLERMANAGER_API StrangeDevice : public UdpDevice
{
public:

	StrangeDevice();
	virtual ~StrangeDevice();

	virtual void update();

	virtual void setYawScale(float scale);
	virtual void setYawOffset(float offset);
	virtual void setPitchScale(float scale);
	virtual void setPitchOffset(float offset);

	virtual float getYawScale();
	virtual float getYawOffset();
	virtual float getPitchScale();
	virtual float getPitchOffset();

protected:
	static void decodeStateFromBinaryMessage(DEVICESTATE* deviceState, NetMessage* msg);
	static void decodeButtonState(DEVICESTATE* deviceState, uint8_t* buttonData, uint8_t numButtons);

	float yawScale;
	float yawOffset;
	float pitchScale;
	float pitchOffset;
}; // StrangeDevice

class StrangeDeviceFactory : public InputDeviceFactory
{
public:
	virtual ~StrangeDeviceFactory(){};
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // InputDeviceFactory




#endif /*STRANGEDEVICE_H_*/
