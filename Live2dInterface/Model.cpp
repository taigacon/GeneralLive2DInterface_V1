#include "Model.h"
#include "GlobalConfig.h"
#include "util/UtSystem.h"
#include "Define.h"
#include "graphics/DrawProfileCocos2D.h"
#include <string>
#include <algorithm>

vector<BKL2D_Model*> BKL2D_Model::_models;

int32_t BKL2D_Model::_channel = -10;

BKL2D_Model::BKL2D_Model()
{
	expressionMgr = new L2DMotionManager;
	channel = _channel--;
}

void BKL2D_Model::uninit()
{
	if (!initialized)
	{
		return;
	}
	delete live2DModel;
	releaseTextures();
}

void BKL2D_Model::releaseTextures()
{
	if (live2DModel)
	{
		live2DModel->deleteTextures();
	}
	for (auto i : textures)
	{
		freeTexture(i);
	}
	textures.clear();
}

bool BKL2D_Model::init(const char *modelFile)
{
	updating = true;
	initialized = false;
	{
		string file = modelFile;
		size_t pos = -1;
		if ((pos = file.find_last_of("/\\")) != -1)
		{
			modelHomeDir = file.substr(0, pos + 1).c_str();
		}
	}

	if (!modelSetting.init(modelFile))
	{
		sendError((string("bkl2d: Can not open modelFile:") + modelFile).c_str());
		return false;
	}

	if (!loadModelData(modelSetting.getModelFile(), modelSetting.getTextureFiles()))
	{
		return false;
	}

	// 表情
	loadExpressions(modelSetting.getExpressionNames(), modelSetting.getExpressionFiles());

	// 物理演算
	loadPhysics(modelSetting.getPhysicsFile());

	// パーツ切り替え
	loadPose(modelSetting.getPoseFile());

	{
		map<LDString, float> layout = modelSetting.getLayout();
		if (layout.find("width") != layout.end())
			modelMatrix->setWidth(layout["width"]);
		if (layout.find("height") != layout.end())
			modelMatrix->setHeight(layout["height"]);
		if (layout.find("x") != layout.end())
			modelMatrix->setX(layout["x"]);
		if (layout.find("y") != layout.end())
			modelMatrix->setY(layout["y"]);
		if (layout.find("center_x") != layout.end())
			modelMatrix->centerX(layout["center_x"]);
		if (layout.find("center_y") != layout.end())
			modelMatrix->centerY(layout["center_y"]);
		if (layout.find("top") != layout.end())
			modelMatrix->top(layout["top"]);
		if (layout.find("bottom") != layout.end())
			modelMatrix->bottom(layout["bottom"]);
		if (layout.find("left") != layout.end())
			modelMatrix->left(layout["left"]);
		if (layout.find("right") != layout.end())
			modelMatrix->right(layout["right"]);
	}

	for (int i = 0; i < modelSetting.getInitParamNum(); i++) {
		LDString id = modelSetting.getInitParamID(i);
		float value = modelSetting.getInitParamValue(i);
		live2DModel->setParamFloat(id.c_str(), value);
	}

	for (int i = 0; i < modelSetting.getInitPartsVisibleNum(); i++) {
		LDString id = modelSetting.getInitPartsVisibleID(i);
		float value = modelSetting.getInitPartsVisibleValue(i);
		live2DModel->setPartsOpacity(id.c_str(), value);
	}

	// 自動目パチ
	eyeBlink = new L2DEyeBlink();

	updating = false;// 更新状態の完了
	initialized = true;// 初期化完了

	return true;
}

#if defined(_MSC_VER) || defined(__MINGW__)
#include "Live2DModelWinGL.h"
#endif

bool BKL2D_Model::loadModelData(const LDString &file, const vector<LDString> &textures)
{
	uint32_t s;
	uint8_t *d = readDataFromFile((modelHomeDir + file).c_str(), &s);
	if (!d)
	{
		sendError((LDString("bkl2d: Can not open modelDataFile:") + file).c_str());
		return false;
	}
#if defined(_MSC_VER) || defined(__MINGW__)
	live2DModel = Live2DModelWinGL::loadModel(d, s);
#endif
	freeData(d);

	for (int i = 0; i < textures.size(); i++)
	{
		uint32_t id = loadTextureFromFile((modelHomeDir + textures[i]).c_str());
		if (id == 0)
		{
			sendError((LDString("bkl2d: Can not load texture:") + textures[i]).c_str());
			releaseTextures();
			return false;
		}
		this->textures.push_back(id);
#if defined(_MSC_VER) || defined(__MINGW__)
		((Live2DModelWinGL *)live2DModel)->setTexture(i, id);
#endif
	}

	modelMatrix = new L2DModelMatrix(live2DModel->getCanvasWidth(), live2DModel->getCanvasHeight());
	modelMatrix->setWidth(2);
	modelMatrix->setCenterPosition(0, 0);// 中心に配置
	return true;
}

