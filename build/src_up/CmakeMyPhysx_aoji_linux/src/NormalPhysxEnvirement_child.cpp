#include "NormalPhysxEnvirement_child.h"
#include "Flags.h"

void NormalPhysxEnvirement_child::CreateStack_ballpool(int lx, float height, int lz, float ballsize) {
    //for (int x = 0; x < lx; x++) {
    //    for (int z = 0; z < lz; z++) {
    //        _physxObjectCreator->createDynamic(PxTransform((PxVec3(x, height, z) * (ballsize + 1))), PxSphereGeometry(ballsize));
    //    }
    //}
}

void NormalPhysxEnvirement_child::ChengeBallSpeed(float dv) {
    _ballSpeed += dv;
    if (_ballSpeed < 0)_ballSpeed = 0;
}

void NormalPhysxEnvirement_child::InitPhysicsEnviourment()
{
    WebSocketppFarcade::SetBackMessage([&](string timeStamp) {
        m_startSimulate = true;
        return "";
        //書き換え中はupdate止めないとだめ

//#if RESERCH_SIMULATETIME
//        auto start_simulate = chrono::system_clock::now();//処理時間計測開始
//#endif
//        //UpdateEnviroment(0.02f);
//        //MoveWall(0.02f);
//        //PhysxMain::StepPhysics(0.02f);
//#if RESERCH_SIMULATETIME
//        {
//            if (PhysXFarcade::isActiveRecord()) {
//                auto dur = chrono::system_clock::now() - start_simulate;
//                auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();//処理時間(msec)
//                float sec = misec / 1000000.0f;//処理時間(sec)
//                _csvOutPutFactory->Update("simulate", sec);
//            }
//        }
//#endif

//#if RESERCH_MAKEODTIME
//        auto start_makeod = std::chrono::system_clock::now();
//#endif
//        HashLogSet();
//#if RESERCH_MAKEODTIME
//        {
//            auto dur = std::chrono::system_clock::now() - start_makeod;
//            auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
//            float sec = misec / 1000000.0f;
//            if (PhysXFarcade::isActiveRecord()) {
//                _csvOutPutFactory->Update("makeOD", sec);
//            }
//        }
//#endif
//        stringstream ss;
//        ss << timeStamp << "\n" << _hashLog;
//        string result = ss.str();
        //return result;
    });
    ReflectData2Envirement();
    HashLogSet();

    _csvOutPutFactory = PhysXFarcade::GetCSVFactory();
#if RESERCH_MAKEODTIME
    _csvOutPutFactory->AddDataSet("makeOD", 1, 100);
#endif
#if RESERCH_SIMULATETIME
    _csvOutPutFactory->AddDataSet("simulate", 1, 500);
#endif
}

