/*
 * WiiDeviceServer.cpp
 *
 *  Created on: Mar 3, 2009
 *      Author: rlander
 */

#include "WiiDeviceServer.h"

#include <assert.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#include "irrXML.h"

using namespace irr;
using namespace io;

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))

WiiDeviceServer* WiiDeviceServer::instance = NULL;

WiiDeviceServer::WiiDeviceServer() {
	running = false;

	useNUNCHUK = false;
	useWiimotePosition = false;

	sendTimeStep = 1;

	hostName = "";
	port = 0;
	controllerID = 0;

	wiimoteAddress = "";

	wiimote = NULL;

	oldNunchukButtonState = 0;
	oldNunchukXAxisValue = 0;
	oldNunchukYAxisValue = 0;
	minNunchukXAxisValue = std::numeric_limits<uint8_t>::max();
	minNunchukYAxisValue = std::numeric_limits<uint8_t>::max();
	maxNunchukXAxisValue = 0;
	maxNunchukYAxisValue = 0;
	nunchukThreshold = 0.05f;

	// default sensor bar
	ledDistance = 0.2f;
	numIRSources = -1;
	irXOffset = 0;
	irYOffset = 0;
	irZOffset = 0;

	calibrated = false;

	osg::osgInit(0, 0);
	lock = osg::ThreadManager::the()->getLock("WiiDeviceServerLock");

	if (instance == NULL)
		instance = this;
	else {
		printf("ERROR: WiiDeviceServer already created!\n");
		assert(false);
	} // else
} // WiiDeviceServer

WiiDeviceServer::~WiiDeviceServer() {
	// TODO Auto-generated destructor stub
}

int WiiDeviceServer::run(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <configFile>\n", argv[0]);
		return -1;
	} // if

	if (!loadConfig(argv[1])) {
		return -1;
	} // if

	if (!initDeviceStateSender()) {
		return -1;
	} // if

	if (!initWiimote()) {
		return -1;
	} // if

	running = true;

	double timeToNextStep = sendTimeStep;
	double oldTime = getSystemTime();
	double dt;

	double time;
	while (running) {
		time = getSystemTime();
		dt = time - oldTime;
		timeToNextStep -= dt;
		while (timeToNextStep < 0) {
			lock->aquire();
				deviceStateSender.send(&deviceState);
			lock->release();
			timeToNextStep += sendTimeStep;
		} // if
		oldTime = time;
		usleep((int)(timeToNextStep * 1000000));
	} // while

	cleanupWiimote();
	cleanupDeviceStateSender();

	return 0;
} // run

void WiiDeviceServer::cwiid_error(cwiid_wiimote_t *wiimote, const char *s, va_list ap) {

} // cwiid_error

void WiiDeviceServer::cwiid_message_callback(cwiid_wiimote_t *wiimote, int mesg_count,
        union cwiid_mesg mesg[], struct timespec *timestamp) {

	if (!instance->calibrated)
		instance->handleCalibrationMessage(wiimote, mesg_count, mesg, timestamp);
	else
		instance->handleMessage(wiimote, mesg_count, mesg, timestamp);
} // cwiid_message_callback

