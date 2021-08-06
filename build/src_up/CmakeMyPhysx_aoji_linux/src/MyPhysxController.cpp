#include "MyPhysxController.h"
#include "MyExtention.h"
#include "GLUTCommandController.h"
#include<vector>
 #include <chrono>
 #include <thread>
#include "IPhysxEnvirement.h"
#include "NormalPhysxEnvirement.h"
#include "NormalPhysxEnvirement_child.h"
#include "IPhysxObjectCreator.h"
#include "StaticObjectCreator.h"
using namespace std;
namespace MyPhysxController {
	void MainLoop_switch(string input) {


		//コマンドと関数の対応の作成
		string command_init_glut = "glut_init";
		string command_init_physx = "physx_init";
		string command_update = "physx_update";
		string command_input = "physx_input";
		string command_motion = "physx_motion";
		string command_mouse = "physx_mouse";
		string command_render = "physx_rendering";
		string command_env_objectData = "envirement_objectdata";

	
		OrderCommandRegister::AddCommand(command_update, PhysXFarcade::Update);
		OrderCommandRegister::AddCommand(command_render, PhysXFarcade::Rendering);
		OrderCommandRegister::AddCommand(command_input,
			[=](string input) {
				try {
					vector<string>vec = MyExtention::Split(input, ',');
					int x = stoi(vec[1]);
					int y = stoi(vec[2]);
					PhysXFarcade::RecieveInput(vec[0][0], x, y);
				}
				catch (exception) {
					cout << "input exception:command_input:input=" << input << endl;
				}
			}, 'd');
		OrderCommandRegister::AddCommand(command_motion,
			[=](string input) {
				try {
					vector<string>vec = MyExtention::Split(input, ',');
					int x = stoi(vec[0]);
					int y = stoi(vec[1]);
					PhysXFarcade::HandleMotion(x, y);
				}
				catch (exception) {
					cout << "input exception:command_motion:input=" << input << endl;
				}
			}, 'd');
		OrderCommandRegister::AddCommand(command_mouse,
			[=](string input) {
				try {
					vector<string>vec = MyExtention::Split(input, ',');
					int button = stoi(vec[0]);
					int state = stoi(vec[1]);
					int x = stoi(vec[2]);
					int y = stoi(vec[3]);
					PhysXFarcade::HandleMouse(button, state, x, y);
				}
				catch (exception) {
					cout << "input exception:command_mouse:input=" << input << endl;
				}
			}, 'd');

		//スレッドなどの関係でEnveirmentにアクセスできなかったので、IPhysxEnveirmentの関数を直接使わず、PhysxMainに関数を作って行っている
		//そのうち治したい
		OrderCommandRegister::AddCommand(command_env_objectData, [&](string input) {
			PhysxMain::ReflectEnv(input);
			}, 'd');
		//コマンド呼び出しの登録 どこから呼び出すかで処理が分岐===============================================================
		if (input.substr(0, 5) == "local") {
			OrderCommandRegister::AddCommand(command_init_physx, [&]() {
				PhysXFarcade::Awake(*new NormalPhysxEnvirement(true), *new IPhysxObjectCreator());
				PhysXFarcade::SetCamera(GLUTCommandController::GetCamera());
				});


			//GLUTとコマンドの対応の作成
			GLUTCommandController::AddCallbacl_awake([=]() {
				OrderCommandRegister::RecieveCommand(command_init_physx);
				});
			GLUTCommandController::AddCallbacl_idle([=]() {
				OrderCommandRegister::RecieveCommand(command_update);
				});
			GLUTCommandController::AddCallbacl_render([=]() {
				OrderCommandRegister::RecieveCommand(command_render);
				});
			GLUTCommandController::AddCallbacl_input([=](unsigned char c, int x, int y) {
				stringstream ss;
				ss << command_input << "," << c << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				});
			GLUTCommandController::AddCallbacl_motion([=](int x, int y) {
				stringstream ss;
				ss << command_motion << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				});
			GLUTCommandController::AddCallbacl_mouse([=](int button, int state, int x, int y) {
				stringstream ss;
				ss << command_mouse << "," << button << "," << state << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				});
			GLUTCommandController::StartDisplay("local", 1000, 100);
		}
		else if (input.substr(0, 4) == "glut") {//glutから命令

			OrderCommandRegister::AddCommand(command_init_physx, [&]() {
				PhysXFarcade::Awake(*new NormalPhysxEnvirement(false), *new StaticObjectCreator());
				PhysXFarcade::SetCamera(GLUTCommandController::GetCamera());
				});


			//GLUTとコマンドの対応の作成
			GLUTCommandController::AddCallbacl_awake([=]() {
				OrderCommandRegister::RecieveCommand(command_init_physx);
				//WebSocketppFarcade::Send(command_init_glut);
				});
			GLUTCommandController::AddCallbacl_idle( [=]() {
				OrderCommandRegister::RecieveCommand(command_update);
				//WebSocketppFarcade::Send(command_update);
			});
			GLUTCommandController::AddCallbacl_render([=]() {
				OrderCommandRegister::RecieveCommand(command_render);
				//WebSocketppFarcade::Send(command_render);
			});
			GLUTCommandController::AddCallbacl_input([=](unsigned char c, int x, int y) {
				stringstream ss;
				ss << command_input << "," << c << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			});
			GLUTCommandController::AddCallbacl_motion([=](int x, int y) {
				stringstream ss;
				ss << command_motion << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			});
			GLUTCommandController::AddCallbacl_mouse([=](int button, int state, int x, int y) {
				stringstream ss;
				ss << command_mouse << "," << button << "," << state << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			});
			//返答データの処理
			WebSocketppFarcade::SetReplayMessage(PhysxMain::ReflectEnv);
			GLUTCommandController::StartDisplay("host",1000,100);

			/*std::thread th([] {GLUTCommandController::StartDisplay("client", 1000, 100); });
			th.detach();*/
			/*std::thread th(
				[] {GLUTCommandController::StartDisplay("client", 1000, 100); }
			);
			th.join();*/
		}
		else if (input.substr(0, 9) == "websocket") {//websocketから命令

			OrderCommandRegister::AddCommand(command_init_physx, [&]() {//環境の初期化
				PhysXFarcade::Awake(*new NormalPhysxEnvirement_child(), *new IPhysxObjectCreator());
				PhysXFarcade::SetCamera(GLUTCommandController::GetCamera());
				});
			//GLUTとコマンドの対応の作成
			GLUTCommandController::AddCallbacl_awake([=]() {

				OrderCommandRegister::RecieveCommand(command_init_physx);
			});
			GLUTCommandController::AddCallbacl_render([=]() {
				OrderCommandRegister::RecieveCommand(command_render);
			});

			GLUTCommandController::AddCallbacl_idle([=]() {
				OrderCommandRegister::RecieveCommand(command_update);
				});GLUTCommandController::AddCallbacl_input([=](unsigned char c, int x, int y) {
				stringstream ss;
				ss << command_input << "," << c << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			});
			GLUTCommandController::AddCallbacl_motion([=](int x, int y) {
				stringstream ss;
				ss << command_motion << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			});
			GLUTCommandController::AddCallbacl_mouse([=](int button, int state, int x, int y) {
				stringstream ss;
				ss << command_mouse << "," << button << "," << state << "," << x << "," << y;
				string command = ss.str();
				OrderCommandRegister::RecieveCommand(command);
				//WebSocketppFarcade::Send(command);
			}); 


			//webSocketで送られてきたメッセージをコマンドとして呼ぶ
			WebSocketppFarcade::SetRecieveMessage(OrderCommandRegister::RecieveCommand);

			//初期化命令の受け取り
			OrderCommandRegister::AddCommand(command_init_glut, [] {
				//GLUTCommandController::StartDisplay("server");

				//StartDisplayすると処理が止まってserverListenできなくなるので別スレッドで呼んでいる
				//雑に実装したのでそのうち問題になりそう
				//遠隔側の描画はデバッグ用なのであんまり問題ないかな？

				if (PhysXFarcade::IsInited())return;
				std::thread th([] {GLUTCommandController::StartDisplay("client",100,100); });
				th.detach();
				});
		}
		else if (input.substr(0, 9) == "ws_offgui") {

			OrderCommandRegister::AddCommand(command_init_physx, [&]() {//環境の初期化
				PhysXFarcade::Awake(*new NormalPhysxEnvirement_child(), *new IPhysxObjectCreator());
				PhysXFarcade::SetCamera(GLUTCommandController::GetCamera());
				});
			//GLUTとコマンドの対応の作成
			GLUTCommandController::AddCallbacl_awake([=]() {

				OrderCommandRegister::RecieveCommand(command_init_physx);
				});
			GLUTCommandController::AddCallbacl_render([=]() {
				OrderCommandRegister::RecieveCommand(command_render);
				});

			GLUTCommandController::AddCallbacl_idle([=]() {
				OrderCommandRegister::RecieveCommand(command_update);
				}); GLUTCommandController::AddCallbacl_input([=](unsigned char c, int x, int y) {
					stringstream ss;
					ss << command_input << "," << c << "," << x << "," << y;
					string command = ss.str();
					OrderCommandRegister::RecieveCommand(command);
					});
				GLUTCommandController::AddCallbacl_motion([=](int x, int y) {
					stringstream ss;
					ss << command_motion << "," << x << "," << y;
					string command = ss.str();
					OrderCommandRegister::RecieveCommand(command);
					});
				GLUTCommandController::AddCallbacl_mouse([=](int button, int state, int x, int y) {
					stringstream ss;
					ss << command_mouse << "," << button << "," << state << "," << x << "," << y;
					string command = ss.str();
					OrderCommandRegister::RecieveCommand(command);
					});


				//webSocketで送られてきたメッセージをコマンドとして呼ぶ
				WebSocketppFarcade::SetRecieveMessage(OrderCommandRegister::RecieveCommand);

				//初期化命令の受け取り
				OrderCommandRegister::AddCommand(command_init_glut, [] {
					//GLUTCommandController::StartDisplay("server");

					//StartDisplayすると処理が止まってserverListenできなくなるので別スレッドで呼んでいる
					//雑に実装したのでそのうち問題になりそう
					//遠隔側の描画はデバッグ用なのであんまり問題ないかな？

					if (PhysXFarcade::IsInited())return;
					std::thread th([] {GLUTCommandController::StartDisplay_guiOff(); });
					th.detach();
					});
		}
	}
	//
	//void MainLoop_switch(string input) {

