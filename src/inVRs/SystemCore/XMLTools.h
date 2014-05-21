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

#ifndef INVRS_SYSTEMCORE_XMLTOOLS_H
#define INVRS_SYSTEMCORE_XMLTOOLS_H

#include <sstream>
#include <irrXML.h>

#include <string.h>

#include "DataTypes.h"
#include "ArgumentVector.h"
#include "Platform.h"
#include "XmlElement.h"
#include "XmlConfigurationLoader.h"

using namespace irr;
using namespace io;

class ModelInterface;

enum OBJECTTYPE {
	OBJECTTYPE_UNDEFINED, OBJECTTYPE_ENTITY, OBJECTTYPE_TILE, OBJECTTYPE_AVATAR, OBJECTTYPE_CURSOR
};

/**
 *
 */
//class XmlArgumentVectorConverter {
//public:
//	static bool convert(XmlElement* element, const Version& srcVersion, const Version& dstVersion);
//}; // XmlArgumentVectorConverter

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlArgumentVectorLoader : public XmlConfigurationLoader {
public:
	static XmlConfigurationLoader& get();

private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4
}; // XmlArgumentVectorLoader

/**
 *
 */
class INVRS_SYSTEMCORE_API XmlRepresentationLoader : public XmlConfigurationLoader {
public:
	static XmlConfigurationLoader& get();

private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4
}; // XmlRepresentationLoader


/**
 *
 */
class INVRS_SYSTEMCORE_API XmlTransformationLoader : public XmlConfigurationLoader {
public:
	static XmlConfigurationLoader& get();

private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4
}; // XmlTransformationLoader

/** checks xml stream for elements translation, rotation, scale and reads the appropriate float attributes
* xml example:
* 		<translation x="0" y="0" z="0"/>
* 		<rotation x="1" y="0" z="0" angle="45"/> angle in degree
* 		<scale x="0.5" y="0.5" z="0.5"/>
* only the element read will be overwritten!
*/
INVRS_SYSTEMCORE_API bool readTransformationDataFromXmlElement(TransformationData& dst,
		const XmlElement* element);

INVRS_SYSTEMCORE_API XmlElement* createXmlElementFromTransformationData(const TransformationData& src); 

INVRS_SYSTEMCORE_API XmlElement* createXmlElementFromModelInterface(ModelInterface* src);

/**
 * @deprecated
 */
INVRS_SYSTEMCORE_API bool readTransformationDataFromXML(TransformationData& dst, IrrXMLReader* xml,
		const char* endTag);

/**
 *
 */
INVRS_SYSTEMCORE_API ModelInterface* readRepresentationFromXmlElement(const XmlElement* element,
		OBJECTTYPE type);

/**
 * @deprecated
 */
INVRS_SYSTEMCORE_API ModelInterface* readRepresentationFromXML(IrrXMLReader* xml, OBJECTTYPE type);

/**
 * Returns a list of pointers to parsed attributes of an xml-element.
 * xml example:
 *	<xxx attInt="2" attBool="true" attString="Selection" attFloat="3.1415"/>
 * returns an array of pointers to the parsed attributes in order
 * supported types:	attBool
 *				attInt
 *				attFloat
 *				attString
 */
INVRS_SYSTEMCORE_API ArgumentVector* readArgumentsFromXmlElement(const XmlElement* element);

/**
 * @deprecated
 */
INVRS_SYSTEMCORE_API ArgumentVector* readArgumentsFromXML(IrrXMLReader* xml,
		std::string endTag = "");
//ArgumentVector* readArgumentsFromXML(IrrXMLReader* xml, int counter = 0, ...);

template <class T> inline std::string toString(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline bool& convert(std::string& src, bool& dst) {
	// TODO: build a better Converter
	if (src == "true" || src == "TRUE" || src == "True")
		dst = true;
	else
		dst = false;
	return dst;
} // convert

inline bool& convert(const char* src, bool& dst) {
	if (src == NULL)
		return dst;

	if (!strcmp(src, "true") || !strcmp(src, "TRUE") || !strcmp(src, "True"))
		dst = true;
	else
		dst = false;
	return dst;
} // convert

INVRS_SYSTEMCORE_API bool stringEqualsAny(std::string& src);
INVRS_SYSTEMCORE_API bool fileExists(const std::string& file);

#endif