bool WiiDeviceServer::loadConfig(char* configFile) {
	IrrXMLReader* xml = createIrrXMLReader(configFile);

	if (!xml)
	{
		printf("Error loading config file %s\n", configFile);
		return false;
	} // if

	while(xml && xml->read())
	{
		switch (xml -> getNodeType())
		{
			case EXN_TEXT:
				break;
			case EXN_ELEMENT:
				if (!strcmp("bluetoothAddress", xml->getNodeName())) {
					if (xml->getAttributeValue("value"))
						wiimoteAddress = xml->getAttributeValue("value");
				} // if
				else if (!strcmp("sensorBar", xml->getNodeName())) {
					if (xml->getAttributeValue("ledDistance"))
						ledDistance = xml->getAttributeValueAsFloat("ledDistance");
				} // else if
				else if (!strcmp("reportWiimotePosition", xml->getNodeName())) {
					if (!strcmp(xml->getAttributeValue("value"), "true") ||
							!strcmp(xml->getAttributeValue("value"), "TRUE"))
						useWiimotePosition = true;
					else if (!strcmp(xml->getAttributeValue("value"), "false") ||
							!strcmp(xml->getAttributeValue("value"), "FALSE"))
						useWiimotePosition = false;
					if (xml->getAttributeValue("xOffset"))
						irXOffset = xml->getAttributeValueAsFloat("xOffset");
					if (xml->getAttributeValue("yOffset"))
						irYOffset = xml->getAttributeValueAsFloat("yOffset");
					if (xml->getAttributeValue("zOffset"))
						irZOffset = xml->getAttributeValueAsFloat("zOffset");
				} // else if
				else if (!strcmp("reportNunchuk", xml->getNodeName())) {
					if (!strcmp(xml->getAttributeValue("value"), "true") ||
							!strcmp(xml->getAttributeValue("value"), "TRUE"))
						useNUNCHUK = true;
					else if (!strcmp(xml->getAttributeValue("value"), "false") ||
							!strcmp(xml->getAttributeValue("value"), "FALSE"))
						useNUNCHUK = false;
				} // else if
				else if (!strcmp("client", xml->getNodeName())) {
					hostName = xml->getAttributeValue("host");
					port = xml->getAttributeValueAsInt("port");
					controllerID = xml->getAttributeValueAsInt("controllerID");
				} // else if
				else if (!strcmp("updateRate", xml->getNodeName())) {
					if (xml->getAttributeValue("stepsPerSecond"))
						sendTimeStep = 1.f/xml->getAttributeValueAsFloat("stepsPerSecond");
				} // else if
				break;
			case EXN_ELEMENT_END:
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;

	return true;
} // loadConfig

bool WiiDeviceServer::initDeviceStateSender() {
	deviceState.numAxis = 0;
	deviceState.numButtons = 0;
	deviceState.numSensors = 0;
	deviceState.buttonValues = NULL;
	deviceState.axisValues = NULL;
	deviceState.sensorValues = NULL;

	deviceState.numButtons += 13;
	if (useNUNCHUK) {
		deviceState.numButtons += 2;
		deviceState.numAxis += 2;
	}
	if (useWiimotePosition) {
		deviceState.numSensors += 1;
	}
	if (deviceState.numButtons > 0 ) {
		deviceState.buttonValues = new bool[deviceState.numButtons];
		memset(deviceState.buttonValues, 0, deviceState.numButtons * sizeof(bool));
	} // if
	if (deviceState.numAxis > 0) {
		deviceState.axisValues = new float[deviceState.numAxis];
		for (int i=0; i < deviceState.numAxis; i++) {
			deviceState.axisValues[i] = 0;
		} // for
	} // if
	if (deviceState.numSensors > 0) {
		deviceState.sensorValues = new SensorData[deviceState.numSensors];
		for (int i=0; i < deviceState.numSensors; i++) {
			deviceState.sensorValues[i].position = gmtl::Vec3f(0,0,0);
			deviceState.sensorValues[i].orientation = gmtl::QUAT_IDENTITYF;
		} // for
	} // if

	deviceStateSender.init(hostName.c_str(), port, controllerID);
	return true;
} // initDeviceStateSender

bool WiiDeviceServer::initWiimote() {

	bdaddr_t bdaddr;	/* bluetooth device address */
	unsigned char mesg = 0;
	unsigned char led_state = 0;
	unsigned char rpt_mode = 0;
	unsigned char rumble = 0;
	int exit = 0;

	cwiid_set_err(WiiDeviceServer::cwiid_error);

	if (wiimoteAddress.length() > 0) {
		str2ba(wiimoteAddress.c_str(), &bdaddr);
	} // if
	else {
		bdaddr = *BDADDR_ANY;
	} // else

	/* Connect to the wiimote */
	printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
	if (!(wiimote = cwiid_open(&bdaddr, 0))) {
		printf("Unable to connect to wiimote!\n");
		return false;
	} // if
	if (cwiid_set_mesg_callback(wiimote, WiiDeviceServer::cwiid_message_callback)) {
		printf("Unable to set message callback!\n");
	} // if

	/* Turn on first led */
	toggle_bit(led_state, CWIID_LED1_ON);
	if (!setWiimoteLedState(wiimote, led_state))
		return false;

	/* register required callback information */
	toggle_bit(rpt_mode, CWIID_RPT_BTN);
	if (useWiimotePosition) {
		toggle_bit(rpt_mode, CWIID_RPT_IR);
		toggle_bit(rpt_mode, CWIID_RPT_ACC);
	}
	if (useNUNCHUK) {
		toggle_bit(rpt_mode, CWIID_RPT_NUNCHUK);
	}
	if (!setWiimoteReportMode(wiimote, rpt_mode))
		return false;

	/* enable messages for callbacks */
	if (cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC)) {
	    printf("Error enabling messages!\n");
	    return false;
	} // if

	if (useNUNCHUK) {
		printf("Please Rotate Nunchuk-Axis around for calibration and press button when finished!\n");
		while (!calibrated) {
			usleep(100000);
		} // while
	} // if
	calibrated = true;

	toggle_bit(led_state, CWIID_LED1_ON);
	toggle_bit(led_state, CWIID_LED2_ON);
	if (!setWiimoteLedState(wiimote, led_state))
		return false;

	return true;
} // initWiimote

void WiiDeviceServer::cleanupWiimote() {
	if (cwiid_close(wiimote))
		printf("Error on wiimote disconnect\n");
} // cleanupWiimote

void WiiDeviceServer::cleanupDeviceStateSender() {
	if (deviceState.axisValues != NULL)
		delete[] deviceState.axisValues;
	if (deviceState.buttonValues != NULL)
		delete[] deviceState.buttonValues;
	if (deviceState.sensorValues != NULL)
		delete[] deviceState.sensorValues;
} // cleanupDeviceStateSender

void WiiDeviceServer::lightWiimoteLeds(int num_leds) {
    unsigned char led_state = 0;
    if (num_leds > 0) led_state |= CWIID_LED1_ON;
    if (num_leds > 1) led_state |= CWIID_LED2_ON;
    if (num_leds > 2) led_state |= CWIID_LED3_ON;
    if (num_leds > 3) led_state |= CWIID_LED4_ON;
    setWiimoteLedState(wiimote, led_state);
} // lightWiimoteLeds


bool WiiDeviceServer::setWiimoteLedState(cwiid_wiimote_t *wiimote, unsigned char led_state) {
	if (cwiid_set_led(wiimote, led_state)) {
		printf("Error setting LEDs!\n");
		return false;
	} // if
	return true;
} // setWiimoteLedState

bool WiiDeviceServer::setWiimoteReportMode(cwiid_wiimote_t *wiimote, unsigned char rpt_mode) {
	assert(wiimote);
	if (cwiid_set_rpt_mode(wiimote, rpt_mode)) {
		printf("Error setting report mode\n");
		return false;
	} // if
	return true;
} // setWiimoteReportMode

void WiiDeviceServer::handleCalibrationMessage(cwiid_wiimote_t *wiimote, int mesg_count,
		union cwiid_mesg mesg[], struct timespec *timestamp) {
	int i;
	for (i=0; i < mesg_count; i++)
	{
		switch (mesg[i].type) {
		case CWIID_MESG_NUNCHUK:
			if (mesg[i].nunchuk_mesg.stick[CWIID_X] < minNunchukXAxisValue)
				minNunchukXAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_X];
			if (mesg[i].nunchuk_mesg.stick[CWIID_X] > maxNunchukXAxisValue)
				maxNunchukXAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_X];
			if (mesg[i].nunchuk_mesg.stick[CWIID_Y] < minNunchukYAxisValue)
				minNunchukYAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_Y];
			if (mesg[i].nunchuk_mesg.stick[CWIID_Y] > maxNunchukYAxisValue)
				maxNunchukYAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_Y];

			if (mesg[i].nunchuk_mesg.buttons != 0 &&
					minNunchukXAxisValue < maxNunchukXAxisValue &&
					minNunchukYAxisValue < maxNunchukYAxisValue) {
				calibrated = true;
			} // if
			break;
		case CWIID_MESG_ERROR:
			if (cwiid_close(wiimote)) {
				fprintf(stderr, "Error on wiimote disconnect\n");
				exit(-1);
			}
			exit(0);
			break;
		default:
			break;
		}
	}
} // handleCalibrationMessage