	//	GLUTCommandController glutCommandController;

	//	//コマンドと関数の対応の作成
	//	string command_init_glut = "glut_init";
	//	string command_init_physx = "physx_init";
	//	string command_update = "physx_update";
	//	string command_input = "physx_input";
	//	string command_motion = "physx_motion";
	//	string command_mouse = "physx_mouse";
	//	string command_render = "physx_rendering";

	//	OrderCommandRegister::AddCommand(command_init_physx, [&]() {
	//		PhysXFarcade::Awake();
	//		PhysXFarcade::SetCamera(glutCommandController._sCamera);
	//		});
	//	OrderCommandRegister::AddCommand(command_update, PhysXFarcade::Update);
	//	OrderCommandRegister::AddCommand(command_render, PhysXFarcade::Rendering);
	//	OrderCommandRegister::AddCommand(command_input,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int x = stoi(vec[1]);
	//				int y = stoi(vec[2]);
	//				PhysXFarcade::RecieveInput(vec[0][0], x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_input:input=" << input << endl;
	//			}
	//		}, 'd');
	//	OrderCommandRegister::AddCommand(command_motion,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int x = stoi(vec[0]);
	//				int y = stoi(vec[1]);
	//				PhysXFarcade::HandleMotion(x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_motion:input=" << input << endl;
	//			}
	//		}, 'd');
	//	OrderCommandRegister::AddCommand(command_mouse,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int button = stoi(vec[0]);
	//				int state = stoi(vec[1]);
	//				int x = stoi(vec[2]);
	//				int y = stoi(vec[3]);
	//				PhysXFarcade::HandleMouse(button, state, x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_mouse:input=" << input << endl;
	//			}
	//		}, 'd');

