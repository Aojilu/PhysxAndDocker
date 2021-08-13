#pragma once
#include <Camera.h>
#include<iostream>
#include<vector>
#include "FrameAction.h"
#include "RenderActor.h"
#include "PhysxMain.h"
#include "IPhysxEnvirement.h"
#include "IPhysxObjectCreator.h"
#include "CSVOutPutFactory.h"
#include "WebSocketppFarcade.h"

namespace PhysXFarcade {
	//void Awake();
	void Awake(IPhysxEnvirement& env, IPhysxObjectCreator& objc);
	void Update();
	void RecieveInput(unsigned char input,int x,int y);
	void HandleMouse(int button, int state, int x, int y);
	void HandleMotion(int x, int y);
	void Rendering();

	void SetCamera(Camera* cam);

	bool IsInited();

	void SetActive_update(bool flag);
	void SetActive_rendering(bool flag);

	//‹L˜^—p
	void StartCSVRecord();
	void EndCSVRecord();

	CSVOutPutFactory* GetCSVFactory();
	bool isActiveRecord();

	void AddFrameCount();
	//void AddMissFrameCount();
	//void AddSendCount();
	//void AddRecieveCount();
}