void WiiDeviceServer::handleMessage(cwiid_wiimote_t *wiimote, int mesg_count,
		union cwiid_mesg mesg[], struct timespec *timestamp) {

	int i;
	int valid_source;
//	bool updateNeeded = false;

	lock->aquire();

	for (i=0; i < mesg_count; i++)
	{
		switch (mesg[i].type) {
		case CWIID_MESG_STATUS:
			break;
		case CWIID_MESG_BTN:
			instance->updateWiimoteButtonState(mesg[i].btn_mesg.buttons);
			if (deviceState.buttonValues[0] && deviceState.buttonValues[1])
				running = false;
//			updateNeeded = true;
			break;
		case CWIID_MESG_ACC:
/*
			printf("Acc Report: x=%d, y=%d, z=%d\n",
                   mesg[i].acc_mesg.acc[CWIID_X],
			       mesg[i].acc_mesg.acc[CWIID_Y],
			       mesg[i].acc_mesg.acc[CWIID_Z]);
*/
			break;
		case CWIID_MESG_IR:
//			updateNeeded = updateNeeded || instance->handleIRInput(mesg[i].ir_mesg);
			instance->handleIRInput(mesg[i].ir_mesg);
			break;
		case CWIID_MESG_NUNCHUK:
			if (mesg[i].nunchuk_mesg.buttons != instance->oldNunchukButtonState) {
				instance->updateNunchukButtonState(mesg[i].nunchuk_mesg.buttons);
				instance->oldNunchukButtonState = mesg[i].nunchuk_mesg.buttons;
//				updateNeeded = true;
			} //  if
			if ((mesg[i].nunchuk_mesg.stick[CWIID_X] != instance->oldNunchukXAxisValue) ||
					(mesg[i].nunchuk_mesg.stick[CWIID_Y] != instance->oldNunchukYAxisValue)) {

//				updateNeeded = updateNeeded || instance->updateNunchukAxes(
//						mesg[i].nunchuk_mesg.stick[CWIID_X], mesg[i].nunchuk_mesg.stick[CWIID_Y]);
				instance->updateNunchukAxes(mesg[i].nunchuk_mesg.stick[CWIID_X],
						mesg[i].nunchuk_mesg.stick[CWIID_Y]);
				instance->oldNunchukXAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_X];
				instance->oldNunchukYAxisValue = mesg[i].nunchuk_mesg.stick[CWIID_Y];
			}
/*
			printf("Nunchuk Report: btns=%.2X stick=(%d,%d) acc.x=%d acc.y=%d "
			       "acc.z=%d\n", mesg[i].nunchuk_mesg.buttons,
			       mesg[i].nunchuk_mesg.stick[CWIID_X],
			       mesg[i].nunchuk_mesg.stick[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_X],
			       mesg[i].nunchuk_mesg.acc[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_Z]);
*/
			break;
		case CWIID_MESG_CLASSIC:
/*
			printf("Classic Report: btns=%.4X l_stick=(%d,%d) r_stick=(%d,%d) "
			       "l=%d r=%d\n", mesg[i].classic_mesg.buttons,
			       mesg[i].classic_mesg.l_stick[CWIID_X],
			       mesg[i].classic_mesg.l_stick[CWIID_Y],
			       mesg[i].classic_mesg.r_stick[CWIID_X],
			       mesg[i].classic_mesg.r_stick[CWIID_Y],
			       mesg[i].classic_mesg.l, mesg[i].classic_mesg.r);
*/
			break;
		case CWIID_MESG_ERROR:
			if (cwiid_close(wiimote)) {
				fprintf(stderr, "Error on wiimote disconnect\n");
				exit(-1);
			}
			exit(0);
			break;
		default:
			printf("Unknown Report");
			break;
		}
	}
