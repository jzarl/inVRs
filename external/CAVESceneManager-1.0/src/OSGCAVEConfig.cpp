/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                        Project: CAVESceneManager                          *
 *                                                                           *
 * The CAVE Scene Manager (CSM) is a wrapper for the OpenSG multi-display    *
 * functionality, which allows the easy configuration of a multi-display     *
 * setup via text files. It was developed by Adrian Haffegee of the Centre   *
 * for Advanced Computing and Emerging Technologies (ACET) at the            *
 * University of Reading, UK as part of his MSc Thesis.                      *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*! \file OSGCAVEConfig.cpp
 *
 *  Adrian Haffegee, Sept 2004
 *	Implementation file for CAVE Scene Manager (CSM) configurator class
 */

#include "../include/OSGCSM/OSGCAVEConfig.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstring>


namespace OSGCSM
{

#define MAXLINELENGTH 255

namespace
{
	struct CAVEUnitsStruct
	{
		OSGCSM::CAVEConfig::CAVEUnitType type;
		const char* name;
		float conversion_factor_to_cm;
	};

	CAVEUnitsStruct CAVEUnits[CAVEConfig::CAVEUnit_Last] = {
		{ CAVEConfig::CAVEUnitFeet, "feet", 30.48f },
		{ CAVEConfig::CAVEUnitMeters, "meters", 100.f },
		{ CAVEConfig::CAVEUnitInches, "inches", 2.54f },
		{ CAVEConfig::CAVEUnitCentimeters, "centimeters", 1.f }
	};

	char* skipWhiteSpace(char* data)
	{
		if ((!data ) || (*data == '#') || (*data == '\0')) {
			return NULL;
		}
		while ((*data == '\t') || (*data == ' ')) {
			data ++;
			// return NULL if find # or \0
			if ((*data == '#') || (*data == '\0')) {
				return NULL;
			}
		}
		return data;
	}

	/*! read next float value from text string
	 * @param data pointer to text to parse
	 * @param value pointer to where to store read value
	 * @return NULL if failed or no more valid characters in line, else pointer to next valid characters after float
	 */
	char* getNextFloat(char *data, float* value)
	{
		data = skipWhiteSpace(data);
		// convert next to float - if not valid, returned value is same as *data, and *value = 0
		*value = static_cast<float>(atof(data));
		// if conversion failed, return NULL
		if ((*value==0) && (*data != '0')) {
			return NULL;
		}
		// otherwise skip forward to next whitespace or # or /0
		do {
			data ++;
		} while ((*data != '\t') && (*data != ' ') && (*data != '#') && (*data != '\0') );
		return data;
	}


	/*! read next integer value from text string
	 * @param data pointer to text to parse
	 * @param value pointer to where to store read value
	 * @return NULL if failed or no more valid characters in line, else pointer to next valid characters after int
	 */
	char* getNextInt(char *data, int* value)
	{
		data = skipWhiteSpace(data);
		// convert next to float - if not valid, returned value is same as *data, and *value = 0
		*value = atoi(data);
		// if conversion failed, return NULL
		if ((*value==0) && (*data != '0')) {
			return NULL;
		}
		// otherwise skip forward to next whitespace or # or /0
		do {
			data ++;
		} while ((*data != '\t') && (*data != ' ') && (*data != '#') && (*data != '\0') );
		return data;
	}


	/*! read next characters from a text string
	 * @param data pointer to text to parse
	 * @param string pointer for where to store read string
	 * @param length maximum number of characters to read
	 * @return NULL if failed or no more valid characters in text line, else pointer to next valid characters
	 */
	char* getNextString(char *data, char* string, int length)
	{
		data = skipWhiteSpace(data);
		if (!data) {
			return NULL;
		}
		// copy characters until next whitespace or # or /0 is reached
		do {
			if (length<=1) {
				// if more than (length-1) chars have been copied, fail by returning NULL
				return NULL;
			}
			*string = *data;
			data ++;
			string ++;
			length --;
		} while ((*data != '\t') && (*data != ' ') && (*data != '#') && (*data != '\0') );
		// null terminate name
		*string = '\0';
		return data;
	}

