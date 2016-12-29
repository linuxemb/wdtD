
#ifndef WATCHDOGINFO_H_
#define WATCHDOGINFO_H_
#include <string>
using namespace std;
class WatchdogInfo {
public:
   WatchdogInfo(std::string id, int cnt);

   WatchdogInfo() {
   }
   ;

   virtual ~WatchdogInfo();

   /*
    *
    * task Id
    */
   void setTaskId(string id);
   string getTaskId() const;
   /*
    *
    * task Pid
    */
   void setTaskPid(string pid);
   string getTaskPid();

   /*
    *
    * task name
    */
   void setTaskName(string name);
   string getTaskName();

   /*
    *
    * task frequence
    */
   void setFrequence(uint32_t freq);
   uint32_t getFrequence() const;
   void subFrequence();

   /*
    *
    * task count
    */
   uint32_t getCount() const;
   void setCount(uint32_t cnt);
   void resetCounter();
   void addCount();

private:

   string mPid;
   string mTid;
   string mId;
   string mTaskName;
   uint32_t mFreq;
   uint32_t mCount;
};

#endif /* WATCHDOGINFO_H_ */
