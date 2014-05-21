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

#ifndef _ARGUMENTVECTOR_H
#define _ARGUMENTVECTOR_H

#include <string>
#include <map>

#include "NetMessage.h"

/******************************************************************************
 * An ArgumentVector provides a map-like interface for variable datatypes.
 * You can associate bool, int, unsigned int, float and std::string values
 * with a key of type std::string, and query for existance of specific keys.
 */
class INVRS_SYSTEMCORE_API ArgumentVector {
public:
	~ArgumentVector();
	void push_back(std::string key, bool value);
	void push_back(std::string key, int value);
	void push_back(std::string key, unsigned int value);
	void push_back(std::string key, float value);
	void push_back(std::string key, std::string value);

	int size() const;
	// 	Argument* operator[] (std::string index);
	/**
	 * Query whether a value is associated with the given key.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool keyExists(std::string key) const;

	/**
	 * Write the boolean value associated with key into <code>val</code>.
	 * If the key does not exist <code>val</code> is not altered.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool get(std::string key, bool& val) const;

	/**
	 * Write the integer value associated with key into <code>val</code>.
	 * If the key does not exist <code>val</code> is not altered.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool get(std::string key, int& val) const;

	/**
	 * Write the unsigned integer  value associated with key into <code>val</code>.
	 * If the key does not exist <code>val</code> is not altered.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool get(std::string key, unsigned int& val) const;

	/**
	 * Write the float value associated with key into <code>val</code>.
	 * If the key does not exist <code>val</code> is not altered.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool get(std::string key, float& val) const;

	/**
	 * Write the string value associated with key into <code>val</code>.
	 * If the key does not exist <code>val</code> is not altered.
	 * @param key the key
	 * @return <code>true</code>, if key exists, <code>false</code> otherwise.
	 */
	bool get(std::string key, std::string& val) const;

	/**
	 * Serialise the ArgumentVector and append it to a NetMessage.
	 */
	void addToBinaryMessage(NetMessage* msg) const;

	/**
	 * Deserialise an ArgumentVector from a NetMessage.
	 */
	static ArgumentVector* readFromBinaryMessage(NetMessage* msg);

private:

	/**
	 * Nested class for storing Argument values. The class allows to store the
	 * type and value of an argument.
	 */
	class Argument {
	public:
		/// Enum for possible argument types
		enum ARGUMENT_TYPE {
			BOOL, INTEGER, UINTEGER, FLOAT, STRING
		};

		// Constructors for possible argument types
		Argument(bool value);
		Argument(int value);
		Argument(unsigned int value);
		Argument(float value);
		Argument(std::string value);

		// copy constructor
		Argument(const Argument& src);

		~Argument();

		// getters for accessing the argument type and argument value
		const ARGUMENT_TYPE getType() const;
		const int& get(int& dst) const;
		const unsigned int& get(unsigned int& dst) const;
		const bool& get(bool& dst) const;
		const float& get(float& dst) const;
		const std::string& get(std::string& dst) const;

	private:
		// copy assignment operator
		Argument& operator=(const Argument& src);

		ARGUMENT_TYPE type;
		void* value;
	}; // Argument


	std::map<std::string, Argument> arguments;
}; // ArgumentVector

#endif // _ARGUMENTVECTOR_H
