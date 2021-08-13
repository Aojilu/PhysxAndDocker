#pragma once
#include "PxPhysicsAPI.h"
#include "ObjectDataHolder.h"
#include <vector>
#include "MyExtention.h"
#include<sstream>
//#include "IPhysxEnvirement.h"
#include "IJudgePartial.h"

//class IPhysxEnvirement;//循環参照対策　そのうち循環参照自体をやめる
using namespace physx;
class IPhysxObjectCreator
{
protected:
	IJudgePartial* _judegePartial = NULL;
	int _objCount = 0;
	PxScene* _gScene = NULL;
	PxPhysics* _gPhysics = NULL;
	PxMaterial* _gMaterial;


	int _containerCount = 0;
	void AddObjectData(PxRigidActor* obj,int index,bool isDynamic,bool isKinematic, const PxGeometry& geometry,string key="") {


		_objCount++;
		if(key=="") key = "Obj_" + to_string(_objCount);

		//対応するデータホルダーがない場合は作成
		if (index >= _containerCount) {
			int needCount = index - _containerCount + 2;
			for (int i = 0; i < needCount; i++) {
				AddContainer();
			}
		}

		//本当はどこのコンテナ所属かも変更しやすくしたい
		_dataHolderList[index]->_datalist.insert(std::make_pair(key, new ObjectData_toHolder(*obj, key, isDynamic, isKinematic, ObjectData_toHolder::CreateGeometyryInfo(geometry))));
		_dataHolderList[index]->_datalist_vec.push_back(new ObjectData_toHolder(*obj, key, isDynamic, isKinematic, ObjectData_toHolder::CreateGeometyryInfo(geometry)));
	}

	void AddContainer() {
		_containerCount++;
		ObjectDataHolder* objectDataHolder = new ObjectDataHolder();
		objectDataHolder->_containerNumber = _containerCount-1;//0番からカウント
		_dataHolderList.push_back(objectDataHolder);
	}

public:
	virtual PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry,string key="",bool isKienmatic=false, const PxVec3& velocity = PxVec3(0));
	virtual PxRigidStatic* createStatic(const PxTransform& t, const PxGeometry& geometry,string key="");
	
	void SetScene(PxScene& scene) {
		_gScene = &scene;
	}

	void SetPhyscis(PxPhysics& physics) {
		_gPhysics = &physics;
	}

	void SetMaterial(PxMaterial& material) {
		_gMaterial = &material;
	}
	/*void SetEnviement(IPhysxEnvirement& env) {
		_envirement = &env;
	}*/
	void SetJudgePartial(IJudgePartial& par) {
		_judegePartial = &par;
	}

	vector<ObjectDataHolder*> _dataHolderList;

	 ObjectData_toHolder* FindObject(string key, int index) {
		if (index >= _dataHolderList.size()) { cout << "IPhysxObjectCreator::FindObject exception:indexover"; return NULL; }
		//if (_dataHolderList[index]->_datalist.count(key) == 0)return NULL;
		//return _dataHolderList[index]->_datalist.at(key);
		return _dataHolderList[index]->_datalist[key];

	}

	 bool IsContain(string key, int index) {
		 if (index >= _dataHolderList.size()) { cout << "IPhysxObjectCreator::FindObject exception:indexover"; return false; }

		 return _dataHolderList[index]->_datalist.count(key) != 0;
	 }

	 int GetObjeCount() {
		return _objCount;
	}
};