	/*! parse for display mode
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseDisplayMode(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(DispMode) '%s'\n", params);
	#endif
		if ((params = skipWhiteSpace(params))) {
			if (strncmp(params, "mono", strlen("mono")) == 0) {
				cfgObj.setStereo(false);
				return true;
			} else if (strncmp(params, "stereo", strlen("stereo")) == 0) {
				cfgObj.setStereo(true);
				return true;
			}
		}
		return false;
	}

	/*! parse for eye separation
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseEyeSparation(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(Eye) '%s'\n", params);
	#endif
		float eyeSep;
		if (params = getNextFloat(params, &eyeSep)) {
			// check unit type
			char units[15];
			if (params = getNextString(params, units, sizeof(units))) {
				for (int i=0; i<CAVEConfig::CAVEUnit_Last; i++) {
					if (strncmp(CAVEUnits[i].name, units, strlen(CAVEUnits[i].name))==0) {
						cfgObj.setEyeSeparation(eyeSep, CAVEUnits[i].type);
						return true;
					}
				}
			} else {
				// assume cm, since nothing was given
				cfgObj.setEyeSeparation(eyeSep, OSGCSM::CAVEConfig::CAVEUnitCentimeters);
				return true;
			}
		}
		return false;
	}

	/*! parse for multicast client/server OpenSG graphics
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseMulticastServerComms(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(M/C server) '%s'\n", params);
	#endif
		if ((params = skipWhiteSpace(params))) {
			if (strncmp(params, "yes", strlen("yes")) == 0) {
				cfgObj.setMCastServer(true);
				return true;
			} else if (strncmp(params, "no", strlen("no")) == 0) {
				cfgObj.setMCastServer(false);
				return true;
			}
		}
		return false;
	}

	bool parseBroadcastaddress(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		std::cout << "\t(Broadcastaddress) '" << params << "'\n";
	#endif
		if ((params = skipWhiteSpace(params))) {
			cfgObj.setBroadcastaddress(params);
			return true;
		}
		return false;
	}

	/*! parse for tracker IPC key
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseTrackerDaemonKey(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(Track) '%s'\n", params);
	#endif
		int key;
		if (params = getNextInt(params, &key)) {
			cfgObj.setTrackerKey(key);
			return true;
		}
		return false;
	}

	/*! parse for controller IPC key
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseControllerDaemonKey(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(controller) '%s'\n", params);
	#endif
		int key;
		if (params = getNextInt(params, &key)) {
			cfgObj.setControllerKey(key);
			return true;
		}
		return false;
	}

	/*! parse for active walls
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseWalls(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(Walls) '%s'\n", params);
	#endif
		if ((params = skipWhiteSpace(params))) {
			char name[25];
			while (params = getNextString(params, name, sizeof(name))) {
				// find wall matching this name, and set it active
				int i;
				for (i=0; i<CSM_MAXCAVEWALLS; i++) {
					if ((cfgObj.getWalls()[i]!=NULL) &&
							(cfgObj.getWalls()[i]->getName() == name))
					{
						cfgObj.getWalls()[i]->setActive(true);
						break;
					}
				}
				// of if not found, look for an empty slot, add the wall and set it active
				if (i==CSM_MAXCAVEWALLS) {
					for (i=0; i<CSM_MAXCAVEWALLS; i++) {
						if (cfgObj.getWalls()[i]==NULL) {
							cfgObj.getWalls()[i] = new CAVEWall(name);
							cfgObj.getWalls()[i]->setActive(true);
							break;
						}
					}
					// if couldn't add new return false for failure
					if (i==CSM_MAXCAVEWALLS) {
						return false;
					}
				}
			}
			return true;
		}
		return false;
	}

	/*! parse for wall display
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseWallDisplay(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(WallsDisp) '%s'\n", params);
	#endif
		if ((params = skipWhiteSpace(params))) {
			char name[25];
			if (params = getNextString(params, name, sizeof(name))) {
				CAVEWall *wall = NULL;
				// find wall matching this name
				for (int i=0; i<CSM_MAXCAVEWALLS; i++) {
					if (cfgObj.getWalls()[i] &&
							(cfgObj.getWalls()[i]->getName() == name)) 
					{
						wall = cfgObj.getWalls()[i];
						break;
					}
				}
				// of if not found, look for an empty slot
				if (!wall) {
					for (int i=0; i<CSM_MAXCAVEWALLS; i++) {
						if (cfgObj.getWalls()[i]==NULL) {
							wall = new CAVEWall(name);
							cfgObj.getWalls()[i] = wall;
							break;
						}
					}
				}
				char buf[255];
				// if have ptr to wall, fill the relevant values
				if ((wall) && (params = getNextString(params, buf, sizeof(buf)))) {
					if (wall->setPipe(buf)) {
						// get next (optional 'window' or geometry)
						if (!(params = getNextString(params, buf, sizeof(buf)))) {
							return true;
						}
						if (strcmp(buf, "window") == 0) {
							// if was window, set it, + get optional next
							wall->setWindowed(true);
							if (!(params = getNextString(params, buf, sizeof(buf)))) {
								return true;
							}
						}
						// remaining must be geometry
						if (wall->parseGeometry(buf)) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	/*! parse for projection data
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseProjectionData(CAVEConfig& cfgObj, char* params)
	{
		if ((params = skipWhiteSpace(params))) 
		{
			char name[25];
			if (params = getNextString(params, name, sizeof(name))) {
				CAVEWall *wall = NULL;
				// find wall matching this name
				for (int i=0; i<CSM_MAXCAVEWALLS; i++) 
				{
					if ((cfgObj.getWalls()[i]!=NULL) &&
							(cfgObj.getWalls()[i]->getName() == name )) 
					{
						wall = cfgObj.getWalls()[i];
						break;
					}
				}
				// of if not found, look for an empty slot
				if (!wall) 
				{
					for (int i=0; i<CSM_MAXCAVEWALLS; i++) 
					{
						if (cfgObj.getWalls()[i]==NULL) 
						{
							wall = new CAVEWall(name);
							cfgObj.getWalls()[i] = wall;
							break;
						}
					}
				}
				char buf[15];
				// if have ptr to wall, check the rest of the values

				if ((wall) && (params = getNextString(params, buf, sizeof(buf)))) {
					// for now a bit hard coded - will only allow proj data for both ("*" or "both")
					// eyes, and for walls (not hmd)
					if ((buf) && ((strcmp(buf, "*") == 0) || (strcmp(buf, "both") == 0))
							&& (params = getNextString(params, buf, sizeof(buf)))
							&& (buf) && (strcmp(buf, "wall") == 0)) {
						// now go through and read the projection data itself
						float projData[3][3];
						for (int point=0; point<3; point++) {
							for (int xyz=0; xyz<3; xyz++) {
								if ((params = getNextFloat(params, &projData[point][xyz])) == NULL) {
									return false;
								}
							}
						}
						// and finally the units
						char units[15];
						if (params = getNextString(params, units, sizeof(units))) {
							for (int i=0; i<OSGCSM::CAVEConfig::CAVEUnit_Last; i++) {
								if (strcmp(CAVEUnits[i].name, units)==0) {
									wall->setProjectionData(OSG::Pnt3f(projData[0][0],
												projData[0][1], projData[0][2]),
											OSG::Pnt3f(projData[1][0], projData[1][1], projData[1][2]),
											OSG::Pnt3f(projData[2][0], projData[2][1], projData[2][2]),
											CAVEUnits[i].conversion_factor_to_cm);
									return true;
								}
							}
						}


					} else {
						printf("Invalid proj data eye and/or type\n");

					}
				}
			}
		}
		return false;
	}


	/*! parse for CAVE origin
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseOrigin(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		printf("\t(Origin) '%s'\n", params);
	#endif
		float x,y,z;
		if ((params = getNextFloat(params, &x))
				&& (params = getNextFloat(params, &y))
				&& (params = getNextFloat(params, &z))) {
			char units[15];
			if (params = getNextString(params, units, sizeof(units))) {
				for (int i=0; i<OSGCSM::CAVEConfig::CAVEUnit_Last; i++) {
					if (strncmp(CAVEUnits[i].name, units, strlen(CAVEUnits[i].name))==0) {
						cfgObj.setCAVEOrigin(OSG::Pnt3f(x,y,z), CAVEUnits[i].type);
						return true;
					}
				}
			}
		}
		return false;
	}

	/*! parse for CAVE width
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseCAVEWidth(CAVEConfig& cfgObj, char* params)
	{
		float width;
		if (params = getNextFloat(params, &width)) {
			char units[15];
			if (params = getNextString(params, units, sizeof(units))) {
				for (int i=0; i<OSGCSM::CAVEConfig::CAVEUnit_Last; i++) {
					if (strcmp(CAVEUnits[i].name, units)==0) {
						cfgObj.setCAVEWidth(width, CAVEUnits[i].type);
						return true;
					}
				}
			}
		}
		return false;
	}

	/*! parse for CAVE height
	 * @param cfgObj CAVE configuration object
	 * @param params string of text to parse
	 * @return true if parsed ok
	 */
	bool parseCAVEHeight(CAVEConfig& cfgObj, char* params)
	{
		float height;
		if (params = getNextFloat(params, &height)) {
			char units[15];
			if (params = getNextString(params, units, sizeof(units))) {
				for (int i=0; i<OSGCSM::CAVEConfig::CAVEUnit_Last; i++) {
					if (strcmp(CAVEUnits[i].name, units)==0) {
						cfgObj.setCAVEHeight(height, CAVEUnits[i].type);
						return true;
					}
				}
			}
		}
		return false;
	}


