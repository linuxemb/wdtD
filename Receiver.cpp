/*
 * Receiver.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: lisa
 */

#include "Receiver.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <boost/assign/list_of.hpp>
using namespace std;
using namespace boost::assign;
enum {
   PING = 0, REG = 1, UNREG = 2
};

vector<WatchdogInfo> Receiver::mWdtInfo = list_of(WatchdogInfo());

bool Receiver::mCanTolerant = true;
Receiver::Receiver(WatchdogInfo & pinfo) :
      mNewMsg(false), mRecvDataLen(0U), mRecvBuffer(0), min(0), mDeadId(""), mDeadPid(
            ""), mDeadName("") {
   mInfo = pinfo;

}

Receiver::~Receiver() {

}

bool Receiver::getCanTolerant() const {
   return mCanTolerant;
}

void Receiver::setCanTolerant(bool tol) {
   mCanTolerant = tol;
}

uint8_t Receiver::addTaskToWatchdog(const WatchdogInfo& info) {

   WatchdogInfo taskinfo = info;
   mWdtInfo.push_back(taskinfo);
   cout << "vector size = ||||" << mWdtInfo.size() << endl;
}

uint8_t Receiver::removeTaskFromWatchdog(const WatchdogInfo& info) {
   cout << "unregistering = ||||" << endl;
   for (auto it = mWdtInfo.begin(); it != mWdtInfo.end(); it++) {
      if (it->getTaskId() == info.getTaskId()) {
         cout << " i task =" << it->getTaskName() << endl;
         mWdtInfo.erase(it);
         cout << " unregistering received " << endl;
      } else
         ++it;

   }
   //  mWdtInfo.shrink_to_fit();
   // Shrink the size of mWdtInfo
   std::vector<WatchdogInfo>(mWdtInfo.begin(), mWdtInfo.end()).swap(mWdtInfo);
   cout << "new size of vector:" << mWdtInfo.size() << endl;
}
void Receiver::processQueue() {
   //  cout << "MY que recev buffer = " << mStringBuffer << endl;
   //  mRecvQueue.pop();
   cout << "mRecvqueue size = " << mRecvQueue.size() << endl;
   while (!mRecvQueue.empty()) {
      mStringBuffer = mRecvQueue.front();
      cout << "MY que recev buffer = " << mStringBuffer << endl;
      mRecvQueue.pop();
      processRecvData();
      mNewMsg = false;
   }
}

void Receiver::processRecvData()

{
   // split receved string to field based on delimiter of ','
   // input: mStringBuffer
   // output : words

   std::string s = mStringBuffer;
   cout << "processRecvData, mStringBuffer=" << mStringBuffer << endl;

   boost::split(words, s, boost::is_any_of(", "), boost::token_compress_on);
   cout << "words 1---" << words[1] << endl;
   printstrs(words);
// Vector words should have all split messages
   uint32_t cmd;
   uint32_t index = 0;

   mCommand = words.at(index);
   istringstream(mCommand) >> cmd;

   mTaskName = words[++index];

   mId = words.at(++index);
   cout << "id == " << mId << endl;

   mPid = words.at(++index);
   cout << "pid == " << mPid << endl;
   mFreq = words.at(++index);

   uint32_t freq;
   istringstream(mFreq) >> freq;

   // Gererate a obj for Watchdoginfo class

   mInfo.setTaskId(mId);
   mInfo.setTaskPid(mPid);

   mInfo.setTaskName(mTaskName);
   mInfo.setFrequence(freq);
   mInfo.setCount(0);

   switch (cmd) {
   case PING: {
      // find corresponding id and  reset
      for (auto i = mWdtInfo.begin(); i != mWdtInfo.end(); i++) {
         if (i->getTaskId() == mInfo.getTaskId()) {
            i->addCount();
            cout << " ping received " << "mCount = " << mInfo.getCount()
                  << "taskId-" << i->getTaskId() << endl;
         }
      }

      break;
   }
   case REG: {

      cout << "register called===============" << endl;
      // add task obj to vector mWdtInfo
      addTaskToWatchdog(mInfo);
      break;
   }

   case UNREG: {
      string task = "";
      cout << " unreg" << endl;
      removeTaskFromWatchdog(mInfo);
      break;
   }
   default:
      break;

   }

}

