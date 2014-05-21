#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cwiid.h>
#include "DeviceStateSender.h"

cwiid_mesg_callback_t cwiid_callback;

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))

#define NUM_BUTTONS		0
#define NUM_AXIS 		0
#define NUM_SENSORS 	1

#define MAX_BUFFER_SIZE (4+1+1+(NUM_BUTTONS+7)/8+1+(4*NUM_AXIS)+1+(7*4*NUM_SENSORS))

#define CLIENTPORT 8082
#define CLIENTCHANNEL 255

struct Point {
    float x;
    float y;
};

DeviceState deviceState;
DeviceStateSender sender;

void set_led_state(cwiid_wiimote_t *wiimote, unsigned char led_state);
void set_rpt_mode(cwiid_wiimote_t *wiimote, unsigned char rpt_mode);
void print_state(struct cwiid_state *state);
void calc(struct Point *firstPoint, struct Point *secondPoint);
void set_num_leds(cwiid_wiimote_t *wiimote, int cnt);
int initNetwork(char* hostname, int port);

cwiid_err_t err;
void err(cwiid_wiimote_t *wiimote, const char *s, va_list ap)
{
	if (wiimote) printf("%d:", cwiid_get_id(wiimote));
	else printf("-1:");
	vprintf(s, ap);
	printf("\n");
}

int main(int argc, char *argv[])
{
	cwiid_wiimote_t *wiimote;	/* wiimote handle */
	struct cwiid_state state;	/* wiimote state */
	bdaddr_t bdaddr;	/* bluetooth device address */
	unsigned char mesg = 0;
	unsigned char led_state = 0;
	unsigned char rpt_mode = 0;
	unsigned char rumble = 0;
	int exit = 0;

	deviceState.numButtons = NUM_BUTTONS;
	deviceState.numAxis = NUM_AXIS;
	deviceState.numSensors = NUM_SENSORS;
	deviceState.axisValues = NULL;
	deviceState.buttonValues = NULL;
	deviceState.sensorValues = new SensorData[NUM_SENSORS];

	cwiid_set_err(err);

	if (argc < 2) {
		fprintf(stderr, "Usage: %s HOSTNAME\n\n", argv[0]);
		return 1;
	} // if

// TODO: parse data from config file!!!
	/* Connect to address given on command-line, if present */
	if (argc > 2) {
		str2ba(argv[2], &bdaddr);
	}
	else {
		bdaddr = *BDADDR_ANY;
	}

	/* Connect to the wiimote */
	printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
	if (!(wiimote = cwiid_open(&bdaddr, 0))) {
		fprintf(stderr, "Unable to connect to wiimote\n");
		return -1;
	}
	if (cwiid_set_mesg_callback(wiimote, cwiid_callback)) {
		fprintf(stderr, "Unable to set message callback\n");
	}

	toggle_bit(led_state, CWIID_LED1_ON);
	set_led_state(wiimote, led_state);
	if (cwiid_get_state(wiimote, &state)) {
		fprintf(stderr, "Error getting state\n");
	}
	print_state(&state);
	toggle_bit(rpt_mode, CWIID_RPT_IR);
	toggle_bit(rpt_mode, CWIID_RPT_BTN);
//	toggle_bit(rpt_mode, CWIID_RPT_ACC);
	set_rpt_mode(wiimote, rpt_mode);
	if (cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC)) {
	    fprintf(stderr, "Error enabling messages\n");
	} else {
	    mesg = 1;
	}

	if (initNetwork(argv[1], CLIENTPORT)) {
		fprintf(stderr, "Error at network initialization\n");
		return -1;
	} // if

	bool running = true;

	while (running) {sleep(1);}

	if (cwiid_close(wiimote)) {
		fprintf(stderr, "Error on wiimote disconnect\n");
		return -1;
	}

	return 0;
}

int initNetwork(char* hostname, int port) {
	sender.init(hostname, port, CLIENTCHANNEL);
	return 0;
} // initNetwork

void set_led_state(cwiid_wiimote_t *wiimote, unsigned char led_state)
{
	if (cwiid_set_led(wiimote, led_state)) {
		fprintf(stderr, "Error setting LEDs \n");
	}
}

void set_rpt_mode(cwiid_wiimote_t *wiimote, unsigned char rpt_mode)
{
	if (cwiid_set_rpt_mode(wiimote, rpt_mode)) {
		fprintf(stderr, "Error setting report mode\n");
	}
}

