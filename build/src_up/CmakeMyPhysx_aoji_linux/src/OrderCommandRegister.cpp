#include "OrderCommandRegister.h"
using namespace std;
namespace OrderCommandRegister {
	map<string, function<void()>> _commanList_void;
	map<string, function<void(double)>> _commanList_double;
	map<string, function<void(string)>> _commanList_string;

	function<void(string)> _func_TimeStamp;
	/*map<string, void(*)()> _commanList_void;
	map<string, void(*)(double input)> _commanList_double;
	map<string, void(*)(unsigned char input)> _commanList_char;*/

	void CoalOrder(string command) {
		auto itr = _commanList_void.find(command);
		if (itr != _commanList_void.end()) {
			_commanList_void[command]();
		}
		else {
			cout << "not found command:" << command << endl;
		}
	}
	void CoalOrder(string command, string subCommand) {
		auto itr_double = _commanList_double.find(command);
		auto itr_char = _commanList_string.find(command);
		if (itr_double != _commanList_double.end()) {
			try {
				double num = stod(subCommand);
				_commanList_double[command](num);
			}
			catch (const std::invalid_argument& e) {
				std::cout << e.what(); // "stoi: no conversion"
				std::cout << "変換に失敗しました" << subCommand << endl;
			}
		}
		else if (itr_char != _commanList_string.end()) {
			_commanList_string[command](subCommand);
		}
		else {
			cout << "not found command:" << command << endl;
		}
	}


	void CoalTimeStamp(string timeStamp) {
		if(_func_TimeStamp)_func_TimeStamp(timeStamp);
	}

	//コマンド受け取り　区切り文字は','　区切られていたら2つ目の文字列をデータとして利用
	void RecieveCommand(string command) {
		vector<string> vec = MyExtention::Split(command, ',');
		if (vec.size() <= 0)return;
		vector<string> head = MyExtention::Split(vec[0], ':');
		string command_h = head[0];
		if (vec.size() == 1) {
			CoalOrder(command_h);
		}
		else if (vec.size() >= 2) {
			string data = command.substr(vec[0].size() + 1);
			CoalOrder(command_h,data);
		}
		if(head.size()>=2)CoalTimeStamp(head[1]);
		/*if (vec.size() == 1) {
			CoalOrder(vec[0]);
		}
		else if (vec.size() >= 2) {
			int size = vec[0].size()+1;
			string subCommand = command.substr(size);
			CoalOrder(vec[0],subCommand );
		}*/
	}

	void AddCommand(string key, function<void()> command) {
		_commanList_void[key] = command;
	}
	void AddCommand(string key, function<void(string)> command,char dummy) {
		_commanList_string[key] = command;
	}
	void AddCommand(string key, function<void(double)> command) {
		_commanList_double[key] = command;
	}

	void SetFuncTimeStamp(function<void(string)> command) {
		_func_TimeStamp = command;
	}
	//void AddCommand(string key, void(*command)()) {
	//	_commanList_void[key] = command;
	//}
	//void AddCommand(string key, void(*command)(double input)) {
	//	_commanList_double[key] = command;
	//}
	//void AddCommand(string key, void(*command)(unsigned char input)) {
	//	_commanList_char[key] = command;
	//}

}