	bool parseFollowHead(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		std::cout << "\t(FollowHead) '" << params << "'\n";
	#endif
		if ((params = skipWhiteSpace(params))) {
			if (strncmp(params, "on", strlen("on")) == 0) {
				cfgObj.setFollowHead(true);
				return true;
			} else if (strncmp(params, "off", strlen("off")) == 0) {
				cfgObj.setFollowHead(false);
				return true;
			}
		}
		return false;
	}

	bool parseSensorIDHead(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		std::cout << "\t(SensorIDHead) '" << params << "'\n";
	#endif
		int id;
		if (params = getNextInt(params, &id)) {
			cfgObj.setSensorIDHead(id);
			return true;
		}
		return false;
	}

	bool parseSensorIDController(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		std::cout << "\t(SensorIDController) '" << params << "'\n";
	#endif
		int id;
		if (params = getNextInt(params, &id)) {
			cfgObj.setSensorIDController(id);
			return true;
		}
		return false;
	}

	bool parseUnits(CAVEConfig& cfgObj, char* params)
	{
	#ifdef CSM_DEBUG
		std::cout << "\t(Units) '" << params << "'\n";
	#endif
		char units[15];
		if (params = getNextString(params, units, sizeof(units))) {
			for (int i=OSGCSM::CAVEConfig::CAVEUnitType(); i<OSGCSM::CAVEConfig::CAVEUnit_Last; ++i) {
				if (strcmp(CAVEUnits[i].name, units)==0) {
					cfgObj.setUnits(static_cast<CAVEConfig::CAVEUnitType>(i));
					return true;
				}
			}
		}
		return false;
	}

