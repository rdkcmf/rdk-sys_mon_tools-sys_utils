##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
RM          := rm -rf 
INSTALL_DIR := $(COMBINED_ROOT)/trm/install/bin/
CPPFLAGS    := -g -fPIC -D_REENTRANT -Wall 
OBJS        := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
IARM        := $(RDK_PROJECT_ROOT_PATH)/iarmbus
IARM_MGR    := $(RDK_PROJECT_ROOT_PATH)/iarmmgrs
TRM         := $(RDK_PROJECT_ROOT_PATH)/trm
EXECUTABLE  := QueryTRMInfo
CC      := $(CROSS_COMPILE)-gcc
CXX     := $(CROSS_COMPILE)-g++

INCLUDE = -I.\
	 -I$(COMBINED_ROOT)/trm/common/include \
	 -I$(COMBINED_ROOT)/trm/diag/include \
	 -I$(COMBINED_ROOT)/iarmbus/core/include \
	 -I$(JANSSON_DIR)/src \
	 -I$(OPENSRC)/include/ \
         -I$(IARM)/core/include\
         -I$(IARM_MGR)/generic/power/include\
         -I$(IARM_MGR)/generic/hal/include\
         -I$(TRM)/diag/include\
         -I$(TRM)/common/include\
	 -I$(OPENSRC)/include/uuid/
	

CPPFLAGS      += $(INCLUDE)

LDFLAGS +=  -ltrm -lpthread -ljansson -luuid -lIARMBus
LDFLAGS +=  -L$(RDK_FSROOT_PATH)/usr/local/lib -L$(IARM)/install -L$(TRM)/install/lib
LDFLAGS += 	-L$(OPENSRC)/lib
LDFLAGS +=	-Wl,-rpath,$(OPENSRC)/lib
LDFLAGS +=	-Wl,-rpath,$(RDK_FSROOT_PATH)/usr/local/lib

all: clean executable 
	@echo "Build Finished...."

executable: $(OBJS)
	@echo "Building $(EXECUTABLE) ...."
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)

%.o:%.cpp
	@echo "Building Source $@ ...."
	$(CXX) -c $<  $(CPPFLAGS) -o $@

clean:
	@echo "Cleaning the directory..."
	@$(RM) $(OBJS) $(EXECUTABLE)

install: all
	@echo "Copying trm utility to $(FSROOT)/usr/bin ..."
	@cp -f $(EXECUTABLE) $(FSROOT)/usr/bin

uninstall:
	@echo "Remove trm  utility from $(FSROOT)/usr/bin..."
	@rm -f $(FSROOT)/usr/bin/$(EXECUTABLE)

