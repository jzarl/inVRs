/*
 * WiiDeviceServer.h
 *
 *  Created on: Mar 3, 2009
 *      Author: rlander
 */

#ifndef WIIDEVICESERVER_H_
#define WIIDEVICESERVER_H_

#include <string>
#include <cwiid.h>

#include "DeviceStateSender.h"

#include <OpenSG/OSGThreadManager.h>

class WiiDeviceServer {
public:
	WiiDeviceServer();
	virtual ~WiiDeviceServer();

	int run(int argc, char** argv);

	static void cwiid_error(cwiid_wiimote_t *wiimote, const char *s, va_list ap);

	static void cwiid_message_callback(cwiid_wiimote_t *wiimote, int mesg_count,
            union cwiid_mesg mesg[], struct timespec *timestamp);

private:
	bool loadConfig(char* configFile);
	bool initDeviceStateSender();
	bool initWiimote();
	void cleanupWiimote();
	void cleanupDeviceStateSender();

	void lightWiimoteLeds(int num_leds);
	bool setWiimoteLedState(cwiid_wiimote_t *wiimote, unsigned char led_state);
	bool setWiimoteReportMode(cwiid_wiimote_t *wiimote, unsigned char rpt_mode);

	void handleCalibrationMessage(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[],
			struct timespec *timestamp);
	void handleMessage(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[],
			struct timespec *timestamp);

	void updateWiimoteButtonState(uint16_t buttons);
	void updateNunchukButtonState(uint8_t buttons);
	bool updateNunchukAxes(uint8_t xAxis, uint8_t yAxis);
	bool handleIRInput(struct cwiid_ir_mesg ir_mesg);

	double getSystemTime();

	static WiiDeviceServer* instance;

	bool running;
	double sendTimeStep;

	// Variables for DeviceStateSender
	std::string hostName;
	int port;
	int controllerID;

	DeviceState deviceState;
	DeviceStateSender deviceStateSender;

	// Variables for Wiimote
	std::string wiimoteAddress;
	bool useWiimotePosition;
	bool useNUNCHUK;
	/* wiimote handle */
	cwiid_wiimote_t *wiimote;
	bool calibrated;

	uint8_t oldNunchukButtonState;
	uint8_t oldNunchukXAxisValue;
	uint8_t oldNunchukYAxisValue;
	uint8_t minNunchukXAxisValue;
	uint8_t minNunchukYAxisValue;
	uint8_t maxNunchukXAxisValue;
	uint8_t maxNunchukYAxisValue;
	float nunchukThreshold;

	float ledDistance;
	int numIRSources;
	float irXOffset;
	float irYOffset;
	float irZOffset;

	osg::Lock* lock;

};

#endif /* WIIDEVICESERVER_H_ */
