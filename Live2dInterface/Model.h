#pragma once

#include "framework/L2DBaseModel.h"
#include "ALive2DModel.h"
#include "framework/L2DTargetPoint.h"
#include "ModelSetting.h"
#include <cstdint>
#include <map>

using namespace live2d;

class BKL2D_Model : public L2DBaseModel
{
	static vector<BKL2D_Model*> _models;
	static int32_t _channel;
	vector<uint32_t> textures;
	BKL2D_ModelSetting modelSetting;
	LDString modelHomeDir;
	ALive2DModel *live2DModel;
	int32_t channel;
	L2DTargetPoint dragMgr;
	void releaseTextures();
	bool hitTest(const LDString &name, const LDString &id, float x, float y);
	
public:
	static int32_t create(const char *file);
	static void destroy(int32_t id);
	static BKL2D_Model *modelFromID(int32_t id);
	static void destroyAll();

	BKL2D_Model();
	bool init(const char *modelFile);
	void uninit();

	bool loadModelData(const LDString &file, const vector<LDString> &textures);
	void loadExpressions(const vector<LDString> &names, const vector<LDString> &filenames);
	void loadPose(const LDString &file);
	void loadPhysics(const LDString &file);

	void update();
	void startMotion(const LDString &name, int no, int priority);
	void startRandomMotion(const LDString &name, int priority);
	void setExpression(const LDString &name);
	void setRandomExpression();

	void draw();
	void setMatrix(float *mat);

	void getCanvasSize(float *w, float *h);

	void dragEvent(float x, float y);
	void flickEvent(float x, float y);
	void tapEvent(float x, float y);
};
