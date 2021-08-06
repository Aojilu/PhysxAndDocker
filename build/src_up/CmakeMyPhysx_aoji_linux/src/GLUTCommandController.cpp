#include "GLUTCommandController.h"

using namespace std;
using namespace physx;
namespace GLUTCommandController {
	
	FrameAction _renderFrameActor = FrameAction(1.0f / 30.0f);

	Camera* sCamera;
	function<void()> awakeCallback;
	function<void(unsigned char, int x, int y)>inputCallback;
	function<void(int, int, int, int)> mouseCallback;
	function<void(int, int)> motionCallback;
	function<void()>idleCallback;
	function<void()>renderCallback;
	/*void(*inputCallback)(unsigned char);
	void(*idleCallback)();
	void(*renderCallback)();*/
	void IdleCallback()
	{
		_renderFrameActor.Update();

		//フレーム処理
		if (_renderFrameActor.IsOverFrameRate()) {
			//ログの出力がややこしいのでなんか関数作りたい
			//cout << "render_step " << _renderFrameActor.GetProgressTime() << " sec" << "\n";
			//auto start = chrono::system_clock::now();
			_renderFrameActor.Refresh();
			glutPostRedisplay();
			//auto end = chrono::system_clock::now();
			//auto dur = end - start;
			//auto msec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
			//cout<<"render simultare: " << msec << "nano sec\n";
		}
		if(IdleCallback)idleCallback();
	}
	void RenderCallback()
	{
		if(renderCallback)renderCallback();
	}
	void KeyboardCallback(unsigned char key, int x, int y)
	{
		unsigned char command = key;
		if(inputCallback)inputCallback(command,x,y);
		//sCamera->handleKey(key, x, y);
	}
	void MouseCallback(int button, int state, int x, int y)
	{
		if (mouseCallback)mouseCallback(button, state, x, y);
		/*sCamera->handleMouse(button, state, x, y);*/
	}
	void MotionCallback(int x, int y)
	{
		if (motionCallback)motionCallback(x, y);
		//sCamera->handleMotion(x, y);
	}
	void ExitCallback()
	{
	}



	void StartDisplay(const char* name,double posx,double posy) {
		cout << "test render"<<endl;
		sCamera = new Camera(PxVec3(0.0f, 50.0f, 50.0f), PxVec3(0.0f, -0.1f, -0.7f));

		RenderActor::setupDefaultWindow(name,posx,posy);
		RenderActor::setupDefaultRenderState();


		glutIdleFunc(IdleCallback);
		glutDisplayFunc(RenderCallback);
		glutKeyboardFunc(KeyboardCallback);
		glutMouseFunc(MouseCallback);
		glutMotionFunc(MotionCallback);
		//MotionCallback(0, 0);
		sCamera->handleMotion(0, 0);
		atexit(ExitCallback);

		//initPhysics(true);
		if (awakeCallback)awakeCallback();
		glutMainLoop();
	}

	void StartDisplay_guiOff() {
		sCamera = new Camera(PxVec3(0.0f, 50.0f, 50.0f), PxVec3(0.0f, -0.1f, -0.7f));
		sCamera->handleMotion(0, 0);
		if (awakeCallback)awakeCallback();
		while (true)
		{
			if (IdleCallback) {
				idleCallback();
				cout << "noguiupdate" << endl;
			}
		}
	}


	void AddCallbacl_awake(function<void()> func) { awakeCallback = func; }
	void AddCallbacl_input(function<void(unsigned char, int x, int y)> func) { inputCallback = func; }
	void AddCallbacl_mouse(function<void(int, int, int, int)> func) { mouseCallback = func; }
	void AddCallbacl_motion(function<void(int, int)> func) { motionCallback = func; }
	void AddCallbacl_idle(function<void()> func) { idleCallback = func; }
	void AddCallbacl_render(function<void()> func) { renderCallback = func; }


	Camera* GetCamera() { return sCamera; }
}
