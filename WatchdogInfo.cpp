
#include "WatchdogInfo.h"
#include <iostream>
#include <boost/thread/mutex.hpp>

using namespace std;
boost::mutex c_mutex;

WatchdogInfo::WatchdogInfo(std::string id, int cnt) :
      mPid(id), mTid(id), mCount(cnt) {

}
//  Auto-generated destructor stub
WatchdogInfo::~WatchdogInfo() {

}

//WE send emppty string "" for Tid field if it is  process

void WatchdogInfo::setTaskId(string id) {
   mId = id;
}

string WatchdogInfo::getTaskId() const {
   return mId;
}

// task pid staff

void WatchdogInfo::setTaskPid(string pid) {
   mPid = pid;

}

string WatchdogInfo::getTaskPid() {
   return mPid;
}

// Task frequence staff

void WatchdogInfo::setFrequence(uint32_t freq) {
   mFreq = freq;
}

void WatchdogInfo::setTaskName(string name) {

   mTaskName = name;
   cout << " task name of watchdoginfo = " << mTaskName << endl;
}

string WatchdogInfo::getTaskName() {

   return mTaskName;

}

uint32_t WatchdogInfo::getFrequence() const {
   return mFreq;
}

void WatchdogInfo::subFrequence() {
   mFreq--;
}
uint32_t WatchdogInfo::getCount() const {
   return mCount;
}
void WatchdogInfo::setCount(uint32_t cnt) {
   mCount = cnt;
}

void WatchdogInfo::resetCounter() {

   boost::mutex::scoped_lock lock(c_mutex);
   mCount = 0;
}

void WatchdogInfo::addCount() {
   boost::mutex::scoped_lock lock(c_mutex);
   mCount++;
}
