#include "PhysXFarcade.h"
#include "Flags.h"
using namespace std;

//extern void initPhysics(bool init);
//extern void stepPhysics(float dt);
//extern void keyPress(unsigned char key, const PxTransform& camera);

namespace PhysXFarcade {
	Camera* _sCamera;
	FrameAction  _physcsFrameActor = FrameAction(1.0f / (50.0f));
	FrameAction  _physcsBackMessage = FrameAction(1.0f / (50.0f));
	FrameAction  _physcsTimeActor = FrameAction(1.0f);
	float _physicsTimeScale = 1.0f;//うまくいかない場合は1にする
	bool inited = false;
	bool _isActive_update = true;//trueならupdateの処理を行う
	bool _isActive_rendering = true;//trueならレンダリングの処理を行う

	CSVOutPutFactory _csvOutPutFactory;
	bool _isActive_csvRecord = false;
	bool _isEnd_csvRecord = false;

	IPhysxObjectCreator* _objCreator;

	int _reserch_frameCount = 0;
	//int _reserch_missFrameCount=0;
	//int _reserch_sendCount = 0;
	//int _reserch_recieveCount = 0;

	std::thread* th_backmess = NULL;
	std::thread* th_frame = NULL;
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

		_isActive_update = PhysxMain::IsNeedUpdate();
	}
	void Update()
	{
		if (!_isActive_update) {
			cout << "noupdate"<<endl;
			return;
		}
		_physcsFrameActor.Update();
		_physcsBackMessage.Update();
#if RESERCH_FPS
		_physcsTimeActor.Update();
#endif

		//フレーム処理
		float pt = _physcsFrameActor.GetProgressTime();
		if (pt < 0.03f)cout << pt << endl;
		else cout << pt << ":over!======" << endl;


		if (_physcsBackMessage.IsOverFrameRate()) {
			_physcsBackMessage.Refresh();
			//th_backmess=new thread([] {
			//	PhysxMain::SendBackMessage();
			//	});
			PhysxMain::SendBackMessage();//返送要求を精製
		}
#if FPS_NOLIMIT //fpsの制限をつけない
		float dt = _physcsFrameActor.GetProgressTime();
		float stepDt = dt * _physicsTimeScale;
		_physcsFrameActor.Refresh();
		PhysxMain::StepEnvirement(stepDt);

		if (_isActive_csvRecord)_csvOutPutFactory.Update("framedistance", dt);
		if (!_csvOutPutFactory.IsActive())EndCSVRecord();
#else 
		if (_physcsFrameActor.IsOverFrameRate()) {
			float dt = _physcsFrameActor.GetProgressTime();
			float stepDt = dt * _physicsTimeScale;
			_physcsFrameActor.Refresh();
			PhysxMain::StepEnvirement(stepDt);

			if (_isActive_csvRecord)_csvOutPutFactory.Update("framedistance", dt);
			if (!_csvOutPutFactory.IsActive())EndCSVRecord();
		}
#endif

#if RESERCH_FPS
		if (_physcsTimeActor.IsOverFrameRate()) {

			_physcsTimeActor.Refresh();
			if (_isActive_csvRecord) {
				_csvOutPutFactory.Update("fps", _reserch_frameCount);
				//_csvOutPutFactory.Update("fps_miss", _reserch_missFrameCount);
				//_csvOutPutFactory.Update("frame_send", _reserch_sendCount);
				//_csvOutPutFactory.Update("frame_recieve", _reserch_recieveCount);
				_reserch_frameCount = 0;
				//_reserch_missFrameCount = 0;
				//_reserch_sendCount = 0;
				//_reserch_recieveCount = 0;
			}
		}
#endif
	}

	void RecieveInput(unsigned char input, int x, int y)
	{
		if (input == 27) {
			//cout << "output file"<<endl;
			exit(0);
		}
		if (toupper(input) == 'R') {
#if RESERCH
			if (!_isActive_csvRecord) {
				cout << "startRecord" << endl;
				StartCSVRecord();
			}
			else if(_isActive_csvRecord&&!_isEnd_csvRecord){
				EndCSVRecord();
			}
#endif
		}
		else if (toupper(input) == 'E') {
			SetActive_rendering(!_isActive_rendering);
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
		_isActive_rendering = flag;
	}

	void StartCSVRecord() {
		_isActive_csvRecord = true;
		//_csvOutPutFactory.AddDataSet("objCount_update", 5);
		_csvOutPutFactory.AddDataSet("objCount", 1,1);
		_csvOutPutFactory.AddDataSet("framedistance", 1, 500);
		_csvOutPutFactory.AddDataSet("fps", 1, 100);
		//_csvOutPutFactory.AddDataSet("fps_miss", 1, 100);
		//_csvOutPutFactory.AddDataSet("frame_send", 1, 100);
		//_csvOutPutFactory.AddDataSet("frame_recieve", 1, 100);

		_csvOutPutFactory.Update("objCount", _objCreator->GetObjeCount());
	}

	void EndCSVRecord() {
		if(_isActive_csvRecord)_csvOutPutFactory.OutPutCSV();
		_isActive_csvRecord = false;
		_isEnd_csvRecord = true;
	}

	CSVOutPutFactory* GetCSVFactory() {
		return &_csvOutPutFactory;
	}

	bool isActiveRecord() {
		return _isActive_csvRecord;
	}

	void AddFrameCount() {
#if RESERCH_FPS
		_reserch_frameCount++;
#endif
	}

//	void AddMissFrameCount() {
//#if RESERCH_FPS
//		_reserch_missFrameCount++;
//#endif
//	}
//	void AddSendCount() {
//#if RESERCH_FPS
//		_reserch_sendCount++;
//#endif
//	}
//	void AddRecieveCount() {
//#if RESERCH_FPS
//		_reserch_recieveCount++;
//#endif
//	}
}