void BKL2D_Model::loadExpressions(const vector<LDString> &names, const vector<LDString> &filenames)
{
	for (int i = 0; i < filenames.size(); i++)
	{
		uint32_t s;
		uint8_t *d = readDataFromFile((modelHomeDir + filenames[i]).c_str(), &s);
		if (!d)
		{
			sendError((LDString("bkl2d: Can not load expression file:") + filenames[i]).c_str());
		}
		expressions[names[i]] = L2DExpressionMotion::loadJson(d, s);
		freeData(d);
	}
}

void BKL2D_Model::loadPose(const LDString &file)
{
	uint32_t s;
	uint8_t *d = readDataFromFile((modelHomeDir + file).c_str(), &s);
	if (!d)
	{
		sendError((LDString("bkl2d: Can not load pose file:") + file).c_str());
	}
	pose = L2DPose::load(d, s);
	freeData(d);
}

void BKL2D_Model::loadPhysics(const LDString &file)
{
	uint32_t s;
	uint8_t *d = readDataFromFile((modelHomeDir + file).c_str(), &s);
	if (!d)
	{
		sendError((LDString("bkl2d: Can not load physics file:") + file).c_str());
	}
	physics = L2DPhysics::load(d, s);
	freeData(d);
}

void BKL2D_Model::startRandomMotion(const LDString &name, int priority) {
	int max = modelSetting.getMotionNum(name);
	int no = (int)(rand() % max);
	startMotion(name, no, priority);
}

/**
* モーションの開始。 再生できる状態かチェックして、できなければ何もしない。 再生出来る場合は自動でファイルを読み込んで再生。
* 音声付きならそれも再生。 フェードイン、フェードアウトの情報があればここで設定。なければ初期値。
*/
void BKL2D_Model::startMotion(const LDString &name, int no, int priority) {
	LDString motionName = modelSetting.getMotionFile(name, no);

	if (!motionName.size() || motionName=="") {
		return;
	}

	// 新しいモーションのpriorityと、再生中のモーション、予約済みモーションのpriorityと比較して
	// 予約可能であれば（優先度が高ければ）再生を予約します。
	//
	// 予約した新モーションは、このフレームで即時再生されるか、もしくは音声のロード等が必要な場合は
	// 以降のフレームで再生開始されます。
	if (!mainMotionMgr->reserveMotion(priority)) {
		return;
	}

	AMotion *motion = NULL;
	{
		LDString motionPath = modelHomeDir + motionName;
		uint32_t s;
		uint8_t *d = readDataFromFile(motionPath.c_str(),&s);
		if (d)
		{
			motion = Live2DMotion::loadMotion(d, s);
			freeData(d);
		}
		if (!motion)
		{
			sendError((LDString("bkl2d: Can not load motion:") + motionName).c_str());
			mainMotionMgr->setReservePriority(0);
			return;
		}
	}

	stopSound(channel);

	// フェードイン、フェードアウトの設定
	motion->setFadeIn(modelSetting.getMotionFadeIn(name, no));
	motion->setFadeOut(modelSetting.getMotionFadeOut(name, no));

	// 音声が無いモーションは即時再生を開始します。
	if (modelSetting.getMotionSound(name, no).size()) {
		mainMotionMgr->startMotionPrio(motion, true, priority);
	}
	// 音声があるモーションは音声のロードを待って次のフレーム以降に再生を開始します。
	else {
		LDString soundName = modelSetting.getMotionSound(name, no);
		LDString soundPath = modelHomeDir + soundName;
		bool lip_sync = modelSetting.getMotionLipSync(name, no);
		if (!playSound(channel, soundPath.c_str(), lip_sync))
		{
			sendError((LDString("bkl2d: Can not load sound:") + soundName + ". Motion will be played silently.").c_str());
		}
		if (lip_sync)
		{
			lipSync = true;
		}
	}
}


