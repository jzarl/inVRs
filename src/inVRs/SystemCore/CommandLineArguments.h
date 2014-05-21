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

#ifndef COMMANDLINEARGUMENTS_H_
#define COMMANDLINEARGUMENTS_H_

#include <string>
#include <vector>
#include <map>

#include "Platform.h"

/******************************************************************************
 * This class wraps the command line arguments into an object
 */
class INVRS_SYSTEMCORE_API CommandLineArguments {
public:

	/**
	 * Constructor initializing command line arguments with passed values
	 */
	CommandLineArguments(int argc, char** argv);

	/**
	 * Destructor
	 */
	~CommandLineArguments();

	/**
	 * Copy constructor
	 */
	CommandLineArguments(const CommandLineArguments& src);

	/**
	 * Returns the number of arguments
	 */
	int getArgc() const;

	/**
	 * Returns the pointer to the array of command line arguments
	 */
	char** getArgv();

private:
	void copyArgv(int argc, char** argv);

	CommandLineArguments& operator=(const CommandLineArguments& src);

	int argc;
	char** argv;
}; // CommandLineArguments

/******************************************************************************
 * This class wraps the command line arguments into an object
 */
class INVRS_SYSTEMCORE_API CommandLineArgumentWrapper {
public:
	/**
	 * Parses the passed command line arguments and stores the values
	 * internally. The class therefore parses the command line strings for
	 * options (beginning with a -) and normal arguments. Options are stored in
	 * the options list while normal arguments are stored in the argument list.
	 * The positioning of options in the arguments is not restricted, so
	 * options can be placed in front, at the end or in between standard
	 * arguments. The application command itself is kept in the arguments to be
	 * compatible to the usual command line argument indices.
	 */
	CommandLineArgumentWrapper(int argc, char** argv);

	/**
	 * Cleans up the arguments and options
	 */
	virtual ~CommandLineArgumentWrapper();

	/**
	 * Returns the number of arguments from the command line.
	 */
	int getNumberOfArguments() const;

	/**
	 * Returns the command line argument at the passed index.
	 */
	std::string getArgument(int index) const;

	/**
	 * Returns if the option was passed via the command line.
	 */
	bool containOption(std::string key) const;

	/**
	 * Returns the value of a command line option if existing
	 */
	std::string getOptionValue(std::string key) const;

	/**
	 * Returns the original command line (including options)
	 */
	CommandLineArguments getCommandLineArguments() const;

private:

	/**
	 * Parses the passed arguments and fills the object (see constructor)
	 */
	void parseArguments(int argc, char** argv);

	/**
	 * Searches in the passed argument if the option has a value (assignment)
	 * and stores the option and its value (if existing) in the optionMap
	 */
	void parseOption(std::string argument);

	CommandLineArguments originalArguments;

	std::vector<std::string> argumentList;
	std::map<std::string, std::string> optionMap;
};

#endif /* COMMANDLINEARGUMENTS_H_ */
