#ifndef INVRS_SYSTEMCORE_UTILITYFUNCTIONS_H
#define INVRS_SYSTEMCORE_UTILITYFUNCTIONS_H

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

#include <vector>
#include <string>
#include "./Platform.h"

inline const std::string getPathSeparator()
{
#ifdef WIN32
	return ";";
#else
	return ":";
#endif
}

inline const std::string getFileSeparator()
{
#ifdef WIN32
	return "\\";
#else
	return "/";
#endif
}

INVRS_SYSTEMCORE_API std::vector<std::string> tokenizeString(const std::string& theString, const std::string &delimiters);

INVRS_SYSTEMCORE_API std::string getConcatenatedPath(const std::string& path, const std::string& configurationPathKey);

#endif
