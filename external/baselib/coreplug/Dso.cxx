/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2013      by the Johannes Kepler University, Linz        *
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

#include "Dso.h"
#include <string>
#include <iostream>

#ifndef WIN32
	#include <dlfcn.h> /* for dlopen, dlsym, etc. */
#endif

class Dso::DsoPrivate
{
public:
	explicit DsoPrivate(const std::string &path);
	~DsoPrivate();
	void updateErrorMessage();

	dsohandle_t dso;
	std::string dsopath;
	std::string errormsg;
private:
	DsoPrivate(const DsoPrivate&);
	DsoPrivate& operator=(const DsoPrivate&);
};

/* ***************** DsoPrivate defininitions:  ***************** */

Dso::DsoPrivate::DsoPrivate(const std::string &path)
	: dso(NULL), dsopath(path), errormsg()
{
	// open the dso
#ifdef WIN32
	// Helmut: '/' must be converted into '\' (claimed by the msdn docu)
	size_t pos = dsopath.find_first_of("/");
	while (pos!=std::string::npos)
	{  
		dsopath[pos]='\\';
		pos=dsopath.find_first_of("/",pos+1);
	}
	dso = LoadLibraryA(dsopath.c_str());
#else
	dso = dlopen(path.c_str(), RTLD_NOW);
#endif
	updateErrorMessage();
}

Dso::DsoPrivate::~DsoPrivate()
{
	if (dso)
	{
#if WIN32
		FreeLibrary(dso);
#else
		dlclose(dso);
#endif
	}
}

/**
 * Overwrites the errormsg with the current value.
 */
void Dso::DsoPrivate::updateErrorMessage()
{
#if WIN32
	DWORD errorType = GetLastError();
	char error[1024] = "";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorType, LANG_SYSTEM_DEFAULT, error, 1024, 0);
	errormsg = error;
#else
	char *error = dlerror();
	if (error)
		errormsg = error;
	else
		errormsg = std::string();
#endif
}


/* ***************** Dso definitions:           ***************** */

Dso::Dso()
: state(0)
{
}

Dso::~Dso()
{
	delete state;
}

bool Dso::open(const std::string &path)
{
	if (state)
		delete state;
	state = new DsoPrivate(path);
	return state->errormsg.empty();
}

bool Dso::isValid() const
{
	return (NULL != state) && (NULL != state->dso);
}

std::string Dso::lastError() const
{
	if (state)
		return state->errormsg;
	return "No DSO open.";
}

dsosymbol_t Dso::rawSymbol(const std::string &symbol) const
{
	dsosymbol_t sym = 0;
	if (isValid())
	{
#if WIN32
		sym = GetProcAddress(state->dso,symbol.c_str());
#else
		sym = dlsym(state->dso,symbol.c_str());
#endif
		state->updateErrorMessage();
	}
	return sym;
}

dsohandle_t Dso::rawHandle() const
{
	if (isValid())
		return state->dso;
	return 0;
}


#if !defined(SHARED_MODULE_PREFIX) || !defined(SHARED_MODULE_SUFFIX)
# error "SHARED_MODULE_PREFIX and SHARED_MODULE_SUFFIX must be provided by the build system!"
#endif

const char* platformDsoPrefix()
{
	return SHARED_MODULE_PREFIX;
}

const char* platformDsoSuffix()
{
	return SHARED_MODULE_SUFFIX;
}
