#pragma once
#include"IPhysxEnvirement.h"
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
	int height = 10;
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
	vector<string> returnData;

	bool _islocal;

	TimeStamp _timeStamp;
	CSVOutPutFactory _csvOutPutFactory;
	//TimeStamp* test_stamp=NULL;
	//string test_stamp_s;

	long long beforetime;
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

	   void SetReflectData(string data) override;
	  void CreateStack_ballpool(float xstart, float height, float zstart, int xCount, int zCount, float ballsize);
	  void ChengeBallSpeed(float dv);
};

