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
    //_csvOutPutFactory.AddDataSet("timestamp", 1, 100);　帰ってきたデータが帰ってくるまでにかかる時間
    //_csvOutPutFactory.AddDataSet("reflectdata", 1, 100);　データの反映にかかる時間
    _csvOutPutFactory.AddDataSet("coalDuarance", 1, 100);// 呼び出し間隔

    //moveWall = _physxObjectCreator->createDynamic(PxTransform(PxVec3(8, 0, -5)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-42, 0, -55)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-42, 0, -25)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-25, 0, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(-59, 0, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);


    _physxObjectCreator->createDynamic(PxTransform(PxVec3(50, 0, -55)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "moveWall", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(50, 0, -25)), PxBoxGeometry(16.0f, 40.0f, 1.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(67, 0, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);
    _physxObjectCreator->createDynamic(PxTransform(PxVec3(33, 0, -43)), PxBoxGeometry(1.0f, 40.0f, 16.0f), "", true);

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
    else {
        if (_connected) {
            if (isWaiting.size()==2) {
                //isWaiting[0] = true;
                //isWaiting[1] = true;
                ReflectData2Envirement();
            }
        }
    }
    
}
void NormalPhysxEnvirement::CreateFrameMessage() {
    if (_connected) {
        if (isWaiting.size() == 2) {
            WebSocketppFarcade::Send("back", "", 0);
            WebSocketppFarcade::Send("back", "", 1);
            
            //cout << "create message" << endl;
        }
    }
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
                string log = CreateLog(i, i);
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
            string log = CreateLog(i, i);
            if (log == "")continue;
            ss << "envirement_objectdata," << log;
            //WebSocketppFarcade::Send(ss.str(), i);
            WebSocketppFarcade::Send("envirement_objectdata",log, i);
        }
        break;
    case 'T':
        _csvOutPutFactory.OutPutCSV();

        //string data = "T,0,0";
        //WebSocketppFarcade::Send("physx_input", data, 0);
        break;
    }
}
void NormalPhysxEnvirement::ReflectData2Envirement() {
    if (_islocal)return;
    if (_gScene == NULL)return;

    if (returnData.size() <= 0)return;
    for (int k = 0; k < returnData.size(); k ++) {
        vector<string> objectDataList = MyExtention::Split(returnData[k], '\n');


        returnData[k] = "";

        if(objectDataList.size()>1)cout << "normal reflect " << k <<":"<< objectDataList.size() << endl;

        if (objectDataList.size() <= 2)continue;//反映するものがない判定

        //cout << "reflect env" << endl;
        //auto start = chrono::system_clock::now();//処理時間計測開始
        for (int i = 0; i < objectDataList.size(); i++) {
            
            vector<string> labelList = MyExtention::Split(objectDataList[i], ';');
            if (labelList.size() != 7)continue;
            //containerの抽出
            string container = MyExtention::Split(labelList[0], ':')[1];
            int containerCount = stoi(container);
            //labelの抽出
            string label = MyExtention::Split(labelList[1], ':')[1];

            //transformの抽出
            string trText = MyExtention::Split(labelList[4], ':')[1];
            vector<string> trValues = MyExtention::Split(trText, ',');
            PxVec3* pos = new PxVec3(stof(trValues[0]), stof(trValues[1]), stof(trValues[2]));

            //quatanionの抽出
            string quatText = MyExtention::Split(labelList[5], ':')[1];
            vector<string> quatValues = MyExtention::Split(quatText, ',');
            PxQuat* quat = new PxQuat(stof(quatValues[0]), stof(quatValues[1]), stof(quatValues[2]), stof(quatValues[3]));

            //対応するオブジェクトの位置を反映
            auto targetObject = _physxObjectCreator->FindObject(label, containerCount);
            if (targetObject == NULL)continue;
            auto transform = targetObject->_objectData->getGlobalPose();
            transform.p = *pos;
            transform.q = *quat;
            targetObject->_objectData->setGlobalPose(transform);

            //std::this_thread::sleep_for(std::chrono::milliseconds(1));//重すぎてめっちゃ待たないといけない　オブジェクト生成方法を改善したら治せるかも？
        }

        //auto dur = chrono::system_clock::now() - start;
        //auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();//処理時間(msec)
        //float sec = misec / 1000000.0f;//処理時間(sec)
        //_csvOutPutFactory.Update("reflectdata", sec);
    }
}
//int NormalPhysxEnvirement::JudgePartialSpaceNumber(PxVec3 pos) {
//    int parialLine = 0;
//    if (pos.x < parialLine)return 0;
//    else return 1; 
//}

void NormalPhysxEnvirement::SetReflectData(string data) {
    if (_islocal)return;
    if (returnData.size() == 0) {
        returnData.push_back("");
        returnData.push_back("");
        isWaiting.push_back(false);
        isWaiting.push_back(false);
    }

    vector<string> objectDataList = MyExtention::Split(data, '\n');
    if (objectDataList.size() < 2)return;
    vector<string> labelList = MyExtention::Split(objectDataList[1], ';');
    if (labelList.size() != 7)return;
    //containerの抽出
    string container = MyExtention::Split(labelList[0], ':')[1];
    int containerCount = stoi(container);
    returnData[containerCount] = data;
    cout << "returnData:" << containerCount;
    isWaiting[containerCount] = false;


    _connected = true;

    //タイムスタンプ処理
    //if (_csvOutPutFactory.IsActive()) {
    //    string timeStamp = objectDataList[0];
    //    long long time = _timeStamp.GetGoneTime(timeStamp);
    //    if (time < 0)return;
    //    double msec = time / 1000000.0f;
    //    cout << "stamp:" << timeStamp << " sec" << msec << endl;
    //    _csvOutPutFactory.Update("timestamp", msec);
    //}
    //else {
    //}

    //到着間隔を記録
    if (containerCount == 0) {
        auto now = std::chrono::system_clock::now();
        auto dur = now.time_since_epoch();
        auto nowTime = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
        if (beforetime == 0)beforetime = nowTime;
        else {
            double msec = (nowTime - beforetime) / 1000000.0f;
            _csvOutPutFactory.Update("coalDuarance", msec);
            beforetime = nowTime;
        }
    }
}