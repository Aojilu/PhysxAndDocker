#pragma once
#include<string>
#include<iostream>
#include<chrono>
#include <sstream>
using namespace std;
class TimeStamp
{
public:
	//タイムスタンプの取得
	string GetTimeStamp();

	//タイムスタンプからの経過時間
	long long GetGoneTime(string timeStamp);

	static long long GetFromEpockTime();
private:

};

