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
CPPFLAGS    := -g -fPIC -D_REENTRANT -Wall 
OBJS        := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
IARM        := $(RDK_PROJECT_ROOT_PATH)/iarmbus
IARM_MGR    := $(RDK_PROJECT_ROOT_PATH)/iarmmgrs
TRM         := $(RDK_PROJECT_ROOT_PATH)/trm
INSTALL     := $(FSROOT)/usr
LIBNAME     := trmdiaginfo
LIBNAMEFULL := lib$(LIBNAME).so
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

all: install
	@echo "Build Finished...."

library: $(OBJS)
	@echo "Building $(LIBNAMEFULL) ...."
	$(CXX) $(OBJS) $(CPPFLAGS) -shared -o $(LIBNAMEFULL)

%.o: %.cpp
	@echo "Building $@ ...."
	$(CXX) -c $<  $(CPPFLAGS) -o $@

%.o: %.c
	@echo "Building $@ ...."
	$(CXX) -c $<  $(CPPFLAGS) -o $@

install: library
	@echo "installling...!"
	@cp -f trmDiagInfo.h $(INSTALL)/include
	@cp -f $(LIBNAMEFULL) $(INSTALL)/lib

uninstall:
	@$(RM) $(INSTALL)/lib/$(LIBNAMEFULL)
	@$(RM) $(INSTALL)/include/trmDiagInfo.h

clean: uninstall
	@echo "Cleaning the directory..."
	@$(RM) $(OBJS) $(LIBNAMEFULL)

