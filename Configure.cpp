/**********************************************************************************************************

 **********************************************************************************************************/

#include "Configure.h"
#include <iostream>

namespace Tyco {
   namespace Lib {
      namespace Watchdog {
         Configure::Configure() {
         }

         Configure::Configure(std::string config) {
            mFin = fopen(config.c_str(), "r");
            if (!mFin) {
               std::cerr << "Unable to open config file! " << std::endl;
            }
         }

         Configure::~Configure() {

            fclose(mFin);
         }

         uint32_t Configure::parse(void) {
            char input[MAXLINELENGTH];
            uint32_t len;
            uint32_t ptr;
            uint32_t ptrnext;
            bool found;
            while (fgets(input, MAXLINELENGTH, mFin)) {
               if (input[0] == '#') {
                  continue;
               }

               len = strlen(input);
               if (len < LENGTH) {
                  continue;
               }
               if ((input[len - 1] == '\n') || (input[len - 1] == '\r\n')) {
                  input[len - 1] = '\0';
               }
               ptr = static_cast<uint32_t>(0);
               found = false;
               while (ptr < len) {
                  if (input[ptr] == '=') {
                     found = true;
                     ptrnext = ptr;
                     ptr = ptr + 1;
                     input[ptrnext] = '\0';

                     break;
                  }
                  ptr++;
               }
               if (found) {
                  myMap[input] = &input[ptr];
               }
            }
            return 0;
         }

         std::map<std::string, std::string> & Configure::getOutput() {

            return myMap;
         }
         void Configure::print() {
            std::map<std::string, std::string>::iterator myIterator =
                  myMap.begin();
            while (myIterator != myMap.end()) {
               printf("\t%s\t%s\n", myIterator->first.c_str(),
                     myIterator->second.c_str());
               myIterator++;
            }
         }
      }   // end of namespace: Watchdog
   }   // end of namespace: Lib
}   // end of namespace: Tyco
