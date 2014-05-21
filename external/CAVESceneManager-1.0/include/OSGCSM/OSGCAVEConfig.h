#ifndef INVRS_EXTERNAL_CSM_OSGCAVECONFIG_H
#define INVRS_EXTERNAL_CSM_OSGCAVECONFIG_H

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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*! \file OSGCAVEConfig.h
 *
 *	Adrian Haffegee, Sept 2004
 *	Header file for CAVE Scene Manager (CSM) configurator class
 */

#include <vector>
#include <string>
#include "./csm_config.h"
#include "./OSGCAVEWall.h"

#define CSM_MAXCAVEWALLS 100

namespace OSGCSM
{

class CAVESCENEMANAGER_DLLMAPPING CAVEConfig {
	public:
		typedef int sensor_id_t;
		typedef std::vector<CAVEWall*> wall_container_t;
		typedef wall_container_t::size_type size_t;
		enum CAVEUnitType
		{  
			CAVEUnitFeet   =  0,
			CAVEUnitMeters,
			CAVEUnitInches,
			CAVEUnitCentimeters,
			CAVEUnit_Last
		};

		CAVEConfig();
		virtual ~CAVEConfig();

		bool loadFile(const char* cfgFile);
		void printConfig() const;
		bool checkConfig();

		void setFollowHead(bool follow_head);
		void setSensorIDHead(sensor_id_t sensor_id);
		void setSensorIDController(sensor_id_t sensor_id);
		void setStereo(bool useStereo);
		void setMCastServer(bool useMCast);
		void setBroadcastaddress(std::string ip);
		void setEyeSeparation(float eyeSep, CAVEUnitType units);
		void setTrackerKey(int key);
		void setControllerKey(int key);
		void setCAVEHeight(float height, CAVEUnitType units);
		void setCAVEWidth(float width, CAVEUnitType units);
		void setCAVEOrigin(OSG::Pnt3f origin, CAVEUnitType units);
		void setUnits(CAVEUnitType units);

		bool getFollowHead() const ;
		sensor_id_t getSensorIDHead() const ;
		sensor_id_t getSensorIDController() const;
		bool isStereo() const;
		bool isMCastServer() const;
		const std::string& getBroadcastaddress() const;
		float getEyeSeparation() const;
		int getTrackerKey() const;
		int getControllerKey() const;
		int getNumActiveWalls() const;
		const CAVEWall* getActiveWall(size_t wallNum) const;
		CAVEWall* editActiveWall(size_t wallNum);
		int getNumPipes() const;
		const wall_container_t& getWalls() const;
		wall_container_t& getWalls();
		CAVEUnitType getUnits() const;

	private:
		bool parseLine(const std::string& line);

		bool m_followHead;
		sensor_id_t m_SensorIDHead;
		sensor_id_t m_SensorIDController;
		std::string m_Broadcastaddress;
		bool m_stereo;
		bool m_mCastServer;
		float m_eyeSeparation;
		int m_trackerKey;
		int m_ControllerKey;
		wall_container_t m_walls;
		size_t m_activeWalls;
		CAVEUnitType m_origUnits;
		OSG::Pnt3f m_origin;
		CAVEUnitType m_heightUnits;
		float m_height;
		CAVEUnitType m_widthUnits;
		float m_width;
		int m_numPipes;
		CAVEUnitType m_Units;
};

CAVESCENEMANAGER_DLLMAPPING float convert_length(CAVEConfig::CAVEUnitType from, float value, CAVEConfig::CAVEUnitType to);

}


#endif