void NormalPhysxEnvirement_child::UpdateEnviroment(float dt)
{
    if (!m_startSimulate)return;
    ReflectData2Envirement();

#if RESERCH_SIMULATETIME//処理時間計測開始
    auto start_simulate = chrono::system_clock::now();
#endif
    MoveWall(dt);
    PhysxMain::StepPhysics(dt);
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

void NormalPhysxEnvirement_child::CreateFrameMessage() {

    if (!m_startSimulate)return;
#if NOTHREAD
    string timeStamp = m_timeStamp.GetTimeStamp();//データの製作開始からカウント
    #if RESERCH_MAKEODTIME
        auto start_makeod = std::chrono::system_clock::now();
    #endif
    HashLogSet();
    #if RESERCH_MAKEODTIME
        {
            auto dur = std::chrono::system_clock::now() - start_makeod;
            auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
            float sec = misec / 1000000.0f;
            if (PhysXFarcade::isActiveRecord()) {
                _csvOutPutFactory->Update("makeOD", sec);
            }
        }
    #endif
    #if EASE_HOST
        m_easyHostCount++;
        if (m_easyHostCount == 3) {
            m_easyHostCount = 0;
            WebSocketppFarcade::Send("", _hashLog, timeStamp, 0);
        }
    #else
        WebSocketppFarcade::Send("", _hashLog, timeStamp, 0);
    #endif
#else 
    std::thread th([&] {
            string timeStamp = m_timeStamp.GetTimeStamp();//データの製作開始からカウント
    #if RESERCH_MAKEODTIME
            auto start_makeod = std::chrono::system_clock::now();
    #endif
            HashLogSet();
    #if RESERCH_MAKEODTIME
            {
                auto dur = std::chrono::system_clock::now() - start_makeod;
                auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
                float sec = misec / 1000000.0f;
                if (PhysXFarcade::isActiveRecord()) {
                    _csvOutPutFactory->Update("makeOD", sec);
                }
            }
    #endif
    #if EASE_HOST
            m_easyHostCount++;
            if (m_easyHostCount == 3) {
                m_easyHostCount = 0;
                WebSocketppFarcade::Send("", _hashLog, timeStamp, 0);
            }
    #else
            WebSocketppFarcade::Send("", _hashLog, timeStamp, 0);
    #endif
        });
    th.detach();
#endif
   
//    string timeStamp = m_timeStamp.GetTimeStamp();//データの製作開始からカウント
//#if RESERCH_MAKEODTIME
//    auto start_makeod = std::chrono::system_clock::now();
//#endif
//    HashLogSet();
//#if RESERCH_MAKEODTIME
//    {
//        auto dur = std::chrono::system_clock::now() - start_makeod;
//        auto misec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
//        float sec = misec / 1000000.0f;
//        if (PhysXFarcade::isActiveRecord()) {
//            _csvOutPutFactory->Update("makeOD", sec);
//        }
//    }
//#endif
//    WebSocketppFarcade::Send("", _hashLog, timeStamp, 0);
}


void NormalPhysxEnvirement_child::MoveWall(float dt) {
    if (!wallmoveFlag)return;
    if (moveWall == NULL) {
        auto temp = _physxObjectCreator->FindObject("moveWall", 0);
        if (temp != NULL)moveWall = (PxRigidDynamic*)temp->_objectData;
    }
    if (moveWall == NULL)return;
    PxVec3 pos = moveWall->getGlobalPose().p;
    if (isUp && pos.z > maxpos)isUp = false;
    else if (!isUp && pos.z < minpos)isUp = true;
    float sign = 0;
    sign = (isUp) ? -1.0f : 1.0f;
    PxTransform next_position(moveWall->getGlobalPose().p - PxVec3(0.0f, 0.0f, dt * wallSpeed * sign));//位置のみを指定しているが、角度も指定できるよ
    moveWall->setKinematicTarget(next_position);
}
void NormalPhysxEnvirement_child::SetReflectData(string data) {
    reflectData = data;
    //ReflectData2Envirement();
}

void NormalPhysxEnvirement_child::keyPress(unsigned char key, const PxTransform& camera)
{
    switch (toupper(key))
    {
    case ' ': {
        //_physxObjectCreator->createDynamic_inPlay(camera, PxBoxGeometry(2.0f, 2.0f, 2.0f), camera.rotate(PxVec3(-0.3f, 0, -1)) * _ballSpeed);
        _physxObjectCreator->createDynamic(camera, PxBoxGeometry(2.0f, 2.0f, 2.0f),"", false, camera.rotate(PxVec3(-0.3f, 0, -1)) * _ballSpeed);
        break;
    }
    case 'Q':ChengeBallSpeed(10.0f); break;
    case 'E':ChengeBallSpeed(-5.0f); break;
    case 'B':
        //CreateStack_ballpool(length_x, height, length_z, _ballsize);
        break;
    case 'X':
        wallmoveFlag = !wallmoveFlag;
        break;
    case 'T':
        //_csvOutPutFactory.OutPutCSV();
        break;
    }
}

void NormalPhysxEnvirement_child::ReflectData2Envirement()
{
    if (_gScene == NULL)return;


    if (reflectData == "")return;
    m_objectDataList = MyExtention::Split(reflectData, '\n');
    reflectData = "";
    for (int i = 0; i < m_objectDataList.size(); i++) {
        m_labelList = MyExtention::Split(m_objectDataList[i], ';');
        //containerの抽出
        string container = m_labelList[0].substr(2);
        _containerCount = stoi(container);
        //labelの抽出
        string label = m_labelList[1].substr(2);
        if (_physxObjectCreator->IsContain(label, 0)) {
            continue;
        }
        else {
            cout << label << endl;
        }
        //IsDynamicの抽出
        bool isDynamic = m_labelList[2].substr(2) == "1";
        //IsKinematicの抽出
        bool isKinematic = m_labelList[3].substr(2) == "1";
        //transformの抽出
        string trText = m_labelList[4].substr(2);
        vector<string> trValues = MyExtention::Split(trText, ',');
        m_pos.x = stof(trValues[0]);
        m_pos.y = stof(trValues[1]);
        m_pos.z = stof(trValues[2]);

        //quatanionの抽出
        string quatText = m_labelList[5].substr(2);
        vector<string> quatValues = MyExtention::Split(quatText, ',');
        m_quat.x = stof(quatValues[0]);
        m_quat.y = stof(quatValues[1]);
        m_quat.z = stof(quatValues[2]);
        m_quat.w = stof(quatValues[3]);
        
        //geometory
        string geoinfo = m_labelList[6].substr(2);

        if (isDynamic)_physxObjectCreator->createDynamic(PxTransform(m_pos, m_quat), *ObjectData_toHolder::ConvertText2Geometry(geoinfo), label, isKinematic);
        else _physxObjectCreator->createStatic(PxTransform(m_pos,m_quat), *ObjectData_toHolder::ConvertText2Geometry(geoinfo), label);

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));//重すぎてめっちゃ待たないといけない　オブジェクト生成方法を改善したら治せるかも？
    }

}

//int NormalPhysxEnvirement_child::JudgePartialSpaceNumber(PxVec3 pos) {
//    return 0;
//    /*int parialLine = 0;
//    if (pos.x > parialLine)return 0;
//    else return 1;*/
//}

void NormalPhysxEnvirement_child::HashLogSet() {
    _hashLog = CreateLog(0, _containerCount,true);

}

bool NormalPhysxEnvirement_child::isNeedUpdate() { return true; }