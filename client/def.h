#ifndef MSG_H
#define MSG_H

#include <cstddef>

const size_t PORT = 6999;

const size_t MSG_VOID		= 0x00000000UL;	
const size_t MSG_ONLINE		= 0x00000001UL;
const size_t MSG_OFFLINE	= 0x00000002UL;
const size_t MSG_CHAT		= 0x00000004UL;
const size_t MSG_LIST		= 0x00000008UL;

#endif
