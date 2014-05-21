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
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _CONFIGCREATOR_H
#define _CONFIGCREATOR_H

#include <string>

#include <inVRs/SystemCore/XmlElement.h>

class ConfigCreator{
public:
  static void createBasicConfig(std::string invrsPath, std::string projectPath);
private:
  static void createGeneralXml(std::string path, std::string invrsPath);
  static void createSystemCoreXml(std::string path);
  static void createWorldDatabaseXml(std::string path);
  static void createEntitiesXml(std::string path);
  static void createEnvironmentXml(std::string path);
  static void createEnvironmentLayoutXml(std::string path);
  static void createTilesXml(std::string path);
  static void createOutputInterfaceXml(std::string path);
  static void createModifiersXml(std::string path);
  static void createUserDatabaseXml(std::string path);

  static XmlElement* createXmlElemPath(std::string name, std::string dir);
};

#endif
