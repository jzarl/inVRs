#ifndef _CONFIG_H_
#define _CONFIG_H_

/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer                              *
 *   thomas_weberndorfer@hotmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*
 * This file contains a few #defines for logging-output.
 *
 * Replace the macros with empty macros to avoid any output of the 3dMenu!
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 */

#include <iostream>

#define WAR(text)     std::cout << "(WW) " << text << std::endl;
#define ERR(text)     std::cerr << "(EE) " << text << std::endl;
#define MSG(text)     std::cout << " +-  " << text << std::endl;
#define INF(text)     std::cout << "(II) " << text << std::endl;

#endif
