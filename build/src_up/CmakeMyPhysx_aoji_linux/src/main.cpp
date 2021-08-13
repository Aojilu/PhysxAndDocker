#include <iostream>
#include <string>
#include "OrderCommandRegister.h"
#include "MyPhysxController.h"
#include "WebSocketppFarcade.h"
#include<regex>
using namespace std;
#pragma region test

void TestFunc1() {
	cout << "TestFunc1" << endl;
}

void TestFunc2(double num) {
	cout << "TestFunc2, num=" << num << endl;
}

#pragma endregion

string ReplaceURI_ID(string uri,string id) {
	string result = regex_replace(uri, regex("<id>"), id);
	return result;
}

int main() {
#if 1
	string input;
	bool done = false;
	while (!done)
	{
		std::cout << "Enter Command: ";
		std::getline(std::cin, input);

		auto inputs = MyExtention::Split(input, ' ');
		if (inputs[0] == "quit") {
			done = true;
		}
		else if (inputs[0] == "local") {
			done = true;
			MyPhysxController::MainLoop_switch("local");
		}
		else if (inputs[0] == "host") {
			done = true;
			//WebSocketppFarcade::Connect(inputs[1]);
			std::thread th([&]() mutable {
				WebSocketppFarcade::Listen();
				});
			MyPhysxController::MainLoop_switch("glut");
			th.join();
		}
		else if (inputs[0]== "client") {
			done = true;
			if (inputs.size() >= 3 && inputs[2] == "-guioff") {
				MyPhysxController::MainLoop_switch("ws_offgui");
				cout << "guioff" << endl;
			}
			else 	MyPhysxController::MainLoop_switch("websocket");
			WebSocketppFarcade::ConnectAndRecieve(inputs[1]);
			//WebSocketppFarcade::Connect(inputs[1]);
		}
		else {
			std::cout << "> Unrecognized Command" << std::endl;
		}
	}
#elif 0
	string input;
	bool done = false;
	while (!done)
	{
		std::cout << "Enter Command: ";
		std::getline(std::cin, input);

		if (input == "quit") {
			done = true;
		}
		else if (input.substr(0,7) == "connect") {
			WebSocketppFarcade::Connect(input.substr(8));
		}
		else if (input.substr(0, 6) == "listen") {
			WebSocketppFarcade::Listen();
		}
		else if (input.substr(0, 4) == "send") {
			WebSocketppFarcade::Send(input.substr(5));
		}
		else if (input.substr(0, 5) == "close") {
			WebSocketppFarcade::Close();
		}
		else {
			std::cout << "> Unrecognized Command" << std::endl;
		}
	}
#elif 0
	string input;
	bool done = false;
	while (!done)
	{
		std::cout << "Enter Command: ";
		std::getline(std::cin, input);

		if (input == "quit") {
			done = true;
		}
		else if (input == "client") {
			std::cout << "> start client system" << std::endl;
			extern int Main_WebSocket_client();
			Main_WebSocket_client();
			done = true;
		}
		else if (input == "server") {
			std::cout << "> start server system" << std::endl;
			extern int	Main_WebSocket_server();
			Main_WebSocket_server();
			done = true;
		}
		else {
			std::cout << "> Unrecognized Command" << std::endl;
		}
	}
	return 0;
#endif
}

