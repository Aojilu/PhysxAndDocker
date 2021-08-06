#pragma once
#include<map>
#include <string>
#include<functional>
#include <iostream>
#include <vector>
#include "MyExtention.h"
using namespace std;
namespace OrderCommandRegister
{
//private:
//	map<string, void(*)()> _commanList_void;
//	map<string,void(*)(double input)> _commanList_double;
//	//受け取ったコマンドから命令をだす関数
//	void CoalOrder(string command);
//	void CoalOrder(string command,string subCommand);
//public:
	void AddCommand(string key,function<void()>);
	void AddCommand(string key,function<void(double)>);
	void AddCommand(string key,function<void(string)>,char dummy); 
	//コマンドを受け取る関数
	void RecieveCommand(string command);

	//タイムスタンプを処理する関数
	void SetFuncTimeStamp(function<void(string)> command);
};

