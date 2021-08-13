#pragma once
#include"IPhysxEnvirement.h"
#include "PhysXFarcade.h"
#include "PxPhysicsAPI.h"
#include<iostream>
#include "IPhysxObjectCreator.h"
#include "WebSocketppFarcade.h"
#include <sstream>
#include "SimpleJudgePartial.h"
#include "TimeStamp.h"
#include "CSVOutPutFactory.h"
using namespace physx;
class NormalPhysxEnvirement :
    public IPhysxEnvirement
{
	int length_x = 10;
	int height = 20;
	int length_z = 10;
	float _ballsize = 1.0f;
	PxRigidDynamic* moveWall1 = NULL;
	PxRigidDynamic* moveWall2 = NULL;
	bool wallmoveFlag = false;
	bool isUp = true;
	float maxpos = -40;
	float minpos = -55;
	float wallSpeed = 10;
	float _ballSpeed = 100;

	float _nowCharge = 0.0f;
	int targetClinet = 0;
	bool _connectWait = false;
	bool _connected = false;

	int clientCount = 2;//クライアントの数　まだ固定値
	vector<bool> isWaiting;
	//vector<string> returnData;
	vector<queue<string>*> returnData;//受け取ったらためていく

	bool _islocal;

	TimeStamp _timeStamp;
	CSVOutPutFactory* _csvOutPutFactory=NULL;

	//キャッシュ
	PxTransform m_transform;
	PxVec3 m_pos;
	PxQuat m_quat;
	ObjectData_toHolder* m_targetObject=NULL;
	vector<string> m_objectDataList;
	vector<string> m_labelList;
public:
	NormalPhysxEnvirement(bool local):IPhysxEnvirement() {
		_islocal = local;
	}
	 void InitPhysicsEnviourment() override;
	  void UpdateEnviroment(float dt) override;
	  void CreateFrameMessage() override;
	  void keyPress(unsigned char key, const PxTransform& camera) override;
	  void ReflectData2Envirement() override;
	  //int JudgePartialSpaceNumber(PxVec3 pos)override;
	  IJudgePartial* GetJudgePartial()override {
		  return new SimpleJudgePartial();
	  }
	  bool isNeedUpdate()override;

	   void SetReflectData(string data) override;
	  void CreateStack_ballpool(float xstart, float height, float zstart, int xCount, int zCount, float ballsize);
	  void ChengeBallSpeed(float dv);
	  void MoveWall(float dt);
};

