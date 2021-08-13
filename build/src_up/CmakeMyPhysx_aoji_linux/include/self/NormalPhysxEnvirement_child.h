#pragma once
#include "IPhysxEnvirement.h"
#include "WebSocketppFarcade.h"
#include "TimeStamp.h"
#include "OrderCommandRegister.h"
#include "CSVOutPutFactory.h"
#include "PhysXFarcade.h"
class NormalPhysxEnvirement_child :
    public IPhysxEnvirement
{
	int length_x = 10;
	int height = 10;
	int length_z = 10;
	float _ballsize = 1.0f;
	PxRigidDynamic* moveWall = NULL;
	bool wallmoveFlag = false;
	bool isUp = true;
	float maxpos = -40;
	float minpos = -55;
	float wallSpeed = 10;
	float _ballSpeed = 100;

	int _containerCount = -1;//自分が何番コンテナなのかの情報

	string _hashLog = "";

	string reflectData;



	CSVOutPutFactory* _csvOutPutFactory;

	bool m_startSimulate = false;
	TimeStamp m_timeStamp;
	int m_easyHostCount = 0;
	//キャッシュ
	PxTransform m_transform;
	PxVec3 m_pos;
	PxQuat m_quat;
	ObjectData_toHolder* m_targetObject;
	vector<string> m_objectDataList;
	vector<string> m_labelList;
public:
	void InitPhysicsEnviourment() override;
	void UpdateEnviroment(float dt) override;
	void CreateFrameMessage() override;
	void keyPress(unsigned char key, const PxTransform& camera) override;
	void ReflectData2Envirement() override;
	//int JudgePartialSpaceNumber(PxVec3 pos)override;
	void HashLogSet() override;
	void SetReflectData(string data) override;
	bool isNeedUpdate()override;

	void CreateStack_ballpool(int lx, float height, int lz, float ballsize);
	void ChengeBallSpeed(float dv);

	void MoveWall(float dt);
};

