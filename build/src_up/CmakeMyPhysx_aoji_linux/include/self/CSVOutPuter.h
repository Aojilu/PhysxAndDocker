#pragma once
#include<vector>
#include<iostream>
#include<fstream>
#include<time.h>
#include<sstream>
using namespace std;
class CSVOutPuter
{
	vector<float> _outData;
	int _setDataDistance;
	int _nowCount;
	int _maxDataSize;//これよりデータサイズが大きい場合は間引きされる
	string _title;
public:
	CSVOutPuter(string title,int distance,int datasize);
	bool TryAddData(float data);
	string GetOutPutText();
	void OutPutCSV();
};

