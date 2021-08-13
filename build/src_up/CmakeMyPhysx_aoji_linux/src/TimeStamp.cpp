#include "TimeStamp.h"
#include<iomanip>
using namespace std;
using namespace chrono;
string TimeStamp::GetTimeStamp()
{
    auto now = std::chrono::system_clock::now();
    auto dur = now.time_since_epoch();
    auto time=std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
    stringstream ss;
    ss << time;
    return ss.str();
}

long long TimeStamp::GetGoneTime(string timeStamp)
{
    //if (timeStamp.size() > 0 && std::all_of(timeStamp.cbegin(), timeStamp.cend(), isdigit)) {//êîílïœä∑ÇÃâ¬î€
    if (timeStamp.size() > 0 ) {//êîílïœä∑ÇÃâ¬î€

        auto now = std::chrono::system_clock::now();
        auto dur = now.time_since_epoch();
        auto nowTime = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

        long long beforeTime = stoll(timeStamp);
        return nowTime - beforeTime;
    }
    else {
        return -1;
    }
}

long long TimeStamp::GetFromEpockTime() {
    auto now = std::chrono::system_clock::now();
    auto dur = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}
