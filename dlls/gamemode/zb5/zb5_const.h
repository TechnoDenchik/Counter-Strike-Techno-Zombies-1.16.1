#ifndef ZB5_CONST_H
#define ZB5_CONST_H
#ifdef _WIN32
#pragma once
#endif

enum ZB5HumanMoraleType_e : byte
{
	ZB5_MORALE_DEFAULT = 0,
	ZB5_MORALE_STRENGTHEN = 1
};

enum ZB5MessageType : byte
{
	ZB5_MESSAGE_KILL, 
	ZB5_MESSAGE_RAGE,
	ZB5_MESSAGE_EVOLUTION,
	ZB5_EXTERN,
};

#endif
