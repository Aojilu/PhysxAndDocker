#pragma once
#include<iostream>
#include<sstream>
#include"MyExtention.h"
#include "PxPhysicsAPI.h"
#include "IPhysxObjectCreator.h"
#include <thread>
#include <chrono>
#include<string>
#include <queue>
#include "IJudgePartial.h"
#include <iomanip>
//class IPhysxObjectCreator;//循環参照対策　そのうち循環参照じたいをやめる

using namespace physx;
using namespace std;
 class IPhysxEnvirement
{
private:
	//キャッシュ
	PxTransform m_tr_parent;
	PxVec3 m_pos_parent;
	PxQuat m_qr_parent;
protected:
	//queue< string >_reflectData;//受け取ったシーンのデータを保管する変数
	PxScene* _gScene = NULL;
	IPhysxObjectCreator* _physxObjectCreator=NULL;
	//ObjectDataHolder* _dataHolder = NULL;

	//string GetLog(int containercount);
	string CreateLog(int serchContainer,int sendContainer,bool isLight);

public:
	IPhysxEnvirement(){}
	virtual void InitPhysicsEnviourment();
	virtual void UpdateEnviroment(float dt);
	virtual void CreateFrameMessage();//フレームの返送要求を送信
	virtual void keyPress(unsigned char key, const PxTransform& camera);
	virtual bool isNeedUpdate() { return true; }

	void SetScene(PxScene& gScene) {
		_gScene = &gScene;
	}
	void SetObjectCreator(IPhysxObjectCreator& objCrtr) {
		_physxObjectCreator = &objCrtr;
	}

	 virtual void SetReflectData(string data) {
		 //_reflectData=data;

		 //while (_reflectData.size() > 4)_reflectData.pop();
	}

	//データを受け取って環境に反映
	virtual void ReflectData2Envirement();

	//どの部分空間に所属するかを判定する
	//virtual int JudgePartialSpaceNumber(PxVec3 pos);
	virtual IJudgePartial* GetJudgePartial() { return new IJudgePartial(); }


	//ログを前もって生成
	virtual void HashLogSet() {}

};

