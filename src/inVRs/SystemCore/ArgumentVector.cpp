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

#include <assert.h>

#include "ArgumentVector.h"
#include "DebugOutput.h"
#include "MessageFunctions.h"


ArgumentVector::~ArgumentVector() {
	arguments.clear();
} // ArgumentVector

void ArgumentVector::push_back(std::string key, bool value) {
	if (keyExists(key)) {
		printd(WARNING,
				"ArgumentVector::push_back(): inserting duplicate entry for key %s! First entry will be removed!\n",
				key.c_str());
		arguments.erase(key);
	} // if
	arguments.insert(make_pair(key, Argument(value)));
} // push_back

void ArgumentVector::push_back(std::string key, int value) {
	if (keyExists(key)) {
		printd(WARNING,
				"ArgumentVector::push_back(): inserting duplicate entry for key %s! First entry will be removed!\n",
				key.c_str());
		arguments.erase(key);
	} // if
	arguments.insert(make_pair(key, Argument(value)));
} // push_back

void ArgumentVector::push_back(std::string key, unsigned int value) {
	if (keyExists(key)) {
		printd(WARNING,
				"ArgumentVector::push_back(): inserting duplicate entry for key %s! First entry will be removed!\n",
				key.c_str());
		arguments.erase(key);
	} // if
	arguments.insert(make_pair(key, Argument(value)));
} // push_back

void ArgumentVector::push_back(std::string key, float value) {
	if (keyExists(key)) {
		printd(WARNING,
				"ArgumentVector::push_back(): inserting duplicate entry for key %s! First entry will be removed!\n",
				key.c_str());
		arguments.erase(key);
	} // if
	arguments.insert(make_pair(key, Argument(value)));
} // push_back

void ArgumentVector::push_back(std::string key, std::string value) {
	if (keyExists(key)) {
		printd(WARNING,
				"ArgumentVector::push_back(): inserting duplicate entry for key %s! First entry will be removed!\n",
				key.c_str());
		arguments.erase(key);
	} // if
	arguments.insert(make_pair(key, Argument(value)));
} // push_back

int ArgumentVector::size() const {
	return arguments.size();
} // size

bool ArgumentVector::keyExists(std::string key) const {
	return (arguments.find(key) != arguments.end());
} // keyExists

bool ArgumentVector::get(std::string key, bool& val) const {
	std::map<std::string, Argument>::const_iterator it;
	it = arguments.find(key);
	if (it == arguments.end())
		return false;
	const Argument& arg = it->second;
	arg.get(val);
	return true;
} // get

bool ArgumentVector::get(std::string key, int& val) const {
	std::map<std::string, Argument>::const_iterator it;
	it = arguments.find(key);
	if (it == arguments.end())
		return false;
	const Argument& arg = it->second;
	arg.get(val);
	return true;
}

bool ArgumentVector::get(std::string key, unsigned int& val) const {
	std::map<std::string, Argument>::const_iterator it;
	it = arguments.find(key);
	if (it == arguments.end())
		return false;
	const Argument& arg = it->second;
	arg.get(val);
	return true;
} // get

bool ArgumentVector::get(std::string key, float& val) const {
	std::map<std::string, Argument>::const_iterator it;
	it = arguments.find(key);
	if (it == arguments.end())
		return false;
	const Argument& arg = it->second;
	arg.get(val);
	return true;
}

bool ArgumentVector::get(std::string key, std::string& val) const {
	std::map<std::string, Argument>::const_iterator it;
	it = arguments.find(key);
	if (it == arguments.end())
		return false;
	const Argument& arg = it->second;
	arg.get(val);
	return true;
}

void ArgumentVector::addToBinaryMessage(NetMessage* msg) const {
	bool tempBool;
	float tempFloat;
	int tempInt;
	unsigned int tempUInt;
	std::string tempString;
	std::map<std::string, Argument>::const_iterator it;

	msgFunctions::encode((int)size(), msg);

	for (it = arguments.begin(); it != arguments.end(); ++it) {
		msgFunctions::encode(it->first, msg);
		const Argument& arg = it->second;
		msgFunctions::encode((unsigned)arg.getType(), msg);
		switch (arg.getType()) {
		case Argument::BOOL:
			arg.get(tempBool);
			msgFunctions::encode(tempBool, msg);
			break;

		case Argument::INTEGER:
			arg.get(tempInt);
			msgFunctions::encode(tempInt, msg);
			break;

		case Argument::UINTEGER:
			arg.get(tempUInt);
			msgFunctions::encode(tempUInt, msg);
			break;

		case Argument::FLOAT:
			arg.get(tempFloat);
			msgFunctions::encode(tempFloat, msg);
			break;

		case Argument::STRING:
			arg.get(tempString);
			msgFunctions::encode(tempString, msg);
			break;

		default:
			assert(false);
		} // switch
	} // for
} // addToBinaryMessage