	//	//コマンド呼び出しの登録 どこから呼び出すかで処理が分岐
	//	if (input.substr(0, 4) == "glut") {
	//		//GLUTとコマンドの対応の作成
	//		glutCommandController._awakeCallback = [=]() {
	//			OrderCommandRegister::RecieveCommand(command_init_physx);
	//			WebSocketppFarcade::Send(command_init_glut);
	//		};
	//		glutCommandController._idleCallback = [=]() {
	//			OrderCommandRegister::RecieveCommand(command_update);
	//			//WebSocketppFarcade::Send(command_update);
	//		};
	//		glutCommandController._renderCallback = [=]() {
	//			OrderCommandRegister::RecieveCommand(command_render);
	//			//WebSocketppFarcade::Send(command_render);
	//		};
	//		glutCommandController._inputCallback = [=](unsigned char c, int x, int y) {
	//			stringstream ss;
	//			ss << command_input << "," << c << "," << x << "," << y;
	//			string command = ss.str();
	//			OrderCommandRegister::RecieveCommand(command);
	//			//WebSocketppFarcade::Send(ss.str());
	//		};
	//		glutCommandController._motionCallback = [=](int x, int y) {
	//			stringstream ss;
	//			ss << command_motion << "," << x << "," << y;
	//			string command = ss.str();
	//			OrderCommandRegister::RecieveCommand(command);
	//			//WebSocketppFarcade::Send(ss.str());
	//		};
	//		glutCommandController._mouseCallback = [=](int button, int state, int x, int y) {
	//			stringstream ss;
	//			ss << command_mouse << "," << button << "," << state << "," << x << "," << y;
	//			string command = ss.str();
	//			OrderCommandRegister::RecieveCommand(command);
	//			//WebSocketppFarcade::Send(ss.str());
	//		};

