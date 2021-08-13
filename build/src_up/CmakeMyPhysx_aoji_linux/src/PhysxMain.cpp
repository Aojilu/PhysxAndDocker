#include "PhysxMain.h"
#include "ObjectDataHolder.h"
#include "Flags.h"
using namespace std;
using namespace physx;
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
#define PVD_USEDEBUG false
//#define USE_GPU false
//#define MAKE_OBJECTDATA false

namespace PhysxMain {
	
	PxDefaultAllocator      gAllocator;
	PxDefaultErrorCallback  gErrorCallback;
	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;
	PxPvd* gPvd = NULL;
	PxMaterial* gMaterial;

    IPhysxEnvirement* _physxEnvirement;
    IPhysxObjectCreator* _objectCreator;
    //ObjectDataHolder* _objectDataHolder = NULL;

    string tempReflectData;

#if USE_GPU
    PxCudaContextManager* gCudaContextManager = NULL;
#endif
	void InitPhysics(bool init)
	{
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

        // PVD(PhysX Visual Debugger) setting. To use PVD, we need to build the project as Debug mode.
        gPvd = PxCreatePvd(*gFoundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
        PxInitExtensions(*gPhysics, gPvd);

        // Scene setting




        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);          // Right-hand coordinate system, Y-UP.
        gDispatcher = PxDefaultCpuDispatcherCreate(1);         // The number of worker threads is one.
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
#if USE_GPU //gpuを利用する場合

        cout <<"use gpu"<< endl;
        PxCudaContextManagerDesc  cudaContextManagerDesc;
        gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
        sceneDesc.cudaContextManager = gCudaContextManager;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
#endif
        gScene = gPhysics->createScene(sceneDesc);
        float gravityRate = 1.0f;
        gScene->setGravity(PxVec3(0, -9.81 * gravityRate, 0));
        // PVD setting
        //PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
        /*if (pvdClient)
        {
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, PVD_USEDEBUG);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, PVD_USEDEBUG);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, PVD_USEDEBUG);
        }*/
        gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);    //static friction, dynamic friction, restitution
        if (init) {
            //床の追加
            PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
            groundPlane->setName("floor");
            gScene->addActor(*groundPlane);

            //初期化
            _objectCreator->SetScene(*gScene);
            _objectCreator->SetMaterial(*gMaterial);
            _objectCreator->SetPhyscis(*gPhysics);
            //_objectCreator->SetEnviement(*_physxEnvirement);
            _objectCreator->SetJudgePartial(*_physxEnvirement->GetJudgePartial());
            //_objectDataHolder = new ObjectDataHolder();
            //_objectCreator->SetDataHolderList(*_objectDataHolder);
            _physxEnvirement->SetScene(*gScene);
            _physxEnvirement->SetObjectCreator(*_objectCreator);

            //_physxEnvirement->SetObjectDataHolder(*_objectDataHolder);

            //環境作成
            _physxEnvirement->SetReflectData(tempReflectData);
            _physxEnvirement->InitPhysicsEnviourment();
        }
	}

	void KeyPress(unsigned char key, const PxTransform& camera)
	{
        _physxEnvirement->keyPress(key, camera);
	}


    //int stepcount1 = 30;
    //int stepcount2 = 30;
    void StepPhysics(float dt)
    {
        //_physxEnvirement->UpdateEnviroment(dt);

        gScene->simulate(dt);
        gScene->fetchResults(true);

    }

    void StepEnvirement(float dt) {
        _physxEnvirement->UpdateEnviroment(dt);
    }

    void SendBackMessage() {
        _physxEnvirement->CreateFrameMessage();
    }
    void SetPhysxEnvirement(IPhysxEnvirement& env)
    {
        _physxEnvirement = &env;
    }

    void SetObjectCreator(IPhysxObjectCreator& objc) {
        _objectCreator = &objc;
    }

    //本当は_physxEnvirementの関数を直接使いたかったがスレッドの関係でうまくいかなかったから作った
    //そのうち辞めたい
    void ReflectEnv(string input) {
        if (_physxEnvirement == NULL) {
            tempReflectData = input;
            return;
        }
        _physxEnvirement->SetReflectData(input);
        //_physxEnvirement->ReflectData2Envirement();
    }

    bool IsNeedUpdate() {
        return _physxEnvirement->isNeedUpdate();
    }
}