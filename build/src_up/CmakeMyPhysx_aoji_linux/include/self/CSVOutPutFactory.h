#pragma once
#include "CSVOutPuter.h"
#include<vector>
#include<time.h>
#include<sstream>

using namespace std;
class CSVOutPutFactory
{
	//dictionary“I‚ÉŽg‚¤
	vector<string> _keyDatas;
	vector<CSVOutPuter*> _csvDatas;
	vector<bool> _isActives;
public:
	void AddDataSet(string title,int distance,int maxDataSize);
	void Update(string key, float data);
	void OutPutCSV();

	bool IsActive() {
		for (int i = 0; i < _isActives.size(); i++) {
			if (_isActives[i])return true;
		}
		return false;
	}
};