	//		glutCommandController.StartDisplay("client");
	//	}
	//	else if (input.substr(0, 9) == "websocket") {

	//		//GLUTとコマンドの対応の作成
	//		glutCommandController._awakeCallback = [=]() {
	//			OrderCommandRegister::RecieveCommand(command_init_physx);
	//		};
	//		glutCommandController._renderCallback = [=]() {
	//			OrderCommandRegister::RecieveCommand(command_render);
	//		};
	//		//webSocketで送られてきたメッセージをコマンドとして呼ぶ
	//		WebSocketppFarcade::SetRecieveMessage(OrderCommandRegister::RecieveCommand);

	//		//初期化命令の受け取り
	//		OrderCommandRegister::AddCommand(command_init_glut, [&] {
	//			glutCommandController.StartDisplay("server");
	//			});
	//	}
	//}

	//void MainLoop_client() {

	//	GLUTCommandController glutCommandController;

	//	//コマンドと関数の対応の作成
	//	string command_init_glut = "glut_init";
	//	string command_init_physx = "physx_init";
	//	string command_update = "physx_update";
	//	string command_input = "physx_input";
	//	string command_motion = "physx_motion";
	//	string command_mouse = "physx_mouse";
	//	string command_render = "physx_rendering";
	//	OrderCommandRegister::AddCommand(command_init_physx, [&]() {
	//		PhysXFarcade::Awake();
	//		PhysXFarcade::SetCamera(glutCommandController._sCamera);
	//		});
	//	OrderCommandRegister::AddCommand(command_update, PhysXFarcade::Update);
	//	OrderCommandRegister::AddCommand(command_render, PhysXFarcade::Rendering);


	//	OrderCommandRegister::AddCommand(command_input,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int x = stoi(vec[1]);
	//				int y = stoi(vec[2]);
	//				PhysXFarcade::RecieveInput(vec[0][0], x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_input:input="<<input << endl;
	//			}
	//		}, 'd');

