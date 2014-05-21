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

#include "CommandLineArguments.h"

#include <assert.h>
#include <cstring>

#include "DebugOutput.h"

using namespace std;

CommandLineArguments::CommandLineArguments(int argc, char** argv)
: argc(argc) {
	copyArgv(argc, argv);
} // CommandLineArguments

CommandLineArguments::~CommandLineArguments() {
	for (int i=0; i < this->argc; i++) {
		delete[] argv[i];
	} // for
	delete[] argv;
} // ~CommandLineArguments

CommandLineArguments::CommandLineArguments(
		const CommandLineArguments& src)
: argc(src.argc) {
	copyArgv(src.argc, src.argv);
} // CommandLineArguments

int CommandLineArguments::getArgc() const {
	return argc;
} // getArgc

char** CommandLineArguments::getArgv() {
	return argv;
} // getArgv

void CommandLineArguments::copyArgv(int argc, char** argv)  {
	int length;
	this->argv = new char*[argc];
	for (int i=0; i < argc; i++) {
		length = strlen(argv[i]);
		this->argv[i] = new char[length+1];
		strcpy(this->argv[i], argv[i]);
	} // for
} // copyArgv

CommandLineArgumentWrapper::CommandLineArgumentWrapper(int argc, char** argv)
: originalArguments(argc, argv) {
	parseArguments(argc, argv);
} // CommandLineArgumentWrapper

CommandLineArgumentWrapper::~CommandLineArgumentWrapper() {
	argumentList.clear();
	optionMap.clear();
} // ~CommandLineArgumentWrapper

int CommandLineArgumentWrapper::getNumberOfArguments() const {
	return argumentList.size();
} // getNumberOfArguments

string CommandLineArgumentWrapper::getArgument(int index) const {
	if (index < 0 || index > (int)argumentList.size()) {
		printd(WARNING,
				"CommandLineArguments::getArgument(): index %i out of range (indices range from 0 to %i)!\n",
				index, argumentList.size());
		return "";
	} // if

	return argumentList[index];
} // getArgument

bool CommandLineArgumentWrapper::containOption(string key) const {
	return (optionMap.find(key) != optionMap.end());
} // containOption

string CommandLineArgumentWrapper::getOptionValue(string key) const {
	map<string, string>::const_iterator it;
	it = optionMap.find(key);

	if (it != optionMap.end()) {
		return it->second;
	} // if
	else {
		printd(WARNING,
				"CommandLineArguments::getOptionValue(): option %s not found in command line arguments!\n",
				key.c_str());
		return "";
	} // else
} // getOptionValue

CommandLineArguments CommandLineArgumentWrapper::getCommandLineArguments() const {
	return originalArguments;
} // getCommandLineArguments

void CommandLineArgumentWrapper::parseArguments(int argc, char** argv) {
	string argument;

	for (int i=0; i < argc; i++) {
		// argument should always exist in my opinion
		assert(argv[i]);
		argument = argv[i];
		// argument should always have at least one character
		assert(argument.size() > 0);
		if (argument[0] == '-')
			parseOption(argument);
		else
			argumentList.push_back(argument);
	} // for
} // parseArguments

void CommandLineArgumentWrapper::parseOption(string argument) {
	string key = "";
	string value = "";
	size_t firstEqualsSign = argument.find('=');

	if (firstEqualsSign == string::npos) {
		key = argument.substr(1);
		value = "";
	} else {
		key = argument.substr(1, firstEqualsSign-1);
		value = argument.substr(firstEqualsSign+1);
	} // else

	if (value.size() > 0) {
		if ((value[0] == '\"' && value[value.size()-1] == '\"') ||
			(value[0] == '\'' && value[value.size()-1] == '\'')) {
			value = value.substr(1, value.size()-2);
		} // if
	} // if

	optionMap[key] = value;
} // parseOption