void Receiver::printstrs(vector<string> strs) {
   for (vector<string>::iterator it = strs.begin(); it != strs.end(); ++it) {
      cout << *it << "-------";
   }
   cout << endl;
}

void Receiver::printobjs(vector<WatchdogInfo> wdtinfo) {
   cout << "id---" << "name---" << "count---" << "freq---" << endl;
   for (vector<WatchdogInfo>::iterator iti = wdtinfo.begin();
         iti != wdtinfo.end(); ++iti) {

      cout << (*iti).getTaskId() << "--" << (*iti).getTaskName() << "--"
            << (*iti).getCount() << "--" << (*iti).getFrequence() << endl;
   }
   cout << endl;
   // cout << "Vector size =" << wdtinfo.size() << endl;
}
void Receiver::Received(const unsigned char *data, size_t len,
      udp::endpoint &ep) //string ip, uint16_t port)

      {
   //Buffer data and exit ASAP
   std::cout << "Received ..len===" << len << std::endl;

   mNewMsg = true;
   mRecvBuffer = (unsigned char*) data;
   mRecvDataLen = len;
   mRmtIP = ep.address().to_string();
   mRmtPort = ep.port();

   ///Get real data size been send which may different from len

   int realLen = 0;
   while (data[realLen] != '\0') {
      realLen++;
   }
   std::string sName((char*) data);
   //  string s(data, len);
   std::cout << "Get task information ===" << sName << endl;
   // buffer data and process
   mStringBuffer = sName;
   // Push sName to queue
   mRecvQueue.push(sName);
   cout << "Received: || mREcvQueue =" << mRecvQueue.front() << endl;
   mNewMsg = true;
   processRecvData();
   //processQueue();
}
bool Receiver::hasNewMsg() {
   return mNewMsg;
}
size_t Receiver::getDataLen() const {
   return mRecvDataLen;
}

string Receiver::getDeadId() const {
   return mDeadId;

}

string Receiver::getDeadPid() const {
   return mDeadPid;
}
string Receiver::getDeadName() const {
   return mDeadName;
}

void Receiver::setNewMsgRecv(bool MsgComing) {
   mNewMsg = MsgComing;
}

bool Receiver::pollingTasks() {

   cout << "polling" << endl;
   cout << "in polling mWdtinfo vector size = " << mWdtInfo.size() << endl;
   //   printobjs(mWdtInfo);

   for (auto i = mWdtInfo.begin(); i != mWdtInfo.end(); i++) {
//        cout << "task = | " << "{ " <<  i->getTaskName() << " } " << "----count = |||"
//                << i->getCount()  <<  "task id = " << i->getTaskId() << endl;
      cout << "count--" << i->getCount() << endl;
      cout << "tansk..." << i->getTaskName() << endl;
      cout << "Id..." << i->getTaskId() << endl;
      cout << endl;
      if (i->getCount() == 0 && i->getTaskName() != "") {
         // missed heartbeat once
         i->subFrequence();
         cout << " task freq  = " << i->getFrequence() << endl;
         if (i->getFrequence() == 0) {
            mDeadId = i->getTaskId();
            mDeadPid = i->getTaskPid();
            mDeadName = i->getTaskName();
            cout << " DEAD DEAD of id = " << i->getTaskId() << endl;
            cout << "DEAD task pid =" << mDeadPid << endl; // i->getTaskPid() << endl;
            cout << "DEAD task name =" << mDeadName << endl;
            mCanTolerant = false; // should reboot system or kill process
         }
      } else
         i->resetCounter();

   }
   return mCanTolerant;
}