/**
* 表情を設定する
*
* @param motion
*/
void BKL2D_Model::setExpression(const LDString &name) {
	for (auto it = expressions.begin(); it != expressions.end(); it++)
	{
		if ((*it).first == name)
		{
			AMotion *motion = (*it).second;
			expressionMgr->startMotion(motion, false);
		}
	}
}

/**
* 表情をランダムに切り替える
*/
void BKL2D_Model::setRandomExpression() {
	int no = (int)(rand() % expressions.size());
	auto it = expressions.begin();
	while (no--)
		it++;
	setExpression((*it).first);
}

void BKL2D_Model::update()
{
	long timeMSec = UtSystem::getUserTimeMSec() - startTimeMSec;
	double timeSec = timeMSec / 1000.0;
	double t = timeSec * 2 * M_PI;// 2πt

	// 待機モーション判定
	if (mainMotionMgr->isFinished()) {
		// モーションの再生がない場合、待機モーションの中からランダムで再生する
		startRandomMotion(BKL2D_Define::MOTION_GROUP_IDLE,
			BKL2D_Define::PRIORITY_IDLE);
	}
	// -----------------------------------------------------------------
	live2DModel->loadParam();// 前回セーブされた状態をロード

	bool update = mainMotionMgr->updateParam(live2DModel);// モーションを更新
	if (!update) {
		// メインモーションの更新がないとき
		eyeBlink->setParam(live2DModel);// 目パチ
	}

	live2DModel->saveParam();// 状態を保存

	expressionMgr->updateParam(live2DModel);// 表情でパラメータ更新（相対変化）

	setDrag(dragMgr.getX(), dragMgr.getY());
	// ドラッグによる変化
	// ドラッグによる顔の向きの調整
	live2DModel->addToParamFloat(PARAM_ANGLE_X, dragX * 30, 1);// -30から30の値を加える
	live2DModel->addToParamFloat(PARAM_ANGLE_Y, dragY * 30, 1);
	live2DModel->addToParamFloat(PARAM_ANGLE_Z, (dragX * dragY) * -30, 1);

	// ドラッグによる体の向きの調整
	live2DModel->addToParamFloat(PARAM_BODY_X, dragX * 10, 1);// -10から10の値を加える

	// ドラッグによる目の向きの調整
	live2DModel->addToParamFloat(PARAM_EYE_BALL_X, dragX, 1);// -1から1の値を加える
	live2DModel->addToParamFloat(PARAM_EYE_BALL_Y, dragY, 1);

	// 呼吸など
	live2DModel->addToParamFloat(PARAM_ANGLE_X,
		(float)(15 * std::sin(t / 6.5345)), 0.5f);
	live2DModel->addToParamFloat(PARAM_ANGLE_Y,
		(float)(8 * std::sin(t / 3.5345)), 0.5f);
	live2DModel->addToParamFloat(PARAM_ANGLE_Z,
		(float)(10 * std::sin(t / 5.5345)), 0.5f);
	live2DModel->addToParamFloat(PARAM_BODY_X,
		(float)(4 * std::sin(t / 15.5345)), 0.5f);
	live2DModel->setParamFloat(PARAM_BREATH,
		(float)(0.5f + 0.5f * std::sin(t / 3.2345)), 1);

	// 加速度による変化
	live2DModel->addToParamFloat(PARAM_ANGLE_Z, 90 * accelX, 0.5f);

	if (physics != NULL)
		physics->updateParam(live2DModel);// 物理演算でパラメータ更新

	// リップシンクの設定
	if (lipSync) {
		float lipSyncValue;
		if (getRealTimeVolumeOfSound(channel, &lipSyncValue))
			live2DModel->setParamFloat(PARAM_MOUTH_OPEN_Y, lipSyncValue, 0.8f);
		else
			lipSync = false;
	}

	// ポーズの設定
	if (pose != NULL)
		pose->updateParam(live2DModel);

	live2DModel->update();
}

void BKL2D_Model::draw()
{
	live2d::DrawProfileCocos2D::preDraw();

	this->update();
	live2DModel->draw();

	live2d::DrawProfileCocos2D::postDraw();
}

