#pragma once

#ifdef BKE_DLL
#undef BKE_DLL
#endif

#if defined(_USRDLL)
   #define BKE_DLL     extern "C" __declspec(dllexport)
#else         /* use a DLL library */
#define BKE_DLL     extern "C" __declspec(dllimport)
#endif

#include <cstdint>

typedef uint8_t *(*f_readDataFromFile)(const char *file, uint32_t *size);
typedef void (*f_freeData)(uint8_t *data);
//·µ»ØtextureID
typedef uint32_t(*f_loadTextureFromFile)(const char *file);
typedef void(*f_freeTexture)(uint32_t);

typedef bool (*f_playSound)(int32_t channel, const char *file, bool calcRealTimeVolume);
typedef void (*f_stopSound)(int32_t channel);
typedef bool (*f_getRealTimeVolumeOfSound)(int32_t channel, float* volume);

typedef void(*f_sendError)(const char *err);

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	f_readDataFromFile _readDataFromFile;
	f_freeData _freeData;
	
	f_loadTextureFromFile _loadTextureFromFile;
	f_freeTexture _freeTexture;

	f_playSound _playSound;
	f_stopSound _stopSound;
	f_getRealTimeVolumeOfSound _getRealTimeVolumeOfSound;

	f_sendError _sendError;
} bkl2d_init_struct;
#pragma pack(pop)

BKE_DLL void bkl2d_globalInit(bkl2d_init_struct *s);

BKE_DLL void bkl2d_globalDestroy();

BKE_DLL int32_t bkl2d_newModel(const char *file);

BKE_DLL void bkl2d_getModelCanvasSize(int32_t id, float *w, float *h);

BKE_DLL void bkl2d_freeModel(int32_t id);

BKE_DLL void bkl2d_setMatrix(int32_t id, float *matrix);

BKE_DLL void bkl2d_draw(int32_t id);

BKE_DLL void bkl2d_setTap(int32_t id, float x, float y);

BKE_DLL void bkl2d_setDrag(int32_t id, float x, float y);

BKE_DLL void bkl2d_setFlick(int32_t id, float x, float y);


