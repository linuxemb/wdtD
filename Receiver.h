/*
 * Receiver.h
 * Define the receive call back function which is used to relay the data out.
 *  Created on: Mar 28, 2015
 *      Author: lisa
 */
#ifndef Receiver_H_
#define Receiver_H_

#include "../../../OS/OAL/UdpDevice.h"
#include <iostream>
#include "WatchdogInfo.h"
#include <boost/algorithm/string.hpp>
#include <queue>

class Receiver {
public:
   static bool mCanTolerant;
   Receiver(WatchdogInfo & pInfo);
   virtual ~Receiver();
   void Received(const unsigned char *data, size_t len, udp::endpoint &ep);

   void processQueue();
   void processRecvData();

   size_t getDataLen() const;

   string getDeadId() const;
   string getDeadPid() const;
   string getDeadName() const;

   bool hasNewMsg();

   void setNewMsgRecv(bool MsgCome);

   bool getCanTolerant() const;
   static void setCanTolerant(bool tol);
   // wdt staff

   void kickTaskWatchdog(const std::string TaskName);

   uint8_t addTaskToWatchdog(const WatchdogInfo& info);
   uint8_t removeTaskFromWatchdog(const WatchdogInfo& info);
   bool pollingTasks();

   queue<string> mRecvQueue;
private:
   bool mNewMsg;
   size_t mRecvDataLen;
   unsigned char * mRecvBuffer;
   string mRmtIP;
   string mStringBuffer;
   unsigned short mRmtPort;
   size_t min;
   vector<std::string> words;  // save each fields after received message split

   /*
    * brief
    * save Watchdoginfor object , which include
    * taskId ----- taskName-----counter------frequence
    */
   static vector<WatchdogInfo> mWdtInfo;
   //   queue <string> mRecvQueue;

   string mCommand;
   string mId;
   string mPid;
   string mTaskName;
   string mFreq;

// Dead task staff

   string mDeadId;
   string mDeadPid;
   string mDeadName;

// elememtn of WatchdogInfo

   WatchdogInfo mInfo;

   // helper function

   void printstrs(vector<string> strs);

   void printobjs(vector<WatchdogInfo> wdtinfo);
};

#endif /* Receiver_H_ */
