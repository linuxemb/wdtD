
#include "../../../OS/OAL/UdpDevice.h"
#include <cstdint>
#include "stdint.h"
#include <stdio.h>
#include "Receiver.h"
#include <sstream>
#include <string>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Configure.h"
#include "../../../System/HAL/watchdog/wdt.h"
#include "signal.h"

using namespace Tyco::System::Oal;
using namespace TycoSecurityProducts::System::Hal;
using namespace Tyco::Lib::Watchdog;
template<typename T>
string NumberToString(T pNumber) {
   std::ostringstream oOStrStream;
   oOStrStream << pNumber;
   return oOStrStream.str();
}

static const string LOCAL_HOST = "127.0.0.1";
static const uint8_t TIME_OUT = 3;

std::map<std::string, std::string> mConfig; // for config staff.

// Helper function to change int ip to string
std::string& intIPtoString(uint32_t ip) {
   struct in_addr addr = { ip };
   static std::string myIP = inet_ntoa(addr);
   std::cout << "MY IP ====" << myIP;
   return myIP;
}

// Helper function to change string to int,
// Using stringstream way without haveing a named object
uint16_t stringToInt(string str) {

   istringstream buffer(str);
   uint32_t value;
   buffer >> value;
   return value;
}

//======================================================================================
//  PC test version main
//===================================================================================

 int main(int argc, char* argv[]) {

 UdpDevice *socket_server;
 WatchdogInfo minfo;

 Receiver socket_receiver(minfo);

 // Add config staff
 std::map<std::string, std::string> mConfig; // for config staff.
 std::string str = "config.txt";
 Configure con(str);
 con.parse();
 mConfig = con.getOutput();
 // for debug
 std::map<std::string, std::string>::iterator myIterator = mConfig.begin();
 while (myIterator != mConfig.end()) {
 printf("\t%s\t%s\n", myIterator->first.c_str(),
 myIterator->second.c_str());

 myIterator++;
 }

 // find the local port for server
 string localPortStr;
 auto iter = mConfig.find("PORT");
 if (iter != mConfig.end()) {
 localPortStr = iter->second;
 cout << "found localport str===== = " << localPortStr << endl;
 }

 //=====================
 uint16_t localPort = stringToInt(localPortStr);

 std::string localipStr = LOCAL_HOST;

 try {

 socket_server = new UdpDevice(localipStr, localPort);
 socket_server->setAppRevCallback(
 boost::bind(&Receiver::Received, socket_receiver, _1, _2, _3));

 } catch (std::exception& e) {
 std::cerr << e.what() << "exception happened " << std::endl;
 }
 // Wait for registration done
 //  bool gotMessage = socket_receiver.hasNewMsg();
 sleep(5);
 //   while (1) { //gotMessage)  {  // && !socket_receiver.mRecvQueue.empty()) {
 //        socket_receiver.processQueue();
 //        cout << "processRecedqueue" << endl;

 // }

 // =================== Monitor tasks in each 3 s for PC test
 while (true) {

 sleep(TIME_OUT);
 // Check if find some dead task, then compare with config file to take different measure
 bool canSupport = socket_receiver.pollingTasks();
 if (!canSupport) { // Mean some task dead
 // Need reset receiver::mCanTollernet ,, for next iteration
 Receiver::setCanTolerant(true);
 cout << " need check config file" << endl;
 // need check config file to decide next step
 string dead = socket_receiver.getDeadName();
 cout << "dead ==" << dead << endl;
 string action;
 auto iter = mConfig.find(dead);
 if (iter != mConfig.end()) {
 action = iter->second;
 cout << "found dead action ===== = " << action << endl;
 }
 // find task priority in config file
 if (action == "respawn") {
 // need to kill the task process
 string deadPidStr = socket_receiver.getDeadPid();
 uint16_t deadPid = stringToInt(deadPidStr);
 kill(deadPid, SIGKILL);
 cout << " kill process pid ==" << deadPid << endl;
 // kill process & ping watchdo dog
 cout << " ping......" << endl;
 }

 else if (action == "reboot") {
 return 0;  // system will reboot
 }
 } else {
 // Need ping dog, for no dead tasks
 cout << " ping......" << endl;
 }
 }
 return 0;
 }

