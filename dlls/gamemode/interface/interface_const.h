
#ifndef INT_INTERFACE_H
#define INT_INTERFACE_H
#ifdef _WIN32
#pragma once
#endif


enum INTMessage : byte
{
	ZB3_REN_MSG,
	ZB3_USED_MSG,
	ORIG_START_MSG,
	ORIG_BOMB_MSG,
	ORIG_BOMB2_MSG,
	ORIG_CTWIN_MSG,
	ORIG_TRWIN_MSG,
	ORIG_RDRAW_MSG,
};
#endif