//	if (updateNeeded)
//		instance->deviceStateSender.send(&instance->deviceState);

	lock->release();
} // handleMessage

void WiiDeviceServer::updateWiimoteButtonState(uint16_t buttons) {
	int index = 0;
	while (index < 13) {
//		printf("buttons: %u\tand=%s\n", buttons, (((buttons & 1) != 0) ? "true" : "false"));
		deviceState.buttonValues[index] = (buttons & 1);
		buttons = buttons >> 1;
		index++;
	} // while

	for (int i=0; i < deviceState.numButtons; i++) {
		printf("%i ", (deviceState.buttonValues[i] ? 1 : 0));
	}
	printf("\n");
} // updateWiimoteButtonState

void WiiDeviceServer::updateNunchukButtonState(uint8_t buttons) {
	assert(deviceState.numButtons >= 15);
	deviceState.buttonValues[13] = buttons & 1;
	buttons = buttons >> 1;
	deviceState.buttonValues[14] = buttons & 1;

	for (int i=0; i < deviceState.numButtons; i++) {
		printf("%i ", (deviceState.buttonValues[i] ? 1 : 0));
	}
	printf("\n");
} // updateNunchukButtonState

bool WiiDeviceServer::updateNunchukAxes(uint8_t xAxis, uint8_t yAxis) {
	assert(deviceState.numAxis >= 2);

	bool needUpdate = true;

	float halfXAxisRange = 0.5f * (maxNunchukXAxisValue - minNunchukXAxisValue);
	float halfYAxisRange = 0.5f * (maxNunchukYAxisValue - minNunchukYAxisValue);
	float newXAxisValue = ((xAxis - minNunchukXAxisValue) / halfXAxisRange) - 1;
	float newYAxisValue = ((yAxis - minNunchukYAxisValue) / halfYAxisRange) - 1;

	if (newXAxisValue > -nunchukThreshold && newXAxisValue < nunchukThreshold)
		newXAxisValue = 0;
	if (newYAxisValue > -nunchukThreshold && newYAxisValue < nunchukThreshold)
		newYAxisValue = 0;

	if (newXAxisValue == deviceState.axisValues[0] && newYAxisValue == deviceState.axisValues[1])
		needUpdate = false;

	deviceState.axisValues[0] = newXAxisValue;
	deviceState.axisValues[1] = newYAxisValue;

	if (needUpdate) {
		for (int i=0; i < deviceState.numAxis; i++) {
			printf("%f ", deviceState.axisValues[i]);
		}
		printf("\n");
	} // if

	return needUpdate;
} // updateNunchukAxes