	//	OrderCommandRegister::AddCommand(command_motion,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int x = stoi(vec[0]);
	//				int y = stoi(vec[1]);
	//				PhysXFarcade::HandleMotion(x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_motion:input=" << input << endl;
	//			}
	//		}, 'd');
	//	OrderCommandRegister::AddCommand(command_mouse,
	//		[=](string input) {
	//			try {
	//				vector<string>vec = MyExtention::Split(input, ',');
	//				int button = stoi(vec[0]);
	//				int state = stoi(vec[1]);
	//				int x = stoi(vec[2]);
	//				int y = stoi(vec[3]);
	//				PhysXFarcade::HandleMouse(button,state,x, y);
	//			}
	//			catch (exception) {
	//				cout << "input exception:command_mouse:input=" << input << endl;
	//			}
	//		}, 'd');

	//	//GLUTとコマンドの対応の作成
	//	glutCommandController._awakeCallback = [=]() { 
	//		OrderCommandRegister::RecieveCommand(command_init_physx);
	//		//WebSocketppFarcade::Send(command_init_glut);
	//	};
	//	glutCommandController._idleCallback = [=]() { 
	//		OrderCommandRegister::RecieveCommand(command_update); 
	//		//WebSocketppFarcade::Send(command_update);
	//	};
	//	glutCommandController._renderCallback = [=]() {
	//		OrderCommandRegister::RecieveCommand(command_render);
	//		//WebSocketppFarcade::Send(command_render);
	//	};
	//	glutCommandController._inputCallback = [=](unsigned char c, int x, int y) {
	//		stringstream ss;
	//		ss << command_input << "," << c << "," << x << "," << y;
	//		string command = ss.str();
	//		OrderCommandRegister::RecieveCommand(command);
	//		//WebSocketppFarcade::Send(ss.str());
	//	};
	//	glutCommandController._motionCallback = [=](int x, int y) {
	//		stringstream ss;
	//		ss << command_motion << "," << x << "," << y;
	//		string command = ss.str();
	//		OrderCommandRegister::RecieveCommand(command);
	//		//WebSocketppFarcade::Send(ss.str());
	//	};
	//	glutCommandController._mouseCallback = [=](int button,int state,int x, int y) {
	//		stringstream ss;
	//		ss << command_mouse <<","<<button<<","<<state<< "," << x << "," << y;
	//		string command = ss.str();
	//		OrderCommandRegister::RecieveCommand(command);
	//		//WebSocketppFarcade::Send(ss.str());
	//	};

	//	glutCommandController.StartDisplay("client");

	//}

	//void MainLoop_server() {

	//	//GLUTCommandController glutCommandController;
	//	////コマンドと関数の対応の作成
	//	//string command_init_glut = "glut_init";
	//	//string command_init_physx = "physx_init";
	//	//string command_update = "physx_update";
	//	//string command_input = "physx_input";
	//	//string command_render = "physx_rendering";

	//	//OrderCommandRegister::AddCommand(command_init_glut, [&]() { glutCommandController.StartDisplay("server"); });
	//	//OrderCommandRegister::AddCommand(command_init_physx, [&]() {
	//	//	PhysXFarcade::Awake();
	//	//	PhysXFarcade::SetCamera(glutCommandController._sCamera);
	//	//	});
	//	//OrderCommandRegister::AddCommand(command_update, PhysXFarcade::Update);
	//	//OrderCommandRegister::AddCommand(command_input, PhysXFarcade::RecieveInput, 'd');
	//	//OrderCommandRegister::AddCommand(command_render, PhysXFarcade::Rendering);
	//	////GLUTとコマンドの対応の作成
	//	//glutCommandController._awakeCallback = [=]() {
	//	//	OrderCommandRegister::RecieveCommand(command_init_physx);
	//	//};
	//	//glutCommandController._renderCallback = [=]() {
	//	//	OrderCommandRegister::RecieveCommand(command_render);
	//	//};

	//	//WebSocketppFarcade::SetRecieveMessage(OrderCommandRegister::RecieveCommand);
	//}
}