//======================================================================================
//  HW target  version main
//===================================================================================
/*
 int main(int argc, char* argv[]) {

   UdpDevice *socket_server;
   WatchdogInfo minfo;

   Receiver socket_receiver(minfo);

   // Add config staff
   std::map<std::string, std::string> mConfig; // for config staff.
   std::string str = "config.txt";
   Configure con(str);
   con.parse();
   mConfig = con.getOutput();
   // for debug
   std::map<std::string, std::string>::iterator myIterator = mConfig.begin();
   while (myIterator != mConfig.end()) {
      printf("\t%s\t%s\n", myIterator->first.c_str(),
            myIterator->second.c_str());

      myIterator++;
   }

   // find the local port for server
   string localPortStr;
   auto iter = mConfig.find("PORT");
   if (iter != mConfig.end()) {
      localPortStr = iter->second;
      cout << "found localport str===== = " << localPortStr << endl;
   }

   //=====================
   uint16_t localPort = stringToInt(localPortStr);

   std::string localipStr = LOCAL_HOST;

   try {

      socket_server = new UdpDevice(localipStr, localPort);
      socket_server->setAppRevCallback(
            boost::bind(&Receiver::Received, socket_receiver, _1, _2, _3));

   } catch (std::exception& e) {
      std::cerr << e.what() << "my test" << std::endl;
   }

   // Wait for registration done
   //  bool gotMessage = socket_receiver.hasNewMsg();
   sleep(5);
   //   while (1) { //gotMessage)  {  // && !socket_receiver.mRecvQueue.empty()) {
   //        socket_receiver.processQueue();
   //        cout << "processRecedqueue" << endl;

   // }

   // Prepare is done, need to populate Watchdog instance and ping it===============================

   try {
      Watchdog mWatchdog;
      // setup watchdog, make it in state of ready to be kicked
      bool dogOk = mWatchdog.setup();

      // each 3s kick dog once
      sleep(TIME_OUT);
      // Check if find some dead task, then compare with config file to take different measure
      bool canSupport = socket_receiver.pollingTasks();
      if (!canSupport) { // Mean some task dead
         // Need reset receiver::mCanTollernet ,, for next iteration
         Receiver::setCanTolerant(true);
         cout << " need check config file" << endl;
         // need check config file to decide next step
         string dead = socket_receiver.getDeadName();
         cout << "dead ==" << dead << endl;
         string action;
         auto iter = mConfig.find(dead);
         if (iter != mConfig.end()) {
            action = iter->second;
            cout << "found dead action ===== = " << action << endl;
         }
         // find task priority in config file
         if (action == "respawn") {
            // need to kill the task process
            string deadPidStr = socket_receiver.getDeadPid();
            uint16_t deadPid = stringToInt(deadPidStr);
            kill(deadPid, SIGKILL);
            cout << " kill process pid ==" << deadPid << endl;
            // kill process & ping watchdo dog
            cout << " ping......" << endl;
            if (dogOk) {
               mWatchdog.kickDog();
            }
         }

         else if (action == "reboot") {
            return 0;  // system will reboot
         }
      } else {
         // Need ping dog, for no dead tasks
         cout << " ping......" << endl;
         if (dogOk) {
            mWatchdog.kickDog();
         }
      }

   } catch (std::string *excep) {
      std::cerr << " exception occurred :" << *excep << std::endl;
      return 1;
   } catch (std::exception &ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
      return 1;
   }

   catch (...) {
      std::cerr << " Unknown error !" << std::endl;
   }

   return 0;
}
*/