	struct cfgFuncStruct
	{
		const char* cfgParam;
		bool (*cfgFunc)(CAVEConfig&, char*);
	};

	const cfgFuncStruct cfgFunctions[] = {
		{"DisplayMode", parseDisplayMode},
		{"EyeSeparation", parseEyeSparation},
		{"MulticastServerComms", parseMulticastServerComms},
		{"TrackerDaemonKey", parseTrackerDaemonKey},
		{"ControllerDaemonKey", parseControllerDaemonKey},
		{"WallDisplay", parseWallDisplay},
		{"Walls", parseWalls},
		{"ProjectionData", parseProjectionData},
		{"Origin", parseOrigin},
		{"CAVEWidth", parseCAVEWidth},
		{"CAVEHeight", parseCAVEHeight},
		{"FollowHead", parseFollowHead},
		{"SensorIDHead", parseSensorIDHead},
		{"SensorIDController", parseSensorIDController},
		{"Broadcastaddress", parseBroadcastaddress},
		{"Units", parseUnits},
	};

}


/*!
 *  Constructor
 */
CAVEConfig::CAVEConfig()
	: m_followHead(true), m_SensorIDHead(0), m_SensorIDController(1), m_walls(CSM_MAXCAVEWALLS, NULL)
{
	m_stereo = false;
	m_mCastServer = false;
	m_eyeSeparation = 6;
	m_trackerKey = 0;
	m_ControllerKey = 0;
	m_height = 0;
	m_heightUnits = CAVEUnitCentimeters;
	m_width = 0;
	m_widthUnits = CAVEUnitCentimeters;
	m_origin.setValues(0,0,0);
	m_origUnits = CAVEUnitCentimeters;
	m_Units = CAVEUnitFeet;
}


/*!
 *  Destructor
 */
CAVEConfig::~CAVEConfig(void)
{
	for (wall_container_t::iterator i = m_walls.begin(); i != m_walls.end(); ++i)
		delete *i;
}


/*! Load a configuration file
 * @param cfgFile name of file (path optional)
 * @return true if loaded ok
 */
bool CAVEConfig::loadFile(const char* cfgFile)
{
	std::ifstream file(cfgFile);
	if (!file.is_open()) 
	{
		printf("Failed to open config file '%s'\n", cfgFile);
		return false;
	} else 
	{
		int lineCnt=0;
		std::string line;
		while (std::getline(file, line))
		{
			++lineCnt;
			if (!parseLine(line)) 
			{
				printf("Error in config file '%s' at line %d\n", cfgFile, lineCnt );
				return false;
			}
		}
	}
	// could have a function to check the validity of the configuration?
	return checkConfig();
}


/*! Print configuration information
 */
void CAVEConfig::printConfig() const
{
	printf("CAVE configuration:\n");
	printf("\t%d Active walls\n", m_activeWalls);
	for (int i=0; i<CSM_MAXCAVEWALLS; i++) {
		if (m_walls[i]!=NULL) {
			m_walls[i]->printConfig();
		}
	}
	printf("\tCAVE width (+depth): %f %s\n", m_width, CAVEUnits[m_widthUnits].name);
	printf("\tCAVE height: %f %s\n", m_height, CAVEUnits[m_heightUnits].name);
	printf("\tCAVE Origin: %f to left wall, %f to floor, %f to front (units %s)\n",
			m_origin[0], m_origin[1], m_origin[2], CAVEUnits[m_origUnits].name);
	printf("\tDisplay Mode: %s\n", isStereo()?"stereo":"mono");
	printf("\tEye separation: %f\n", getEyeSeparation());
	printf("\tTracker key: %d\n", getTrackerKey());
	printf("\tController key: %d\n", getControllerKey());
	printf("\tMulticast graphics server comms: %s\n", isMCastServer()?"yes":"no");
	std::cout << "\tBroadcastaddress: '" << getBroadcastaddress() << "'\n";
	std::cout << "\tfollow Head: " << (getFollowHead() ? "on" : "off") << '\n';
	std::cout << "\tSensor ID Head: " << getSensorIDHead() << '\n';
	std::cout << "\tSensor ID Controller: " << getSensorIDController() << '\n';
	std::cout << "\tUnits: " << getUnits() << " (" << CAVEUnits[getUnits()].name <<")\n";
	std::cout << '\n';
}


/*! check configuration
 * @return true configuraton ok
 */
bool CAVEConfig::checkConfig()
{
	// check configuration is as expected

	// count number of active walls
	m_activeWalls = 0;
	m_numPipes = 0;
	for (int i=0; i<CSM_MAXCAVEWALLS; i++) 
	{
		if ((m_walls[i]) && (m_walls[i]->isActive())) 
		{
			// check for wall projection data (calculate it for regular CAVE walls)
			float origCf = CAVEUnits[m_origUnits].conversion_factor_to_cm;

			if (!m_walls[i]->checkProjectionData(OSG::Vec3f( m_origin[0]*origCf,
							m_origin[1]*origCf, m_origin[2]*origCf),
						m_width*CAVEUnits[m_widthUnits].conversion_factor_to_cm,
						m_height*CAVEUnits[m_heightUnits].conversion_factor_to_cm)) 
			{

				return false;
			}
			m_activeWalls++;
			// check against all previous active walls - give it an index to reference its pipe
			int pipeIndex = -1;
			for (int prevWall=0; prevWall<i; prevWall++) {
				if (getActiveWall(i)->getPipe() == getActiveWall(prevWall)->getPipe()) {
					pipeIndex = getActiveWall(prevWall)->getPipeIndex();
					break;
				}
			}
			if (pipeIndex==-1) {
				pipeIndex = m_numPipes++;
			}
			editActiveWall(i)->setPipeIndex(pipeIndex);
		}
	}
	if (!m_activeWalls) {
#ifdef CSM_DEBUG
		printf("Cfg: No WallDisplays configured\n");
#endif
		return false;
	}

	return true;
}

void CAVEConfig::setFollowHead(bool follow_head)
{
	m_followHead = follow_head;
}

void CAVEConfig::setSensorIDHead(sensor_id_t sensor_id)
{
	m_SensorIDHead = sensor_id;
}

void CAVEConfig::setSensorIDController(sensor_id_t sensor_id)
{
	m_SensorIDController = sensor_id;
}

/*! set (visual) stereo mode
 * @param useStereo true if using stereo
 */
void CAVEConfig::setStereo(bool useStereo)
{
	m_stereo = useStereo;

}

/*! set if using multicast for client/server OpenSG graphics
 * @param useMCast if using multicast
 */
void CAVEConfig::setMCastServer(bool useMCast)
{
	m_mCastServer = useMCast;

}

void CAVEConfig::setBroadcastaddress(std::string ip)
{
	m_Broadcastaddress = ip;
}


/*! set eye separation
 * @param eyeSep value for eye separation
 */
void CAVEConfig::setEyeSeparation(float eyeSep, CAVEUnitType units)
{
	m_eyeSeparation = eyeSep * CAVEUnits[units].conversion_factor_to_cm;
}

/*! set tracker IPC key
 * @param key tracker IPC key
 */
void CAVEConfig::setTrackerKey(int key)
{
	m_trackerKey = key;
}

/*! set controller IPC key
 * @param key controller IPC key
 */
void CAVEConfig::setControllerKey(int key)
{
	m_ControllerKey = key;
}

/*! set CAVE height
 * @param height height value
 * @param units height units
 */
void CAVEConfig::setCAVEHeight(float height, CAVEUnitType units)
{
	m_height = height;
	m_heightUnits = units;
}

/*! set CAVE width
 * @param width width value
 * @param units width units
 */
void CAVEConfig::setCAVEWidth(float width, CAVEUnitType units)
{
	m_width = width;
	m_widthUnits = units;
}

/** Set the CAVE origin
 * @param origin the position of the origin for the CAVE
 * @param units the type of units used
 */
void CAVEConfig::setCAVEOrigin(OSG::Pnt3f origin, CAVEUnitType units)
{
	m_origin = origin;
	m_origUnits = units;
}

void CAVEConfig::setUnits(CAVEUnitType units)
{
	m_Units = units;
}

bool CAVEConfig::getFollowHead() const 
{
	return m_followHead;
}

CAVEConfig::sensor_id_t CAVEConfig::getSensorIDHead() const 
{
	return m_SensorIDHead;
}

CAVEConfig::sensor_id_t CAVEConfig::getSensorIDController() const
{
	return m_SensorIDController;
}


/*! Get stereo mode
 * @return if stereo mode or not
 */
bool CAVEConfig::isStereo() const
{
	return m_stereo;
}

/*! Get mode for if using multicast for client/server OpenSG graphics
 * @return if stereo mode or not
 */
bool CAVEConfig::isMCastServer() const
{
	return m_mCastServer;
}

const std::string& CAVEConfig::getBroadcastaddress() const
{
	return m_Broadcastaddress;
}

/*! Get the eye separation
 * @return eye separation (cm)
 */
float CAVEConfig::getEyeSeparation() const
{
	return m_eyeSeparation;
}

/*! Get tracker IPC key
 * @return tracker IPC key
 */
int CAVEConfig::getTrackerKey() const
{
	return m_trackerKey;
}

/*! Get controller IPC key
 * @return controller IPC key
 */
int CAVEConfig::getControllerKey() const
{
	return m_ControllerKey;
}

/*! Get the number of active display walls
 * @return number of walls
 */
int CAVEConfig::getNumActiveWalls() const
{
	return m_activeWalls;
}

/*! Get a pointer to an active display wall
 * @param wallNum wall number to get the pointer for
 * @return pointer to the wall
 */
CAVEWall* CAVEConfig::editActiveWall(size_t wallNum)
{
	if (wallNum >= m_activeWalls) {
		return NULL;
	}

	for (int i=0; i<CSM_MAXCAVEWALLS; i++) {
		if ((m_walls[i]) && (m_walls[i]->isActive())) {

			if (wallNum==0) {
				return m_walls[i];
			}
			wallNum--;
		}
	}
	return NULL;
}

/*! Get a pointer to an active display wall
 * @param wallNum wall number to get the pointer for
 * @return pointer to the wall
 */
const CAVEWall* CAVEConfig::getActiveWall(size_t wallNum) const
{
	if (wallNum >= m_activeWalls) {
		return NULL;
	}

	for (int i=0; i<CSM_MAXCAVEWALLS; i++) {
		if ((m_walls[i]) && (m_walls[i]->isActive())) {

			if (wallNum==0) {
				return m_walls[i];
			}
			wallNum--;
		}
	}
	return NULL;
}

/*! Get the number of pipes
 * @return number of walls
 */
int CAVEConfig::getNumPipes() const
{
	return m_numPipes;
}

const CAVEConfig::wall_container_t& CAVEConfig::getWalls() const
{
	return m_walls;
}

CAVEConfig::wall_container_t& CAVEConfig::getWalls()
{
	return m_walls;
}

CAVEConfig::CAVEUnitType CAVEConfig::getUnits() const
{
	return m_Units;
}

/*! parse a null terminated line of configuration data
 * @param line of text to parse
 * @return true if parsed ok
 */
bool CAVEConfig::parseLine(const std::string& line)
{
	// skip forward to first non white-space (tab or ' ') - ignoring any with # at start
	char* cline = static_cast<char*>(malloc(line.size() * sizeof(char) + 1));
	std::strcpy(cline, line.c_str());
	cline[line.size()] = '\0';
	if (!(cline = skipWhiteSpace(cline))) 
	{
		free(cline);
		return true;
	}

	// parse the config command, calling the respective function if found
	for(std::size_t i = 0; i < sizeof(cfgFunctions)/sizeof(cfgFunctions[0]); ++i)
	{
		int len=strlen(cfgFunctions[i].cfgParam);

		if (strncmp(cline, cfgFunctions[i].cfgParam, len) == 0) 
		{
			if (!cfgFunctions[i].cfgFunc(*this, cline+len)) {
				free(cline);
				return false;
			}
		}
	}

	free(cline);
	return true;
}

float convert_length(CAVEConfig::CAVEUnitType from, float value, CAVEConfig::CAVEUnitType to)
{
	return value * CAVEUnits[from].conversion_factor_to_cm / CAVEUnits[to].conversion_factor_to_cm;
}

}
