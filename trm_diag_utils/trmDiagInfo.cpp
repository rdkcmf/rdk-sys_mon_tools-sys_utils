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




/*C++ include*/
#include <errno.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <string.h>
#include <uuid/uuid.h>
#include <list>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <memory>
#include <limits>
#include <iostream>
#include <map>
#include <string>

/*IARM  include*/
#include "libIBus.h"


/*TRM Manager  include*/
#include "TRMMgr.h"

/* TRM Include*/
#include "trm/Messages.h"
#include "trm/MessageProcessor.h"
#include "trm/Activity.h"
#include "trm/JsonEncoder.h"
#include "trm/JsonDecoder.h"
#include "trm/TRM.h"
#include "trm/Klass.h"
#include "trm/TunerReservation.h"
#include "trm/TunerReservation.h"

using namespace TRM;

#include "trmDiagInfo.h"

#define TRM_DIAG_SUCCESS 0
#define TRM_DIAG_FAILURE 1
#define DISABLE_DEBUG 1

static int getDiagParameter(TRMMessageType_t type);
static void processBuffer( const char* buf, int len);

char *dataVersion = NULL ;
char **connectedDeviceIds = NULL ;
char **tunerReservationList = NULL ;
char **connectionErrorList = NULL ;
int numberOfConnectedDevices = 0 ;
int numberOfReservedTuners = 0 ;
int numberOfConnectionErrors = 0 ;
int numberOfTRMErrors = 0;
int numberOfTuners = 0;
static int maxDeviceIdLen = 25 ;
static int maxTunerTokenLen = 26 ;



static bool iarmBusInitialized = false;

void iarmBusInit( void ) {
    if(iarmBusInitialized == true)
    {
        return;
    }
    FILE fp_old = *stdout;  // preserve the original stdout
    #ifdef DISABLE_DEBUG
    *stdout = *fopen("/dev/null","w");
    #endif
    /* Initialization with IARM and Register RPC for the clients */
    IARM_Bus_Init("getTRMDiagInfo");
    IARM_Bus_Connect();
    *stdout=fp_old;  // restore stdout
    iarmBusInitialized = true;
}

void iarmBusDisconnect(void) {
    FILE fp_old = *stdout;  // preserve the original stdout
    #ifdef DISABLE_DEBUG
    *stdout = *fopen("/dev/null","w");
    #endif
    IARM_Bus_Disconnect();
    IARM_Bus_Term();
    *stdout=fp_old;  // restore stdout
}

static int getDiagParameter(TRMMessageType_t type)
{
    TRMDiagInfo_t infoParam;
    memset(&infoParam, 0, sizeof(infoParam));
    int status = TRM_DIAG_FAILURE ;
    iarmBusInit();
    infoParam.retCode = TRMMgr_ERR_UNKNOWN;
    infoParam.bufLen  = 0;
    infoParam.msgType = type;    
    IARM_Bus_Call(IARM_BUS_TRMMGR_NAME,
                (char *)IARM_BUS_TRMMGR_API_GetTRMDiagInfo,
                (void *)&infoParam,
                sizeof(infoParam));
    if (infoParam.retCode == TRMMgr_ERR_NONE)
    {
        if(infoParam.bufLen)
          processBuffer(&(infoParam.buf[0]),infoParam.bufLen);

        numberOfTRMErrors = infoParam.numOfTRMError;
        numberOfTuners = infoParam.numOfTuner;
        status = TRM_DIAG_SUCCESS ;
    } else {
        status = TRM_DIAG_FAILURE ;
    }
    return status ;
}


/**
 * @brief This function is used to Deocode the parse messafe from TRM Srv
 */
class MsgProcessor : public TRM::MessageProcessor
{

public :
    void operator() (const TRM::GetVersionResponse &msg );
    void operator() (const TRM::GetAllReservationsResponse &msg);
    void operator() (const TRM::GetAllConnectedDeviceIdsResponse &msg);
    void operator() (const TRM::GetTRMConnectionEvents &msg);
};


/**
 * @brief Version Response Message
 */
void MsgProcessor::operator() (const TRM::GetVersionResponse &msg )
{
  
    int statusCode = 0;
    int dataLength = 0 ;
    statusCode = msg.getStatus().getStatusCode();
    if (TRM::ResponseStatus::kOk == statusCode)
    {
        dataLength = msg.getVersion().length() + 1;
        dataVersion = (char*) malloc(dataLength);
        if(NULL != dataVersion) {
            strncpy(dataVersion, msg.getVersion().c_str(), dataLength);
        }
    }
    else
    {
        printf(" StatusCode = %d \r\n",statusCode);
    }
}

/**
 * @brief TRM GetAllReservationsResponse Message 
 */
