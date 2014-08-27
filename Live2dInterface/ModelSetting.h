#pragma once

#include "util/Json.h"
#include "GlobalConfig.h"
#include <string>
#include <vector>
#include <map>

using namespace live2d;
using namespace std;

class BKL2D_ModelSetting
{
private:
	Value *json;
	Json * doc;
	static const char * NAME;
	static const char * ID;
	static const char * MODEL;
	static const char * TEXTURES;
	static const char * HIT_AREAS;
	static const char * PHYSICS;
	static const char * POSE;
	static const char * EXPRESSIONS;
	static const char * MOTION_GROUPS;
	static const char * SOUND;
	static const char * LIP_SYNC;
	static const char * FADE_IN;
	static const char * FADE_OUT;

	static const char * VALUE;
	static const char * FILE;
	static const char * INIT_PARTS_VISIBLE;
	static const char * INIT_PARAM;
	static const char * LAYOUT;

public:
	BKL2D_ModelSetting(){
		doc = NULL;
	}
	~BKL2D_ModelSetting(){
		delete doc;
	}
	bool init(const char *file);

	bool existMotion(const LDString &name) const {
		return !((*json)[MOTION_GROUPS][name].isNull()); 
	}//(*json).motion_group[name]
	bool existMotionSound(const LDString &name, int n) const {
		return !((*json)[MOTION_GROUPS][name][n][SOUND].isNull()); 
	}
	bool existMotionLipSync(const LDString &name, int n) const {
		return !((*json)[MOTION_GROUPS][name][n][LIP_SYNC].isNull());
	}
	bool existMotionFadeIn(const LDString &name, int n) const {
		return !(*json)[MOTION_GROUPS][name][n][FADE_IN].isNull();
	}
	bool existMotionFadeOut(const LDString &name, int n) const {
		return !(*json)[MOTION_GROUPS][name][n][FADE_OUT].isNull();
	}

	LDString getModelName() const
	{
		if ((*json)[NAME].isNull()) return LDString();
		return (*json)[NAME].toString();
	}

	LDString getModelFile() const
	{
		if ((*json)[MODEL].isNull()) return LDString();
		return (*json)[MODEL].toString();
	}

	int getTextureNum() const
	{
		if ((*json)[TEXTURES].isNull()) return 0;
		return (*json)[TEXTURES].getVector()->size();//(*json).textures.length
	}

	LDString getTextureFile(int n) const
	{
		return (*json)[TEXTURES][n].toString();//(*json).textures[n]
	}

	int getHitAreasNum() const
	{
		if ((*json)[HIT_AREAS].isNull()) return 0;
		return (*json)[HIT_AREAS].getVector()->size();//(*json).hit_area.length
	}

	LDString getHitAreaID(int n) const
	{
		return (*json)[HIT_AREAS][n][ID].toString();//(*json).hit_area[n].id
	}

	LDString getHitAreaName(int n) const
	{
		return (*json)[HIT_AREAS][n][NAME].toString();//(*json).hit_area[n].name
	}

	LDString getPhysicsFile() const
	{
		if ((*json)[PHYSICS].isNull()) return LDString();
		return (*json)[PHYSICS].toString();
	}

	LDString getPoseFile() const
	{
		if ((*json)[POSE].isNull()) return LDString();
		return (*json)[POSE].toString();
	}

	int getMotionNum(const LDString &name) const
	{
		if (!existMotion(name))return 0;
		return (*json)[MOTION_GROUPS][name].getVector()->size();//(*json).motion_group[name].length
	}

	LDString getMotionFile(const LDString &name, int n) const
	{
		if (!existMotion(name))return LDString();
		return (*json)[MOTION_GROUPS][name][n][FILE].toString();//(*json).motion_group[name][n].file
	}

	LDString getMotionSound(const LDString &name, int n) const
	{
		if (!existMotionSound(name, n))return LDString();
		return (*json)[MOTION_GROUPS][name][n][SOUND].toString();//(*json).motion_group[name][n].sound
	}

