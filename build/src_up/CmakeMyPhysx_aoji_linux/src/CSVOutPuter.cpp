#pragma once
#include "CSVOutPuter.h"
#include <iomanip>
using namespace std;

string CreateTextName();
CSVOutPuter::CSVOutPuter(string title,int count,int datasize) {
	_setDataDistance = count;
	_maxDataSize=datasize;
	_nowCount = 0;
	_title = "_"+title;
}

bool CSVOutPuter::TryAddData(float data) {

	if (_outData.size() >= _maxDataSize)return false;
	_nowCount++;
	if (_nowCount == _setDataDistance) {
		_nowCount = 0;
		_outData.push_back(data);
		if (_outData.size() == _maxDataSize)cout << "outputter-" << _title << " filled" << endl;
	}
	return true;
}

string CSVOutPuter::GetOutPutText() {
	stringstream ss;
	ss << _title << ",";
	for (int i = 0; i < _outData.size(); i++) {
		ss << _outData[i] << ",";
	}
	return  ss.str().substr(0,ss.str().length()-1);
}


void CSVOutPuter::OutPutCSV() {
	//string textName = CreateTextName();
	//ofstream outputFile(_title+textName);
	//outputFile << GetOutPutText();
	//outputFile.close();
}

string CreateTextName() {
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	stringstream ss;
	ss  << "_outData_"
		<< tm->tm_year + 1900<<"_"
		<< tm->tm_mon + 1 << "_"
		<< tm->tm_mday << "_"
		<< tm->tm_hour << "_"
		<< tm->tm_min << "_"
		<< tm->tm_sec
		<< ".csv";
	return ss.str();
}