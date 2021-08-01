#include "PhysXFarcade.h"
using namespace std;

//extern void initPhysics(bool init);
//extern void stepPhysics(float dt);
//extern void keyPress(unsigned char key, const PxTransform& camera);
namespace PhysXFarcade {
	Camera* _sCamera;
	FrameAction  _physcsFrameActor = FrameAction(1.0f / (50.0f));
	float _physicsTimeScale = 1.0f;//うまくいかない場合は1にする
	bool inited = false;
	bool _isActive_update = true;//trueならupdateの処理を行う
	bool _isActive_rendering = true;//trueならレンダリングの処理を行う

	CSVOutPutFactory _csvOutPutFactory;
	bool _isActive_csvRecord = false;

	IPhysxObjectCreator* _objCreator;
	//void Awake()
	//{
	//	//initPhysics(true);
	//}
	void Awake(IPhysxEnvirement& env, IPhysxObjectCreator& objc) {
		PhysxMain::SetPhysxEnvirement(env);
		PhysxMain::SetObjectCreator(objc);
		PhysxMain::InitPhysics(true);
		inited = true;
		_objCreator = &objc;
	}
	void Update()
	{
		if (!_isActive_update) {
			_physcsFrameActor.Refresh();
			return;
		}
		_physcsFrameActor.Update();

		//if (_isActive_csvRecord) {
		//	float idledt = _physcsFrameActor.GetProgressTime();//フレームレート
		//	//_csvOutPutFactory.Update("objCount_update", _objCreator->GetObjeCount());
		//}

		//cout << "idledt " << idledt << " sec" << "\n";
		//フレーム処理

		if (_physcsFrameActor.IsOverFrameRate()) {
			float dt = _physcsFrameActor.GetProgressTime();
			float stepDt = dt * _physicsTimeScale;
			_physcsFrameActor.Refresh();


			auto start = chrono::system_clock::now();//処理時間計測開始
			PhysxMain::StepPhysics(stepDt);
			if (!_isActive_csvRecord)return;
			auto dur = chrono::system_clock::now() - start;
			auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();//処理時間(msec)
			float sec = misec / 1000000.0f;//処理時間(sec)

			_csvOutPutFactory.Update("simulate", sec);
			_csvOutPutFactory.Update("objCount", _objCreator->GetObjeCount());
			_csvOutPutFactory.Update("update", dt);//フレームレート
			if (!_csvOutPutFactory.IsActive())EndCSVRecord();
		}
	}

	void RecieveInput(unsigned char input, int x, int y)
	{
		if (input == 27) {
			//cout << "output file"<<endl;
			//EndCSVRecord();
			exit(0);
		}
		if (toupper(input) == 'R') {
			cout << "startRecord"<<endl;
			StartCSVRecord();
		}

		_sCamera->handleKey(input, x, y);
		//keyPress(input, _sCamera->getTransform());
		PhysxMain::KeyPress(input, _sCamera->getTransform());
	}

	void HandleMouse(int button, int state, int x, int y) {
		_sCamera->handleMouse(button, state, x, y);
	}

	void HandleMotion(int x, int y) {
		_sCamera->handleMotion(x, y);
	}

	void Rendering()
	{
		//if (!_isRendering)return;
		if (!_isActive_rendering)return;
		if (_sCamera == NULL)return;
		RenderActor::startRender(_sCamera->getEye(), _sCamera->getDir());
		PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		if (nbActors)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			RenderActor::renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
		}
		RenderActor::finishRender();//ダブルバッファリングをしてる
	}


	void SetCamera(Camera* cam)
	{
		_sCamera = cam;
	}

	bool IsInited() {
		return inited;
	}


	void SetActive_update(bool flag) {
		//_isActive_update = flag;
	}
	void SetActive_rendering(bool flag) {
		//_isActive_rendering = flag;
	}

	void StartCSVRecord() {
		_isActive_csvRecord = true;
		//_csvOutPutFactory.AddDataSet("objCount_update", 5);
		_csvOutPutFactory.AddDataSet("objCount", 1,1);
		_csvOutPutFactory.AddDataSet("update", 1,500);
		_csvOutPutFactory.AddDataSet("simulate", 1,500);
	}

	void EndCSVRecord() {
		if(_isActive_csvRecord)_csvOutPutFactory.OutPutCSV();
		_isActive_csvRecord = false;
	}
}