ArgumentVector* ArgumentVector::readFromBinaryMessage(NetMessage* msg) {
	int i, maxi;
	ArgumentVector* ret;
	unsigned argType;
	std::string key;
	bool tempBool;
	float tempFloat;
	int tempInt;
	unsigned int tempUInt;
	std::string tempString;

	ret = new ArgumentVector();
	msgFunctions::decode(maxi, msg);

	for (i = 0; i < maxi; i++) {
		msgFunctions::decode(key, msg);
		msgFunctions::decode(argType, msg);
		switch (argType) {
		case Argument::BOOL:
			msgFunctions::decode(tempBool, msg);
			ret->push_back(key, tempBool);
			break;

		case Argument::INTEGER:
			msgFunctions::decode(tempInt, msg);
			ret->push_back(key, tempInt);
			break;

		case Argument::UINTEGER:
			msgFunctions::decode(tempUInt, msg);
			ret->push_back(key, tempUInt);
			break;

		case Argument::FLOAT:
			msgFunctions::decode(tempFloat, msg);
			ret->push_back(key, tempFloat);
			break;

		case Argument::STRING:
			msgFunctions::decode(tempString, msg);
			ret->push_back(key, tempString);
			break;

		default:
			assert(false);
		} // switch
	} // for

	return ret;
} // readFromBinaryMessage

ArgumentVector::Argument::Argument(bool value) :
	type(Argument::BOOL),
	value(new bool(value))
{}

ArgumentVector::Argument::Argument(int value) :
	type(Argument::INTEGER),
	value(new int(value))
{}

ArgumentVector::Argument::Argument(unsigned int value) :
	type(Argument::UINTEGER),
	value(new unsigned int (value))
{}

ArgumentVector::Argument::Argument(float value) :
	type(Argument::FLOAT),
	value(new float(value))
{}

ArgumentVector::Argument::Argument(std::string value) :
	type(Argument::STRING),
	value(new std::string(value))
{}

ArgumentVector::Argument::Argument(const Argument& src) :
	type(src.type) {
	switch(type) {
	case Argument::BOOL:
		value = new bool(*((bool*)src.value));
		break;
	case Argument::INTEGER:
		value = new int(*((int*)src.value));
		break;
	case Argument::UINTEGER:
		value = new unsigned int(*((unsigned int*)src.value));
		break;
	case Argument::FLOAT:
		value = new float(*((float*)src.value));
		break;
	case Argument::STRING:
		value = new std::string(*((std::string*)src.value));
		break;
	default:
		assert(false);
		break;
	} // switch
} // Argument

ArgumentVector::Argument::~Argument() {
	if (type == Argument::BOOL)
		delete ((bool*)value);
	else if (type == Argument::INTEGER)
		delete ((int*)value);
	else if (type == Argument::UINTEGER)
		delete ((unsigned int*)value);
	else if (type == Argument::FLOAT)
		delete ((float*)value);
	else if (type == Argument::STRING)
		delete ((std::string*)value);
} // Argument

const ArgumentVector::Argument::ARGUMENT_TYPE ArgumentVector::Argument::getType() const {
	return type;
} // getType

const int& ArgumentVector::Argument::get(int& dst) const {
	if (type != Argument::INTEGER)
		printd(WARNING, "Argument::get(): WARNING: conversion from non-integer to integer type!\n");
	dst = *((int*)value);
	return dst;
} // get

const unsigned int& ArgumentVector::Argument::get(unsigned int& dst) const {
	if (type != Argument::UINTEGER)
		printd(WARNING,
				"Argument::get(): WARNING: conversion from non unsigned integer to unsigned integer type!\n");
	dst = *((unsigned int*)value);
	return dst;
} // get

const bool& ArgumentVector::Argument::get(bool& dst) const {
	if (type != Argument::BOOL)
		printd(WARNING, "Argument::get(): WARNING: conversion from non-boolean to boolean type!\n");
	dst = *((bool*)value);
	return dst;
} // get

const float& ArgumentVector::Argument::get(float& dst) const {
	if (type != Argument::FLOAT)
		printd(WARNING, "Argument::get(): WARNING: conversion from non-float to float type!\n");
	dst = *((float*)value);
	return dst;
} // get

const std::string& ArgumentVector::Argument::get(std::string& dst) const {
	if (type != Argument::STRING)
		printd(WARNING, "Argument::get(): WARNING: conversion from non-string to string type!\n");
	dst = *((std::string*)value);
	return dst;
} // get
