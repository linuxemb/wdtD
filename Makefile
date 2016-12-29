

include ../../../include.mk

# The application name.
APPNAME = watchdogd
# Application version.
# The major.minor version is defined first. If "TESTBUILD" is defined when
# calling "Make" then the TESTBUILD number is appended to the version.
VERSION := 0.3
ifdef TESTBUILD
VERSION := $(VERSION).$(TESTBUILD)
endif

# Make a list of all .cpp source files in the current directory.

cppsrcFiles = $(wildcard *.cpp)

# Create a list of object files from the source file list.
cpp_obj = $(addsuffix .o, $(basename $(cppsrcFiles)))

# Make a list of header files.
headers = $(wildcard *.h)

LIB += -L../../../OS/OAL
LIB += -L../../../System/HAL/watchdog
LIBS += -lpthread -lboost_system -lboost_thread -lboost_chrono -loal -lwatchdog
OPTIONS += -Wwrite-strings -std=c++0x

ifndef MY_DEBUG
MY_DEBUG=0
endif
DEFINES += -DMY_DEBUG=$(MY_DEBUG)

ifneq ($(PLATFORM), qca)
DEFINES += -DDEBUG_ON_PC
endif

# Build all goals
.PHONY:	all
all:	$(APPNAME)

# Full build
.PHONY:	full
full:		clean all package

# Clean-up
.PHONY:	clean
clean:
		$(RM) -f *.o
		$(RM) -f *~
		$(RM) -f core
		$(RM) -f $(APPNAME)
		$(RM) -f *.ipk
		$(RM) -f version.h
		$(RM) -fr ipk


# Build the unittest
.PHONY: unittest
unittest:
		@echo "Unit test target"
#build the OpenWRT package.
.PHONY:	package
package:
		(\
		mkdir -p $(HOME)/ipk; \
		mkdir -p ipk/usr/bin; \
		mkdir -p ipk/CONTROL; \
		echo "Package: $(APPNAME)" > ipk/CONTROL/control;\
		echo "Version: $(VERSION)" >> ipk/CONTROL/control;\
		echo "Depends:" >> ipk/CONTROL/control;\
		echo "Provides:" >> ipk/CONTROL/control;\
		echo "Source: $(cppsrcFiles)" >> ipk/CONTROL/control;\
		echo "Section: CoreFramework" >> ipk/CONTROL/control;\
		echo "Essential: no" >> ipk/CONTROL/control;\
		echo "Priority: optional" >> ipk/CONTROL/control;\
		echo "Maintainer: Tyco Developers" >> ipk/CONTROL/control;\
		echo "Architecture: ipq806x" >> ipk/CONTROL/control;\
		echo "Installed-Size: " >> ipk/CONTROL/control;\
		echo "Description:  This package contains an application for radio." \
			 >> ipk/CONTROL/control;\
		)
		$(CP) $(APPNAME) ipk/usr/bin
		$(IPKGBUILD) -c -o 0 -g 0 ipk
		$(CP) *.ipk $(HOME)/ipk

# Build the unittest
.PHONY: unittest



# Create the object files.
$(cpp_obj):	%.o: %.cpp $(headers) version.h
		$(GPP) $(DEFINES) $(INC) $(LIB) $(OPTIONS) -c $< -o $@
$(c_obj):	%.o: %.c $(headers) version.h
		$(GPP) $(DEFINES) $(INC) $(LIB) $(OPTIONS) -c $< -o $@

# Create the executable.
$(APPNAME):	$(c_obj) $(cpp_obj) $(headers)
		$(GPP) $(DEFINES) $(INC) $(LIB) $(OPTIONS) \
			$(cpp_obj) $(c_obj) $(LIBS) -o $(APPNAME)

# Create the version header file.
version.h:
		echo "const char *version=\"$(VERSION)\";" > version.h

-include deps.out

