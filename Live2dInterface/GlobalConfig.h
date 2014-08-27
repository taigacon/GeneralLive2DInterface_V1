#pragma once

#include "Interface.h"
#include "type/LDString.h"

extern f_readDataFromFile readDataFromFile;
extern f_freeData freeData;
extern f_loadTextureFromFile loadTextureFromFile;
extern f_freeTexture freeTexture;
extern f_playSound playSound;
extern f_stopSound stopSound;
extern f_getRealTimeVolumeOfSound getRealTimeVolumeOfSound;
extern f_sendError sendError;

inline bool operator != (const live2d::LDString &l, const live2d::LDString &r)
{
	return !(l == r);
}

inline bool operator != (const live2d::LDString &l, const char *r)
{
	return !(l == r);
}


