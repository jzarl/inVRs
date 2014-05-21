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

#ifndef _SYNCHRONISATIONMACROS_H
#define _SYNCHRONISATIONMACROS_H

/***
 * 1 Argument
 **/
#define DECLARE_SERVER_METHOD1(_method, _argType1) \
	protected: \
	void _method ## _sync(_argType1); \
	void _method ## _server(_argType1); \
	public: \
	void _method

#define SERVER_METHOD1(_class, _method, _argType1) void _class::_method(_argType1 _arg1) { \
	if (Physics::isServer()) \
		_class::_method ## _server(_arg1); \
	else \
		_class::_method ## _sync(_arg1); \
} \
void _class::_method ## _sync(_argType1 _arg1) { \
	NetMessage* msg = RPCObjectInterface::createSyncMessage(); \
	msgFunctions::encode(FUNCTION_ ## _method, msg); \
	msgFunctions::encode(_arg1, msg); \
	Physics::syncToServer(msg); \
	delete msg; \
} \
void _class::_method ## _server

/***
 * 2 Arguments
 **/
#define DECLARE_SERVER_METHOD2(_method, _argType1, _argType2) \
	protected: \
	void _method ## _sync(_argType1, _argType2); \
	void _method ## _server(_argType1, _argType2); \
	public: \
	void _method

#define SERVER_METHOD2(_class, _method, _argType1, _argType2) void _class::_method(_argType1 _arg1, _argType2 _arg2) { \
	if (Physics::isServer()) \
		_class::_method ## _server(_arg1, _arg2); \
	else \
		_class::_method ## _sync(_arg1, _arg2); \
} \
void _class::_method ## _sync(_argType1 _arg1, _argType2 _arg2) { \
	NetMessage* msg = RPCObjectInterface::createSyncMessage(); \
	msgFunctions::encode(FUNCTION_ ## _method, msg); \
	msgFunctions::encode(_arg1, msg); \
	msgFunctions::encode(_arg2, msg); \
	Physics::syncToServer(msg); \
	delete msg; \
} \
void _class::_method ## _server

/***
 * 3 Arguments
 **/
#define DECLARE_SERVER_METHOD3(_method, _argType1, _argType2, _argType3) \
	protected: \
	void _method ## _sync(_argType1, _argType2, _argType3); \
	void _method ## _server(_argType1, _argType2, _argType3); \
	public: \
	void _method

#define SERVER_METHOD3(_class, _method, _argType1, _argType2, _argType3) void _class::_method(_argType1 _arg1, _argType2 _arg2, _argType3 _arg3) { \
	if (Physics::isServer()) \
		_class::_method ## _server(_arg1, _arg2, _arg3); \
	else \
		_class::_method ## _sync(_arg1, _arg2, _arg3); \
} \
void _class::_method ## _sync(_argType1 _arg1, _argType2 _arg2, _argType3 _arg3) { \
	NetMessage* msg = RPCObjectInterface::createSyncMessage(); \
	msgFunctions::encode(FUNCTION_ ## _method, msg); \
	msgFunctions::encode(_arg1, msg); \
	msgFunctions::encode(_arg2, msg); \
	msgFunctions::encode(_arg3, msg); \
	Physics::syncToServer(msg); \
	delete msg; \
} \
void _class::_method ## _server

/***
 * 4 Arguments
 **/
#define DECLARE_SERVER_METHOD4(_method, _argType1, _argType2, _argType3, _argType4) \
	protected: \
	void _method ## _sync(_argType1, _argType2, _argType3, _argType4); \
	void _method ## _server(_argType1, _argType2, _argType3, _argType4); \
	public: \
	void _method

#define SERVER_METHOD4(_class, _method, _argType1, _argType2, _argType3, _argType4) void _class::_method(_argType1 _arg1, _argType2 _arg2, _argType3 _arg3, _argType4 _arg4) { \
	if (Physics::isServer()) \
		_class::_method ## _server(_arg1, _arg2, _arg3, _arg4); \
	else \
		_class::_method ## _sync(_arg1, _arg2, _arg3, _arg4); \
} \
void _class::_method ## _sync(_argType1 _arg1, _argType2 _arg2, _argType3 _arg3, _argType4 _arg4) { \
	NetMessage* msg = RPCObjectInterface::createSyncMessage(); \
	msgFunctions::encode(FUNCTION_ ## _method, msg); \
	msgFunctions::encode(_arg1, msg); \
	msgFunctions::encode(_arg2, msg); \
	msgFunctions::encode(_arg3, msg); \
	msgFunctions::encode(_arg4, msg); \
	Physics::syncToServer(msg); \
	delete msg; \
} \
void _class::_method ## _server


#endif // _SYNCHRONISATIONMACROS_H
