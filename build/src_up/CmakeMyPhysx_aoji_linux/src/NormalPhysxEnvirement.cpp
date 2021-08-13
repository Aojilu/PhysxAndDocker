#include "NormalPhysxEnvirement.h"
#include "Flags.h"
void NormalPhysxEnvirement::CreateStack_ballpool(float xstart, float height,float zstart,int xCount,int zCount, float ballsize) {
    for (int x = 0; x < xCount; x++) {
        for (int z = 0; z < zCount; z++) {
            _physxObjectCreator->createDynamic(PxTransform((PxVec3(xstart,height,zstart)+PxVec3(x, height, z) * (ballsize + 1))), PxSphereGeometry(ballsize));
        }
    }
}

void NormalPhysxEnvirement::ChengeBallSpeed(float dv) {
    _ballSpeed += dv;
    if (_ballSpeed < 0)_ballSpeed = 0;
}

void NormalPhysxEnvirement::InitPhysicsEnviourment()
{

    if(!_islocal)WebSocketppFarcade::Send("host","", 0);
    _csvOutPutFactory = PhysXFarcade::GetCSVFactory();
    _csvOutPutFactory->AddDataSet("reflectOD", 1, 500);//ODを反映するのにかかる時間
    _csvOutPutFactory->AddDataSet("backDataTime", 1, 500);//データが帰ってくるのにかかる時間
    _csvOutPutFactory->AddDataSet("simulate", 1, 500);//データが帰ってくるのにかかる時間


    //moveWall = _physxObjectCreator->createDynamic(PxTransform(PxVec3(8, 0, -5)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-42, 20, -55)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-42, 20, -25)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-25, 20, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-59, 20, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);


    _physxObjectCreator->createDynamic(PxTransform(PxVec3(50, 20, -55)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(50, 20, -25)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(67, 20, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(33, 20, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);

    for (int i = 0; i < 5; i++) {
        CreateStack_ballpool(-50, height + i * _ballsize,-50,length_x, length_z, _ballsize);
    }
    for (int i = 0; i < 5; i++) {
        CreateStack_ballpool(42, height + i * _ballsize,-50, length_x, length_z, _ballsize);
    }
}

void NormalPhysxEnvirement::UpdateEnviroment(float dt)
{
    if (_islocal) {
#if RESERCH_SIMULATETIME//処理時間計測開始
        auto start_simulate = chrono::system_clock::now();
#endif
        PhysxMain::StepPhysics(dt);
        MoveWall(dt); 
#if RESERCH_SIMULATETIME
        {
            if (PhysXFarcade::isActiveRecord()) {
                auto dur = chrono::system_clock::now() - start_simulate;
                auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();//処理時間(msec)
                float sec = misec / 1000000.0f;//処理時間(sec)
                _csvOutPutFactory->Update("simulate", sec);
            }
        }
#endif
#if RESERCH_FPS
        if (PhysXFarcade::isActiveRecord()) {
            PhysXFarcade::AddFrameCount();
        }
#endif
    }
    else {
        if (_connected && isWaiting.size() == 2) {
            //isWaiting[0] = true;
            //isWaiting[1] = true;
            //ReflectData2Envirement();

        }
    }
    
}
void NormalPhysxEnvirement::CreateFrameMessage() {
//    if (_connected) {
//        if (isWaiting.size() == 2) {
//            
//            string time = _timeStamp.GetTimeStamp();
//            //一定数たまってしまったらいったん止める
//            //if (returnData[0]->size() < 5)WebSocketppFarcade::Send("back", "", time, 0);
//            //if (returnData[1]->size() < 5)WebSocketppFarcade::Send("back", "", time, 1);
//            //WebSocketppFarcade::Send("back", "", time, 0);
//            //WebSocketppFarcade::Send("back", "", time, 1);
////#if RESERCH_FPS
////            if (PhysXFarcade::isActiveRecord()) {
////                PhysXFarcade::AddSendCount();
////            }
////#endif
//            //cout << "create message" << endl;
//        }
//    }
}

void NormalPhysxEnvirement::MoveWall(float dt) {
    if (!wallmoveFlag)return;
    if (moveWall1 == NULL) {
        auto temp = _physxObjectCreator->FindObject("moveWall", 0);
        if (temp != NULL)moveWall1 = (PxRigidDynamic*)temp->_objectData;
        auto temp2 = _physxObjectCreator->FindObject("moveWall", 1);
        if (temp2 != NULL)moveWall2 = (PxRigidDynamic*)temp2->_objectData;
    }
    if (moveWall1 == NULL || moveWall2 == NULL)return;

    PxVec3 pos = moveWall1->getGlobalPose().p;
    if (isUp && pos.z > maxpos)isUp = false;
    else if (!isUp && pos.z < minpos)isUp = true;
    float sign = (isUp) ? -1.0f : 1.0f;

    PxTransform next_position1(moveWall1->getGlobalPose().p - PxVec3(0.0f, 0.0f, dt * wallSpeed * sign));//位置のみを指定しているが、角度も指定できるよ
    moveWall1->setKinematicTarget(next_position1);

    PxTransform next_position2(moveWall2->getGlobalPose().p - PxVec3(0.0f, 0.0f, dt * wallSpeed * sign));//位置のみを指定しているが、角度も指定できるよ
    moveWall2->setKinematicTarget(next_position2);
}

void NormalPhysxEnvirement::keyPress(unsigned char key, const PxTransform& camera)
{
    switch (toupper(key))
    {
    case 'R'://記録開始を送信
    {
        //stringstream ss;
        //ss << "physx_input" << "," << "R" << "," << "0" << "," << "0";
        //string command = ss.str();
        //for (int i = 0; i < clientCount; i++)WebSocketppFarcade::Send(ss.str(),, i); 
        for (int i = 0; i < clientCount; i++)WebSocketppFarcade::Send("physx_input","R,0,0", i);
    }
        break;
    //case ' ': {
    //    _physxObjectCreator->createDynamic(camera, PxBoxGeometry(2.0f, 2.0f, 2.0f),"",false, camera.rotate(PxVec3(-0.3f, 0, -1)) * _ballSpeed);
    //    break;
    //    }
    //case 'Q':ChengeBallSpeed(10.0f); break;
    //case 'E':ChengeBallSpeed(-5.0f); break;
    //case 'B':
    //    CreateStack_ballpool(-50, height, -50, length_x, length_z, _ballsize);
    //    CreateStack_ballpool(42, height, -50, length_x, length_z, _ballsize);
    //    break;
    case 'V':
        if (_islocal)return;
        {//分散処理の開始
            for (int i = 0; i < clientCount; i++) {
                stringstream ss;
                //string log = GetLog(i);
                string log = CreateLog(i, i,false);
                if (log == "")continue;
                ss << "envirement_objectdata," << log;
                //WebSocketppFarcade::Send(ss.str(), i);
                WebSocketppFarcade::Send("envirement_objectdata",log, i);

                std::this_thread::sleep_for(std::chrono::milliseconds(100));//一気に送るとエラー吐くので待ってる　くそコード
                WebSocketppFarcade::Send("glut_init","", i);
            }
            break;
        }
    case 'C':
        _connectWait = true;
        WebSocketppFarcade::Send("back","", 0);
        WebSocketppFarcade::Send("back","", 1);
        break;
    case 'X':
    {
        if (!_islocal) {//壁を動かす
            /*stringstream ss;
            ss << "physx_input" << "," << "X" << "," << "0" << "," << "0";
            string command = ss.str();
            for (int i = 0; i < clientCount; i++)WebSocketppFarcade::Send(ss.str(), i);*/
            string data = "X,0,0";
            for (int i = 0; i < clientCount; i++)WebSocketppFarcade::Send("physx_input",data, i);
        }
        else {
            wallmoveFlag = !wallmoveFlag;
        }

        break;
    }
    case 'Z'://ボールの追加
        CreateStack_ballpool(-50, height, -50, length_x, length_z, _ballsize);
        CreateStack_ballpool(42, height, -50, length_x, length_z, _ballsize);
        if (_islocal)return;
        for (int i = 0; i < clientCount; i++) {
            stringstream ss;
            //string log = GetLog(i);
            string log = CreateLog(i, i,false);
            if (log == "")continue;
            ss << "envirement_objectdata," << log;
            //WebSocketppFarcade::Send(ss.str(), i);
            WebSocketppFarcade::Send("envirement_objectdata",log, i);
        }
        break;
    }
}
void NormalPhysxEnvirement::ReflectData2Envirement() {
    if (_islocal)return;
    if (_gScene == NULL)return;
//    if (returnData[0]->size()==0 || returnData[1]->size() == 0) {//両方更新できるときのみ更新
//#if RESERCH_FPS
//        if (PhysXFarcade::isActiveRecord()) {
//            PhysXFarcade::AddMissFrameCount();
//        }
//#endif
//        return;
//    }
//    else {
//#if RESERCH_FPS
//        if (PhysXFarcade::isActiveRecord()) {
//            PhysXFarcade::AddFrameCount();
//        }
//#endif
//    }

    if (returnData.size() <= 0)return;
#if RESERCH_REFLECTODTIME
    auto start_refod = std::chrono::system_clock::now();
#endif
    //if (returnData[0]->size() == 0 || returnData[1]->size() == 0)return;
    for (int k = 0; k < returnData.size(); k++) {
        if (returnData[k]->size() == 0) {
            return;
        }
        else {
#if RESERCH_FPS
            if (PhysXFarcade::isActiveRecord()&&k==0) {
                PhysXFarcade::AddFrameCount();
            }
#endif
        }

        string target = returnData[k]->front();
        returnData[k]->pop();
        m_objectDataList = MyExtention::Split(target, '\n');

        if (m_objectDataList.size() <= 2)continue;//反映するものがない判定


        //オブジェクトデータの反映処理
        for (int i = 0; i < m_objectDataList.size(); i++) {

            //vector<string> labelList = MyExtention::Split(objectDataList[i], ';');
            m_labelList = MyExtention::Split(m_objectDataList[i], ';');
            if (m_labelList.size() != 4)continue;

            //containerの抽出
            string container = m_labelList[0].substr(2);
            int containerCount = stoi(container);
            //labelの抽出
            string label = m_labelList[1].substr(2);

            //transformの抽出
            vector<string> trValues = MyExtention::Split(m_labelList[2].substr(2), ',');
            m_pos.x = stof(trValues[0]);
            m_pos.y = stof(trValues[1]);
            m_pos.z = stof(trValues[2]);

            //quatanionの抽出
            vector<string> quatValues = MyExtention::Split(m_labelList[3].substr(2), ',');
            m_quat.x = stof(quatValues[0]);
            m_quat.y = stof(quatValues[1]);
            m_quat.z = stof(quatValues[2]);
            m_quat.w = stof(quatValues[3]);

            //対応するオブジェクトの位置を反映
            //auto targetObject = _physxObjectCreator->FindObject(label, containerCount);
            //if (targetObject == NULL)continue;
            m_targetObject = _physxObjectCreator->FindObject(label, containerCount);
            if (m_targetObject == NULL)continue;

            //auto transform = targetObject->_objectData->getGlobalPose();
            //transform.p = *pos;
            //transform.q = *quat;
            //targetObject->_objectData->setGlobalPose(transform);


            m_transform.p = m_pos;
            m_transform.q = m_quat;
            m_targetObject->_objectData->setGlobalPose(m_transform);
        }



    }
#if RESERCH_REFLECTODTIME
    auto end_refod = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end_refod - start_refod).count();
    if (PhysXFarcade::isActiveRecord()) {
        _csvOutPutFactory->Update("reflectOD", dur / 1000000.0f);
    }
#endif
}
//int NormalPhysxEnvirement::JudgePartialSpaceNumber(PxVec3 pos) {
//    int parialLine = 0;
//    if (pos.x < parialLine)return 0;
//    else return 1; 
//}

void NormalPhysxEnvirement::SetReflectData(string data) {
    if (_islocal)return;
    if (returnData.size() == 0) {
        //returnData.push_back("");
        //returnData.push_back("");
        returnData.push_back(new queue<string>());
        returnData.push_back(new queue<string>());
        isWaiting.push_back(false);
        isWaiting.push_back(false);
    }

    vector<string> objectDataList = MyExtention::Split(data, '\n');
    if (objectDataList.size() < 2)return;
    vector<string> labelList = MyExtention::Split(objectDataList[1], ';');
    if (labelList.size() != 4)return;
    //containerの抽出
    string container = MyExtention::Split(labelList[0], ':')[1];
    int containerCount = stoi(container);
    //returnData[containerCount] = data;
    returnData[containerCount]->push( data.substr(objectDataList[0].size()));
    //cout << "returnData:" << containerCount;
    isWaiting[containerCount] = false;


    _connected = true;

    ReflectData2Envirement();//これの位置は審議
#if RESERCH_DATABACKTIME//データの作成、送信、受け取り、反映　の時間

    if (PhysXFarcade::isActiveRecord) {
        string timeStamp = objectDataList[0];
        long long time_back = _timeStamp.GetGoneTime(timeStamp);
        if (time_back < 0)return;
        double msec = time_back / 1000000.0f;
        _csvOutPutFactory->Update("backDataTime", msec);
    }
    else {
    }
#endif
//#if RESERCH_FPS
//    if (PhysXFarcade::isActiveRecord() && containerCount == 0) {
//        PhysXFarcade::AddRecieveCount();
//    }
//#endif

    //到着間隔を記録
    //if (containerCount == 0) {
    //    auto now = chrono::system_clock::now();
    //    auto dur = now.time_since_epoch();
    //    auto nowTime = chrono::duration_cast<chrono::microseconds>(dur).count();
    //    if (beforetime == 0)beforetime = nowTime;
    //    else {
    //        double msec = (nowTime - beforetime) / 1000000.0f;
    //        _csvOutPutFactory.Update("coalDuarance", msec);
    //        beforetime = nowTime;
    //    }
    //}
}
bool NormalPhysxEnvirement::isNeedUpdate() { return true; }