void MsgProcessor::operator() (const TRM::GetAllReservationsResponse &msg)
{
    int statusCode = msg.getStatus().getStatusCode();
    if (TRM::ResponseStatus::kOk == statusCode)
    {
        const std::map<std::string, std::list<TunerReservation> > & allReservations = msg.getAllReservations();
        std::map<std::string, std::list<TunerReservation> >::const_iterator it;
        int tuner = 0;
        int tunerTokenLen = 0 ;
        numberOfReservedTuners = allReservations.size();
        if ( 0 != numberOfReservedTuners ) {
            tunerReservationList = (char**) malloc(numberOfReservedTuners * maxTunerTokenLen * sizeof(char));
        }
        for (it = allReservations.begin(); it != allReservations.end(); it++)
        {
            const std::list<TunerReservation> & tunerReservations = it->second;
            std::list<TunerReservation>::const_iterator it1;
            
            for (it1 = tunerReservations.begin(); it1 != tunerReservations.end(); it1++) 
            {
               tunerTokenLen = (*it1).getReservationToken().length() + 1 ;
               tunerReservationList[tuner] = (char*) malloc( tunerTokenLen * sizeof(char));
               strncpy(tunerReservationList[tuner], (*it1).getReservationToken().c_str(), tunerTokenLen);
               tuner++;          
               
            }
        }
    }
    else
    {
        printf("StatusCode = %d \r\n",statusCode);
    }
    
}


/**
 * @brief Version Response Message
 */
void MsgProcessor::operator() (const TRM::GetAllConnectedDeviceIdsResponse &msg)
{
   
    int statusCode = msg.getStatus().getStatusCode();
    int size = 0;
    int count = 0 ;
    if (TRM::ResponseStatus::kOk == statusCode)
    {
        const std::list<std::string> & DeviceIds = msg.getDeviceIds();
        size = DeviceIds.size();
        numberOfConnectedDevices = size ; 
        connectedDeviceIds = (char**) malloc( size * maxDeviceIdLen * sizeof(char) );
        std::list<std::string>::const_iterator it;
        
        for (it = DeviceIds.begin(); it != DeviceIds.end(); it++) {
           int idLen = (*it).length() + 1;
           if( NULL != connectedDeviceIds ) {
              connectedDeviceIds[count] = (char*) malloc( idLen * sizeof(char));
              strncpy(connectedDeviceIds[count],(*it).c_str(),idLen);
           }
           count++;
        }
    }
    else
    {
       printf(" StatusCode = %d \r\n",statusCode);
    }
    
}

void MsgProcessor::operator() (const TRM::GetTRMConnectionEvents &msg)
{
    /* TRM diag still seems to be not complete */
    /* Maintaining the multidimentional array with expectation of availability of list of errors if present */
    int statusCode = msg.getStatus().getStatusCode();
    char mockMessage[] = "Connection Error ";
    
    connectionErrorList = (char**) malloc(  maxDeviceIdLen * sizeof(char) );
    if (TRM::ResponseStatus::kOk == statusCode)
    {
        printf("(GetTRMConnectionEvents) Sucess = %d\r\n",statusCode);
        numberOfConnectionErrors = 0 ;

    }
    else
    {
        connectionErrorList[0] = (char*) malloc( strlen(mockMessage) * sizeof(char));
        strncpy(connectionErrorList[0], mockMessage, strlen(mockMessage));
        numberOfConnectionErrors = 1 ;
    }
}




/**
 * @brief This function is used to Deocode the parse messafe from TRM Srv
 */
static void processBuffer( const char* buf, int len)
{

    if (buf != NULL)
    {
        std::vector<uint8_t> response;
        response.insert( response.begin(), buf, buf+len);
        MsgProcessor msgProc;
        TRM::JsonDecoder jdecoder( msgProc);
        jdecoder.decode( response);
    }

}

/*
 * Caller function should free the memory
 */
int getTrmConnectedDeviceId( char*** ptrConnectedDeviceList, int* ptrLength ) {
    int status = 0 ;
    status = getDiagParameter( TRMMgr_MSG_TYPE_GET_CONN_DEVICE_IDS );
    *ptrLength = numberOfConnectedDevices ; 
    if (  NULL != connectedDeviceIds && status == 0 ) { 
        *ptrConnectedDeviceList = connectedDeviceIds ;
    }
    return status ;
}

/**
 * Caller function should free the memory
 */
int getTrmReservedTuners( char*** ptrReservedTunerList, int* ptrLength ) {
    int status = 0 ;
    status = getDiagParameter( TRMMgr_MSG_TYPE_GET_TUNER_RESERVATION );
    *ptrLength = numberOfReservedTuners; 
    if (  NULL != tunerReservationList ) { 
        *ptrReservedTunerList = tunerReservationList ;
    }
    return status ;
}


int getNumOfTrmErrors(int* errCount){
    int status = 0 ;
    status = getDiagParameter( TRMMgr_MSG_TYPE_GET_NUM_TRM_ERRORS );
    *errCount = numberOfTRMErrors;
    numberOfTRMErrors = 0; 
   return status ;
}


int getTrmConnectionErrors( char*** ptrConnectionErrorList, int* ptrLength ) {
    int status = 0 ;
    status = getDiagParameter( TRMMgr_MSG_TYPE_GET_CONNECTION_ERRORS );
    *ptrLength = numberOfConnectionErrors; 
    if (  NULL != connectionErrorList ) { 
        *ptrConnectionErrorList = connectionErrorList ;
    }
    return status ;
}


int getNumOfTuners(int* tunerCount){
    int status = 0 ;
    status = getDiagParameter( TRMMgr_MSG_TYPE_GET_NUM_IN_BAND_TUNERS );
    *tunerCount = numberOfTuners;
    numberOfTuners = 0; 
    return status ;
}

