#include "IPhysxEnvirement.h"

using namespace std;

string IPhysxEnvirement::CreateLog(int serch,int send, bool isLight) {
	stringstream ss;
	auto dataHolderList = _physxObjectCreator->_dataHolderList;
	if (serch >= dataHolderList.size())return "";
	auto dataHolder = dataHolderList[serch]->_datalist_vec;

	//string send_s = to_string(send);
	//string result = "";
	//result.reserve(1000000);

	for (const auto& it : dataHolder) {
		//PxTransform pose = it->_objectData->getGlobalPose();
		//PxVec3 tr = pose.p;
		//PxQuat qr = pose.q;		
		m_tr_parent = it->_objectData->getGlobalPose();
		m_pos_parent = m_tr_parent.p;
		m_qr_parent = m_tr_parent.q;

		//stringstream ss_temp;
		//ss_temp << "p:" << fixed << setprecision(3) << tr.x << "," << tr.y << "," << tr.z << ";"
		//	    << "q:" << qr.x << "," << qr.y << "," << qr.z << "," << qr.w << ";";
		//result.append("c:").append(send_s).append(";");
		//result.append("l:").append(it->_id).append(";");
		//result.append("d:").append(to_string( it->_isDynamic)).append(";");
		//result.append("k:").append(to_string(it->_isKinematic)).append(";");
		//result.append(ss_temp.str());
		//result.append("g:").append(it->_geometoryInfo).append(";\n");

		if (isLight) {
			ss  << "c:" << send << ";"
				<< "l:" << it->_id << ";"
				<< "p:" << fixed << setprecision(3) << m_pos_parent.x << "," << m_pos_parent.y << "," << m_pos_parent.z << ";"
				<< "q:" << m_qr_parent.x << "," << m_qr_parent.y << "," << m_qr_parent.z << "," << m_qr_parent.w << ";"
				<< "\n";
		}
		else {
			ss  << "c:" << send << ";"
				<< "l:" << it->_id << ";"
				<< "d:" << it->_isDynamic << ";"
				<< "k:" << it->_isKinematic << ";"
				<< "p:" << fixed << setprecision(3) << m_pos_parent.x << "," << m_pos_parent.y << "," << m_pos_parent.z << ";"
				<< "q:" << m_qr_parent.x << "," << m_qr_parent.y << "," << m_qr_parent.z << "," << m_qr_parent.w << ";"
				<< "g:" << it->_geometoryInfo << ";"
				<< "\n";
		}
	}
	return ss.str();
	//for (auto itr = dataHolder.begin(); itr != dataHolder.end(); ++itr) {
	//	auto obj = itr->second;
	//	auto pose = obj->_objectData->getGlobalPose();
	//	auto tr = pose.p;
	//	auto qr = pose.q;
	//	ss << "c:" << send << ";"
	//		<< "l:" << itr->first << ";"
	//		<< "d:" << obj->_isDynamic << ";"
	//		<< "k:" << obj->_isKinematic << ";"
	//		<< "p:" << fixed << setprecision(3) << tr.x << "," << tr.y << "," << tr.z << ";"
	//		<< "q:" << qr.x << "," << qr.y << "," << qr.z << "," << qr.w << ";"
	//		<< "g:" << obj->_geometoryInfo << ";" << "\n";
	//}

}

void IPhysxEnvirement::ReflectData2Envirement() {
	if (_gScene == NULL)return;
	//vector<string> objectDataList= MyExtention::Split(reflect, '\n');
	//for (int i = 0; i < objectDataList.size(); i++) {
	//	vector<string> labelList = MyExtention::Split(objectDataList[i], ';');

	//	//label�̒��o
	//	string label = MyExtention::Split(labelList[1], ':')[1];
	//	if (_physxObjectCreator->FindObject(label, 0) != NULL) {
	//		continue;
	//	}
	//	else {
	//		cout << label << endl;
	//	}
	//	//IsDynamic�̒��o
	//	bool isDynamic = MyExtention::Split(labelList[2], ':')[1] == "1";
	//	//IsKinematic�̒��o
	//	bool isKinematic = MyExtention::Split(labelList[3], ':')[1] == "1";
	//	//transform�̒��o
	//	string trText = MyExtention::Split(labelList[4], ':')[1];
	//	vector<string> trValues = MyExtention::Split(trText, ',');
	//	PxVec3* pos = new PxVec3(stof( trValues[0]), stof(trValues[1]), stof(trValues[2]));

	//	//quatanion�̒��o
	//	string quatText = MyExtention::Split(labelList[5], ':')[1];
	//	vector<string> quatValues = MyExtention::Split(quatText, ',');
	//	PxQuat* quat = new PxQuat(stof(quatValues[0]), stof(quatValues[1]), stof(quatValues[2]), stof(quatValues[3]));

	//	//geometory
	//	string geoinfo= MyExtention::Split(labelList[6], ':')[1];

	//	if (isDynamic)_physxObjectCreator->createDynamic(PxTransform(*pos, *quat), *ObjectData_toHolder::ConvertText2Geometry(geoinfo),label,isKinematic);
	//	else _physxObjectCreator->createStatic(PxTransform(*pos, *quat), *ObjectData_toHolder::ConvertText2Geometry(geoinfo),label);

	//	//std::this_thread::sleep_for(std::chrono::milliseconds(1));//�d�����Ă߂�����҂��Ȃ��Ƃ����Ȃ��@�I�u�W�F�N�g�������@�����P�����玡���邩���H
	//}
	////cout <<"reflectData="<< _reflectData << endl;
}

void IPhysxEnvirement::InitPhysicsEnviourment()
{
}

void IPhysxEnvirement::UpdateEnviroment(float dt)
{
}
void IPhysxEnvirement::CreateFrameMessage(){}
void IPhysxEnvirement::keyPress(unsigned char key, const PxTransform& camera)
{
}
//
//int IPhysxEnvirement::JudgePartialSpaceNumber(PxVec3 pos) {
//	return 0;
//}
