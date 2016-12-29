

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <stdio.h>
#include <cstring>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdint>


         class Configure {
         public:
            enum {
               MAXLINELENGTH = 1024U, LENGTH = 2U,
            };
            Configure();
            explicit Configure(std::string config);
            virtual ~Configure();

            std::map<std::string, std::string> & getOutput();
            uint32_t parse();
            void print();
         private:
            std::map<std::string, std::string> myMap;
            FILE *mFin;
         };


#endif /* CONFIGURE_H_ */