	bool getMotionLipSync(const LDString &name, int n) const
	{
		if (!existMotionLipSync(name, n))return false;
		return (*json)[MOTION_GROUPS][name][n][LIP_SYNC].toBoolean();//(*json).motion_group[name][n].lip_sync
	}

	int getMotionFadeIn(const LDString &name, int n) const
	{
		return (!existMotionFadeIn(name, n)) ? 1000 : (*json)[MOTION_GROUPS][name][n][FADE_IN].toInt();//(*json).motion_group[name][n].fade_in
	}

	int getMotionFadeOut(const LDString &name, int n) const
	{
		return (!existMotionFadeOut(name, n)) ? 1000 : (*json)[MOTION_GROUPS][name][n][FADE_OUT].toInt();//(*json).motion_group[name][n].fade_out
	}

	vector<LDString> getMotionGroupNames() const
	{
		vector<LDString> rtn;
		if ((*json)[MOTION_GROUPS].isNull()) return rtn;
		auto m = (*json)[MOTION_GROUPS].getMap();
		for (auto it = m->begin(); it != m->end(); it++)
		{
			rtn.push_back((*it).first);
		}
		return rtn;
	}
	
	map<LDString, float> getLayout() const
	{
		map<LDString, float> rtn;
		if ((*json)[LAYOUT].isNull())return rtn;
		auto m = (*json)[MOTION_GROUPS].getMap();
		for (auto it = m->begin(); it != m->end(); it++)
		{
			rtn[(*it).first] = (*it).second->toDouble();
		}
		return rtn;
	}
	
	int getInitParamNum() const
	{
		if ((*json)[INIT_PARAM].isNull()) return 0;
		return (*json)[INIT_PARAM].getVector()->size();
	}

	double getInitParamValue(int n) const
	{
		return (*json)[INIT_PARAM][n][VALUE].toDouble();
	}
	
	LDString getInitParamID(int n) const
	{
		return (*json)[INIT_PARAM][n][ID].toString();
	}

	int getInitPartsVisibleNum() const
	{
		if ((*json)[INIT_PARTS_VISIBLE].isNull()) return 0;
		return (*json)[INIT_PARTS_VISIBLE].getVector()->size();
	}

	double getInitPartsVisibleValue(int n) const
	{
		return (*json)[INIT_PARTS_VISIBLE][n][VALUE].toDouble();
	}

	LDString getInitPartsVisibleID(int n) const
	{
		return (*json)[INIT_PARTS_VISIBLE][n][ID].toString();
	}

	int getExpressionNum() const
	{
		if ((*json)[EXPRESSIONS].isNull()) return 0;
		return (*json)[EXPRESSIONS].getVector()->size();
	}

	LDString getExpressionFile(int n) const
	{
		return (*json)[EXPRESSIONS][n][FILE].toString();
	}

	LDString getExpressionName(int n) const
	{
		return (*json)[EXPRESSIONS][n][NAME].toString();
	}

	vector<LDString> getTextureFiles()  const
	{
		vector<LDString> rtn;
		for (int i = 0; i < (*json)[TEXTURES].size(); i++)
		{
			rtn.push_back((*json)[TEXTURES][i].toString());
		}
		return rtn;
	}

	vector<LDString> getExpressionFiles() const
	{
		vector<LDString> rtn;
		for (int i = 0; i < (*json)[EXPRESSIONS].size(); i++)
		{
			Value &m = (*json)[EXPRESSIONS][i];
			rtn.push_back(m[FILE].toString());
		}
		return rtn;
	}
	
	vector<LDString> getExpressionNames() const
	{
		vector<LDString> rtn;
		for (int i = 0; i < (*json)[EXPRESSIONS].size(); i++)
		{
			Value &m = (*json)[EXPRESSIONS][i];
			rtn.push_back(m[NAME].toString());
		}
		return rtn;
	}
};
