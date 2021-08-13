#pragma once
#include <iostream>
#include "PxPhysicsAPI.h"
#include "IPhysxEnvirement.h"
#include "IPhysxObjectCreator.h"
#include "IJudgePartial.h"

//class IPhysxEnvirement;//循環参照対策　そのうち循環参照じたいをやめる
//class IPhysxObjectCreator;//循環参照対策　そのうち循環参照じたいをやめる
using namespace physx;
namespace PhysxMain
{
	void InitPhysics(bool init);
	void KeyPress(unsigned char key, const PxTransform& camera);
	void StepPhysics(float dt);
	void StepEnvirement(float dt);
	void SendBackMessage();

	void SetPhysxEnvirement(IPhysxEnvirement& env);
	void SetObjectCreator(IPhysxObjectCreator& objc);
	void ReflectEnv(string input);

	bool IsNeedUpdate();
};

