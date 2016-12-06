/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
 
#include <iostream>
#include <string.h>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "frontPanelConfig.hpp"
#include "frontPanelTextDisplay.hpp"
#include "videoOutputPort.hpp"
#include "videoOutputPortType.hpp"
#include "videoResolution.hpp"
#include "host.hpp"
#include "manager.hpp"

#include "dsUtl.h"
#include "dsError.h"
#include "list.hpp"
#include "libIBus.h"
#include "unsupportedOperationException.hpp"

#define SIZEOF_ARR(arr) (sizeof((arr))/sizeof((arr)[0]))

const char* validParams[] = {"--hdcpTvPwrState","--setTimeFormat"};
const int numberOfParams = SIZEOF_ARR(validParams) ;
const int RX_SENSE_UNSUPPORTED = 4 ;


void displayHelp() {
     printf("Usage : QueryDSState [CMD] \n");
     printf("CMDs are \n" );
     printf("%5s -> %s \n","--help", "print this help.");
     printf("%5s -> %s \n","--hdcpTvPwrState", "Get HDCP Power State");
     printf("%16s -> %s \n","    Output :", "ON(1), OFF(2), Unknown(3), NOT Supported(4)");
     printf("%5s -> %s \n","--setTimeFormat <TimeFormat>", "Set Time Format");
     printf("%16s -> %s \n","    Params :", "12_HR(0), 24_HR(1),NOT Supported(2)");

}

/**
   Return the index of parameter to be retrived if valid.
   If not valid return -1 and display the help screen
**/
int validateParams(const char* param) {
    int paramIndex = -1 ;
    
    if (param != NULL)
    {
        for ( int i=0; i < numberOfParams; i++ ) {
            if (strcmp(param, validParams[i]) == 0 ) {
                paramIndex = i ;
                break ;
            }
        }
    }
    return paramIndex;
}

void getHDCPTvPowerState() {


    FILE fp_old = *stdout;  // preserve the original stdout
    // redirect stdout to null to avoid printing debug prints from other modules
    *stdout = *fopen("/dev/null","w");
    /*Unknown needs to be displayed if Component out is active*/
    int hdcpStatus = 3;
    bool isHDMIConnected = false;
    bool isHDMIActive = false;
    IARM_Bus_Init("DsUtility");
    IARM_Bus_Connect();

    try {
        device::Manager::Initialize();
        device::VideoOutputPort vPort = device::Host::getInstance().getVideoOutputPort(std::string("HDMI0"));

        isHDMIConnected = vPort.isDisplayConnected();
        if ( isHDMIConnected == true ) {
            try {
                isHDMIActive = vPort.isActive();
                if ( isHDMIActive == true ) {
                    hdcpStatus = 1; // ON
                } else {
                    hdcpStatus = 2; // OFF
                }
            } catch (device::UnsupportedOperationException &e) { // This happens for pltforms where SoC doesn't support RxSense
                hdcpStatus = RX_SENSE_UNSUPPORTED ;
            }
        } else {
            hdcpStatus = 3 ; // Unknown
        }

        device::Manager::DeInitialize();
    }
    catch (...) {
        printf("Exception Caught during [%s]\r\n",__func__);
    }


    IARM_Bus_Disconnect();
    IARM_Bus_Term();

    *stdout=fp_old;  // restore stdout

    printf("%d\r\n", hdcpStatus );

}

void setFPTimeFormat(int timeformat) {

    IARM_Bus_Init("DsUtility");
    IARM_Bus_Connect();

    try {
        device::Manager::Initialize();
        device::FrontPanelConfig::getInstance().getTextDisplay("Text").setTimeFormat(timeformat);
        printf("Time Format change to  %s \r\n",timeformat == dsFPD_TIME_12_HOUR ?"12_HR":"24_HR");
        device::Manager::DeInitialize();
    }
    catch (...) {
        printf("Exception Caught during [%s]\r\n",__func__);
    }

    IARM_Bus_Disconnect();
    IARM_Bus_Term();
}

int main(int argc, char *argv[]) 
{

    int paramIndex = 0;
    int paramArg = 0;

    if (argc > 3) {
        displayHelp();
        return -1;
    }

    paramIndex = validateParams(argv[1]);

    if( paramIndex == -1 ){
        displayHelp();
        return -1;
    }

    switch(paramIndex) {
        /*Check for validParams array for parameter name mapping*/
        case 0 :
               getHDCPTvPowerState();
               break;
        case 1 :
               if(argc == 3)
               {
                   paramArg = atoi((const char *)argv[2]);
                   setFPTimeFormat(paramArg);
               }
               else
               {
                    displayHelp(); 
               }
               break;
        default :
               displayHelp();
               break;

    }
   
    return 0;
}