void BKL2D_Model::setMatrix(float *mat)
{
// 	float m[16] = { 0 };
// 	L2DMatrix44::mul(mat, modelMatrix->getArray(), m);
// #if defined(_MSC_VER) || defined(__MINGW__)
// 	((Live2DModelWinGL*)live2DModel)->setMatrix(m);
// #endif
// //	((Live2DModelWinGL*)live2DModel)->setMatrix(modelMatrix->getArray());
	float w = 800;
	float h = 600;
	float scx = 2.0 / live2DModel->getCanvasWidth();
	float scy = -2.0 / live2DModel->getCanvasWidth() * (w / h);
	float x = -1;
	float y = 1;
	float matrix[] = {
		scx, 0, 0, 0,
		0, scy, 0, 0,
		0, 0, 1, 0,
		x, y, 0, 1
	};
	((Live2DModelWinGL*)live2DModel)->setMatrix(matrix);
}

bool BKL2D_Model::hitTest(const LDString &name, const LDString &id, float x, float y)
{
	int drawIndex = live2DModel->getDrawDataIndex(id.c_str());
	if (drawIndex < 0)
		return false;// 存在しない場合はfalse
	int pointCount;
	float* points = live2DModel->getTransformedPoints(drawIndex, &pointCount);

	float left = live2DModel->getCanvasWidth();
	float right = 0;
	float top = live2DModel->getCanvasHeight();
	float bottom = 0;

	for (int j = 0; j < pointCount; j = j + 2) {
		float x = points[j];
		float y = points[j + 1];
		if (x < left)
			left = x; // 最小のx
		if (x > right)
			right = x; // 最大のx
		if (y < top)
			top = y; // 最小のy
		if (y > bottom)
			bottom = y;// 最大のy
	}

	float tx = modelMatrix->invertTransformX(x);
	float ty = modelMatrix->invertTransformY(y);

	return (left <= tx && tx <= right && top <= ty && ty <= bottom);
}



void BKL2D_Model::flickEvent(float x, float y)
{
	int hitAreas = modelSetting.getHitAreasNum();
	for (int h = 0; h < hitAreas; h++)
	{
		if (hitTest(modelSetting.getHitAreaName(h), modelSetting.getHitAreaID(h), x, y))
		{
			LDString motionGroup = LDString("flick_") + modelSetting.getHitAreaName(h);
			int exist = modelSetting.getMotionNum(motionGroup);
			if (exist != 0)
			{
				startRandomMotion(motionGroup, BKL2D_Define::PRIORITY_NORMAL);
				break;
			}
		}
	}
}

void BKL2D_Model::tapEvent(float x, float y)
{
	int hitAreas = modelSetting.getHitAreasNum();
	for (int h = 0; h < hitAreas; h++)
	{
		if (hitTest(modelSetting.getHitAreaName(h), modelSetting.getHitAreaID(h), x, y))
		{
			LDString motionGroup = LDString("tap_") + modelSetting.getHitAreaName(h);
			int exist = modelSetting.getMotionNum(motionGroup);
			if (exist != 0)
			{
				startRandomMotion(motionGroup, BKL2D_Define::PRIORITY_NORMAL);
				break;
			}
		}
	}
}

void BKL2D_Model::dragEvent(float x, float y)
{
	dragMgr.set(x, y);
}

int32_t BKL2D_Model::create(const char *file)
{
	BKL2D_Model *model = new BKL2D_Model;
	if (model && model->init(file))
	{
		_models.push_back(model);
		return _models.size() - 1;
	}
	return -1;
}

void BKL2D_Model::destroy(int32_t id)
{
	if (id >=0 && id < (int32_t)_models.size() && _models[id])
	{
		delete _models[id];
		_models[id] = NULL;
	}
}

BKL2D_Model *BKL2D_Model::modelFromID(int32_t id)
{
	if (id < (int32_t)_models.size())
	{
		return _models[id];
	}
	return NULL;
}

void BKL2D_Model::getCanvasSize(float *w, float *h)
{
	*w = live2DModel->getCanvasWidth();
	*h = live2DModel->getCanvasHeight();
}

void BKL2D_Model::destroyAll()
{
	for (auto i : _models)
	{
		if (i)
		{
			delete i;
		}
	}
	_models.clear();
}