void print_state(struct cwiid_state *state)
{
	int i;
	int valid_source = 0;

	printf("Report Mode:");
	if (state->rpt_mode & CWIID_RPT_STATUS) printf(" STATUS");
	if (state->rpt_mode & CWIID_RPT_BTN) printf(" BTN");
	if (state->rpt_mode & CWIID_RPT_ACC) printf(" ACC");
	if (state->rpt_mode & CWIID_RPT_IR) printf(" IR");
	if (state->rpt_mode & CWIID_RPT_NUNCHUK) printf(" NUNCHUK");
	if (state->rpt_mode & CWIID_RPT_CLASSIC) printf(" CLASSIC");
	printf("\n");

	printf("Active LEDs:");
	if (state->led & CWIID_LED1_ON) printf(" 1");
	if (state->led & CWIID_LED2_ON) printf(" 2");
	if (state->led & CWIID_LED3_ON) printf(" 3");
	if (state->led & CWIID_LED4_ON) printf(" 4");
	printf("\n");

	printf("Rumble: %s\n", state->rumble ? "On" : "Off");

	printf("Battery: %d%%\n",
	       (int)(100.0 * state->battery / CWIID_BATTERY_MAX));

	printf("Buttons: %X\n", state->buttons);

	printf("Acc: x=%d y=%d z=%d\n", state->acc[CWIID_X], state->acc[CWIID_Y],
	       state->acc[CWIID_Z]);

	printf("IR: ");
	for (i = 0; i < CWIID_IR_SRC_COUNT; i++) {
		if (state->ir_src[i].valid) {
			valid_source = 1;
			printf("(%d,%d) ", state->ir_src[i].pos[CWIID_X],
			                   state->ir_src[i].pos[CWIID_Y]);
		}
	}
	if (!valid_source) {
		printf("no sources detected");
	}
	printf("\n");

	switch (state->ext_type) {
	case CWIID_EXT_NONE:
		printf("No extension\n");
		break;
	case CWIID_EXT_UNKNOWN:
		printf("Unknown extension attached\n");
		break;
	case CWIID_EXT_NUNCHUK:
		printf("Nunchuk: btns=%.2X stick=(%d,%d) acc.x=%d acc.y=%d "
		       "acc.z=%d\n", state->ext.nunchuk.buttons,
		       state->ext.nunchuk.stick[CWIID_X],
		       state->ext.nunchuk.stick[CWIID_Y],
		       state->ext.nunchuk.acc[CWIID_X],
		       state->ext.nunchuk.acc[CWIID_Y],
		       state->ext.nunchuk.acc[CWIID_Z]);
		break;
	case CWIID_EXT_CLASSIC:
		printf("Classic: btns=%.4X l_stick=(%d,%d) r_stick=(%d,%d) "
		       "l=%d r=%d\n", state->ext.classic.buttons,
		       state->ext.classic.l_stick[CWIID_X],
		       state->ext.classic.l_stick[CWIID_Y],
		       state->ext.classic.r_stick[CWIID_X],
		       state->ext.classic.r_stick[CWIID_Y],
		       state->ext.classic.l, state->ext.classic.r);
		break;
	}
}

/* Prototype cwiid_callback with cwiid_callback_t, define it with the actual
 * type - this will cause a compile error (rather than some undefined bizarre
 * behavior) if cwiid_callback_t changes */
/* cwiid_mesg_callback_t has undergone a few changes lately, hopefully this
 * will be the last.  Some programs need to know which messages were received
 * simultaneously (e.g. for correlating accelerometer and IR data), and the
 * sequence number mechanism used previously proved cumbersome, so we just
 * pass an array of messages, all of which were received at the same time.
 * The id is to distinguish between multiple wiimotes using the same callback.
 * */
