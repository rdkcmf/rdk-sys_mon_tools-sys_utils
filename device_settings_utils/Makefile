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
IARM        := $(RDK_PROJECT_ROOT_PATH)/iarmbus
IARM_MGR        := $(RDK_PROJECT_ROOT_PATH)/iarmmgrs
DEVICE_SETTINGS  := $(RDK_PROJECT_ROOT_PATH)/devicesettings
CC      := $(CROSS_COMPILE)-gcc
CXX     := $(CROSS_COMPILE)-g++

CFLAGS = -I$(IARM)/core/include\
	 -I$(IARM_MGR)/generic/power/include\
	 -I$(IARM_MGR)/generic/hal/include\
	 -I$(DEVICE_SETTINGS)/ds\
	 -I$(DEVICE_SETTINGS)/ds/include\
	 -I$(DEVICE_SETTINGS)/hal/include\
	 -I$(DEVICE_SETTINGS)/hal/src\
	 -I$(DEVICE_SETTINGS)/rpc/include\
         -I$(CROSS_TOOLCHAIN)/usr/local/include/directfb \
         -I$(FUSION_DALE)/include

LDFLAGS=-L. -L$(CROSS_TOOLCHAIN)/usr/lib -L$(FSROOT)/lib -L$(FSROOT)/usr/lib -L$(FSROOT)/usr/local/lib/ -L$(CROSS_TOOLCHAIN)/lib -L$(DEVICE_SETTINGS)/lib -L$(IARM)/install -L$(HAL_LDFLAGS)

FUSION_LIBS=-ldirect -lfusiondale -lfusion
GLIBS=-lglib-2.0 -lz 

all: clean getDeviceSettings.cpp
	@echo "Building device settings query utility ....."
	$(CXX) $(CFLAGS) $(LDFLAGS) $(ADDED_LDFLAGS) $(GLIBS) -ldbus-1 -lds -ldshalcli -lIARMBus  getDeviceSettings.cpp -o QueryDSState 

clean:
	@rm -rf *.o
	@rm -rf QueryDSState 

install: all
	@echo "Copying QueryDSState to $(FSROOT)/usr/bin ..."
	@cp -f QueryDSState $(FSROOT)/usr/bin

uninstall:
	@echo "Remove device settings query utility from $(FSROOT)/usr/bin..."
	@rm -f $(FSROOT)/usr/bin/QueryDSState