bool WiiDeviceServer::handleIRInput(struct cwiid_ir_mesg ir_mesg) {
	int valid_sources = 0;

	gmtl::Vec2f firstPoint, secondPoint;

	for (int j = 0; j < CWIID_IR_SRC_COUNT; j++) {
		if (ir_mesg.src[j].valid) {
			valid_sources++;
			if (valid_sources == 1) {
				firstPoint[0] = ir_mesg.src[j].pos[CWIID_X];
				firstPoint[1] = ir_mesg.src[j].pos[CWIID_Y];
			} else if (valid_sources == 2) {
			    secondPoint[0] = ir_mesg.src[j].pos[CWIID_X];
			    secondPoint[1] = ir_mesg.src[j].pos[CWIID_Y];
			} // else if
		} // if
	} // for
	if (valid_sources != numIRSources) {
		lightWiimoteLeds(valid_sources);
		numIRSources = valid_sources;
	} // if
	if (valid_sources != 2)
		return false;

	gmtl::Vec2f pointDistanceVec = firstPoint - secondPoint;
	gmtl::Vec3f wiimotePosition;
	gmtl::Vec2f averageVec;
	float pointDistance = gmtl::length(pointDistanceVec);
	// wiimote camera captures 45 degrees within 1024 pixel
	float radiansPerPixel = (3.1415 / 4) / 1024.0f;

    // calculate the angle between wiimote and target point
    float angle = radiansPerPixel * pointDistance / 2;
    // calculate distance to screen
    wiimotePosition[2] = (float)((pointDistance / 2) / tan(angle)) * (ledDistance / pointDistance);

    // Calculate position of mid-point between two ir sources in camera coordinates (pixel)
    averageVec[0] = (firstPoint[0] + secondPoint[0]) / 2.0f;
    averageVec[1] = (firstPoint[1] + secondPoint[1]) / 2.0f;

    // calculate the horizontal angle between ray target point and mid-point of ir sources
    float xAngle = radiansPerPixel * (averageVec[0] - 512);
    wiimotePosition[0] = -(float)(tan(xAngle) * wiimotePosition[2]) + irXOffset;

    // calculate the vertical angle between ray target point and mid-point of ir sources
    float yAngle = radiansPerPixel * (averageVec[1] - 384);
    wiimotePosition[1] = -(float)(tan(yAngle) * wiimotePosition[2]) + irYOffset;
//    printf("head: dist: %f, x: %f, y: %f\n", wiimotePosition[2], wiimotePosition[0],
//   		wiimotePosition[1]);

// X and Y won't work as soon as wiimote is rotated
//	deviceState.sensorValues[0].position[0] = wiimotePosition[0];
//	deviceState.sensorValues[0].position[1] = wiimotePosition[1];
    deviceState.sensorValues[0].position[0] = 0;
    deviceState.sensorValues[0].position[1] = 0;
	deviceState.sensorValues[0].position[2] = -wiimotePosition[2] + irZOffset;

	gmtl::Vec3f rayDir(0,0,-1);
	yAngle = atan(wiimotePosition[1]/wiimotePosition[2]);
	xAngle = atan(wiimotePosition[0]/wiimotePosition[2]);
	gmtl::AxisAnglef xAxAng(yAngle, 1, 0, 0);
	gmtl::AxisAnglef yAxAng(xAngle, 0, 1, 0);
	gmtl::Quatf xRot, yRot;
	gmtl::set(xRot, xAxAng);
	gmtl::set(yRot, yAxAng);

	deviceState.sensorValues[0].orientation = xRot * yRot;

	return true;
} // handleIRInput

double WiiDeviceServer::getSystemTime() {
#ifdef WIN32
	return ((double)GetTickCount()/1000.0);
#else
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	gettimeofday(&tv, 0);
	return ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0);
#endif
} // getSystemTime


int main(int argc, char **argv) {
	WiiDeviceServer* app = new WiiDeviceServer();
	bool result = app->run(argc, argv);
	delete app;
	return result;
} // main