void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg[], struct timespec *timestamp)
{
	int i, j;
	int valid_source;
	struct Point firstPoint;
	struct Point secondPoint;

	for (i=0; i < mesg_count; i++)
	{
		switch (mesg[i].type) {
		case CWIID_MESG_STATUS:
			printf("Status Report: battery=%d extension=",
			       mesg[i].status_mesg.battery);
			switch (mesg[i].status_mesg.ext_type) {
			case CWIID_EXT_NONE:
				printf("none");
				break;
			case CWIID_EXT_NUNCHUK:
				printf("Nunchuk");
				break;
			case CWIID_EXT_CLASSIC:
				printf("Classic Controller");
				break;
			default:
				printf("Unknown Extension");
				break;
			}
			printf("\n");
			break;
		case CWIID_MESG_BTN:
			printf("Button Report: %.4X (%u)\n", mesg[i].btn_mesg.buttons, mesg[i].btn_mesg.buttons);
			break;
		case CWIID_MESG_ACC:
			printf("Acc Report: x=%d, y=%d, z=%d\n",
                   mesg[i].acc_mesg.acc[CWIID_X],
			       mesg[i].acc_mesg.acc[CWIID_Y],
			       mesg[i].acc_mesg.acc[CWIID_Z]);
			break;
		case CWIID_MESG_IR:
//			printf("IR Report: ");
			valid_source = 0;
			for (j = 0; j < CWIID_IR_SRC_COUNT; j++) {
				if (mesg[i].ir_mesg.src[j].valid) {
					valid_source++;
//					printf("(%d,%d) ", mesg[i].ir_mesg.src[j].pos[CWIID_X],
//					                   mesg[i].ir_mesg.src[j].pos[CWIID_Y]);
					if (valid_source == 1) {
					    firstPoint.x = mesg[i].ir_mesg.src[j].pos[CWIID_X];
					    firstPoint.y = mesg[i].ir_mesg.src[j].pos[CWIID_Y];
					} else if (valid_source == 2) {
					    secondPoint.x = mesg[i].ir_mesg.src[j].pos[CWIID_X];
					    secondPoint.y = mesg[i].ir_mesg.src[j].pos[CWIID_Y];
					}
				}
			}
//			if (!valid_source) {
//				printf("no sources detected");
//			}
//			printf("\n");
			if (valid_source == 2) {
			    calc(&firstPoint, &secondPoint);
			}
			set_num_leds(wiimote, valid_source);
			break;
		case CWIID_MESG_NUNCHUK:
			printf("Nunchuk Report: btns=%.2X stick=(%d,%d) acc.x=%d acc.y=%d "
			       "acc.z=%d\n", mesg[i].nunchuk_mesg.buttons,
			       mesg[i].nunchuk_mesg.stick[CWIID_X],
			       mesg[i].nunchuk_mesg.stick[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_X],
			       mesg[i].nunchuk_mesg.acc[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_Z]);
			break;
		case CWIID_MESG_CLASSIC:
			printf("Classic Report: btns=%.4X l_stick=(%d,%d) r_stick=(%d,%d) "
			       "l=%d r=%d\n", mesg[i].classic_mesg.buttons,
			       mesg[i].classic_mesg.l_stick[CWIID_X],
			       mesg[i].classic_mesg.l_stick[CWIID_Y],
			       mesg[i].classic_mesg.r_stick[CWIID_X],
			       mesg[i].classic_mesg.r_stick[CWIID_Y],
			       mesg[i].classic_mesg.l, mesg[i].classic_mesg.r);
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
	sender.send(&deviceState);
}


void set_num_leds(cwiid_wiimote_t *wiimote, int cnt) {
    static int prev_cnt = -1;
    unsigned char led_state = 0;
    if (prev_cnt == cnt) return;
    if (cnt > 0) led_state |= CWIID_LED1_ON;
    if (cnt > 1) led_state |= CWIID_LED2_ON;
    if (cnt > 2) led_state |= CWIID_LED3_ON;
    if (cnt > 3) led_state |= CWIID_LED4_ON;
    set_led_state(wiimote, led_state);
    prev_cnt = cnt;
}

void calc(struct Point *firstPoint, struct Point *secondPoint) {
	//TODO: move data into config file
    float movementScaling = 1.0f;
    float radiansPerPixel = (3.1415 / 4) / 1024.0f;
    float dotDistanceInMM = 8.5f * 25.4f;
    float screenHeightinMM = 20 * 25.4f;
    float dx = firstPoint->x - secondPoint->x;
    float dy = firstPoint->y - secondPoint->y;
    float pointDist = sqrt(dx * dx + dy * dy);
    int cameraIsAboveScreen = 0;
    float cameraVerticaleAngle = 0;

    float angle = radiansPerPixel * pointDist / 2;
    //in units of screen height since the box is a unit cube and box hieght is 1
    float headDist = movementScaling * (float)((dotDistanceInMM / 2) / tan(angle)) / screenHeightinMM;

    float avgX = (firstPoint->x + secondPoint->x) / 2.0f;
    float avgY = (firstPoint->y + secondPoint->y) / 2.0f;

    float headX = (float)(movementScaling *  sin(radiansPerPixel * (avgX - 512)) * headDist);
    float headY;

    float relativeVerticalAngle = (avgY - 384) * radiansPerPixel;//relative angle to camera axis

    if(cameraIsAboveScreen) {
	headY = .5f+(float)(movementScaling * sin(relativeVerticalAngle + cameraVerticaleAngle)  *headDist);
    } else {
        headY = -.5f + (float)(movementScaling * sin(relativeVerticalAngle + cameraVerticaleAngle) * headDist);
    }
    printf("head: dist: %f, x: %f, y: %f\n", headDist, headX, headY);

	deviceState.sensorValues[0].position[0] = headX;
	deviceState.sensorValues[0].position[1] = headY;
	deviceState.sensorValues[0].position[2] = headDist;
}
