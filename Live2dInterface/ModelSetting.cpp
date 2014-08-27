#include "ModelSetting.h"
#include "Model.h"

const char * BKL2D_ModelSetting::NAME = "name";
const char * BKL2D_ModelSetting::ID = "id";
const char * BKL2D_ModelSetting::MODEL = "model";
const char * BKL2D_ModelSetting::TEXTURES = "textures";
const char * BKL2D_ModelSetting::HIT_AREAS = "hit_areas";
const char * BKL2D_ModelSetting::PHYSICS = "physics";
const char * BKL2D_ModelSetting::POSE = "pose";
const char * BKL2D_ModelSetting::EXPRESSIONS = "expressions";
const char * BKL2D_ModelSetting::MOTION_GROUPS = "motions";
const char * BKL2D_ModelSetting::SOUND = "sound";
const char * BKL2D_ModelSetting::LIP_SYNC = "lip_sync";
const char * BKL2D_ModelSetting::FADE_IN = "fade_in";
const char * BKL2D_ModelSetting::FADE_OUT = "fade_out";

const char * BKL2D_ModelSetting::VALUE = "val";
const char * BKL2D_ModelSetting::FILE = "file";
const char * BKL2D_ModelSetting::INIT_PARTS_VISIBLE = "init_parts_visible";
const char * BKL2D_ModelSetting::INIT_PARAM = "init_param";
const char * BKL2D_ModelSetting::LAYOUT = "layout";

bool BKL2D_ModelSetting::init(const char *file)
{
	uint32_t s;
	uint8_t *d = readDataFromFile(file, &s);
	if (!d)
	{
		return false;
	}

	Json *p = Json::parseFromBytes((const char *)d, s);
	freeData(d);
	LDString type = p->getRoot()["type"].toString();
	if (type != "Live2D Model Setting")
	{
		delete p;
		return false;
	}
	doc = p;
	json = &p->getRoot();
	return true;
}