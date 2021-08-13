#pragma once
#include<unordered_map>
#include<vector>
#include<PxPhysicsAPI.h>
#include<string>
#include"MyExtention.h"
#include<sstream>
using namespace std;
using namespace physx;

class ObjectData_toHolder
{
public:
	PxRigidActor* _objectData;
	bool _isDynamic;
	bool _isKinematic;
	string _geometoryInfo = "";
	string _id="";
	ObjectData_toHolder(PxRigidActor& objData,string id, bool isDynamic, bool isKinematic, string geometoryInfo) {
		_id = id;
		_objectData = &objData;
		_isDynamic = isDynamic;
		_isKinematic = isKinematic;
		_geometoryInfo = geometoryInfo;
	}

#pragma region static

	static PxGeometry* ConvertText2Geometry(string input) {
		auto inputs = MyExtention::Split(input, ',');
		if (inputs[0] == "box") {
			stod("5");
			return new PxBoxGeometry(stod(inputs[1]), stod(inputs[2]), stod(inputs[3]));
		}
		else if (inputs[0] == "sphere") {
			return new PxSphereGeometry(stod(inputs[1]));
		}
		return NULL;
	}


	static string CreateGeometyryInfo(const PxGeometry& geometry) {

		stringstream ss;

		switch (geometry.getType())
		{
		case physx::PxGeometryType::Enum::eSPHERE: {
			ss << "sphere,";
			auto sphere = (const PxSphereGeometry*)&geometry;
			ss << sphere->radius;
			break; }
		case physx::PxGeometryType::Enum::eBOX: {
			ss << "box,";
			auto box = (const PxBoxGeometry*)&geometry;
			ss << box->halfExtents.x << "," << box->halfExtents.y << "," << box->halfExtents.z;
			break; }
		default:
			break;
		}
		return ss.str();
	}
#pragma endregion

};


class ObjectDataHolder
{
public:
	//map<string, PxRigidActor*> _datalist;
	unordered_map<string, ObjectData_toHolder*> _datalist;
	vector<ObjectData_toHolder*>_datalist_vec;
	int _containerNumber = -1;
};
