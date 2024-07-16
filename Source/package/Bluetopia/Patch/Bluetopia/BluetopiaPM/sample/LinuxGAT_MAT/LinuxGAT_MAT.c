#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <unistd.h>           /* Include for getpid().                        */

#include "SS1BTPM.h"          /* BTPM Application Programming Interface.      */

#include "LinuxGAT_MAT.h"

//#include "BTVSAPI.h"


typedef struct _tagPrepareWriteEntry_t
{
   GATT_Connection_Type_t          ConnectionType;
   BD_ADDR_t                       RemoteDeviceAddress;
   unsigned int                    ServiceID;
   unsigned int                    AttributeOffset;
   unsigned int                    AttributeValueOffset;
   unsigned int                    MaximumValueLength;
   unsigned int                    ValueLength;
   Byte_t                         *Value;
   struct _tagPrepareWriteEntry_t *NextPrepareWriteEntryPtr;
} PrepareWriteEntry_t;

static PrepareWriteEntry_t *PrepareWriteList; 
static AttributeInfo_t *SearchServiceListByOffset(unsigned int ServiceID, unsigned int AttributeOffset);
static void BTPSAPI DEVM_Event_Callback(DEVM_Event_Data_t *EventData, void *CallbackParameter);
static int RegisterService(unsigned int ServiceIndex);
static void DisplayGATTUUID(GATT_UUID_t *UUID, char *Prefix, unsigned int Level);
static void ProcessWriteRequestEvent(GATM_Write_Request_Event_Data_t *WriteRequestData);
static void DumpData(Boolean_t String, unsigned int Length, Byte_t *Data);
void update_connectable_mode(void);

static Boolean_t           Initialized; 

DEVM_Advertising_Information_t AdvertisingInfo;

static unsigned int        GATMCallbackID;

  static Mutex_t             ServiceMutex;
  
#define EXIT_CODE                                  (-3)

#define FUNCTION_ERROR                             (-4)

#define INDENT_LENGTH                                 3
  
static unsigned int        AuthenticationCallbackID;

static Boolean_t           CurrentLowEnergy;        /* Variable which holds the        */
                                                    /* current LE state of the device  */
                                                    /* which is currently pairing or   */
                                                    /* authenticating. */

static BD_ADDR_t           CurrentRemoteBD_ADDR;    /* Variable which holds the        */
                                                    /* current BD_ADDR of the device   */
                                                    /* which is currently pairing or   */
                                                    /* authenticating.
                                                     *
                                                     */
static GAP_IO_Capability_t IOCapability;            /* Variable which holds the        */
                                                    /* current I/O Capabilities that   */
                                                    /* are to be used for Secure Simple*/
                                                    /* Pairing.
                                                     *
                                                     *                                               */
static Boolean_t           OOBSupport;              /* Variable which flags whether    */
                                                    /* or not Out of Band Secure Simple*/
                                                    /* Pairing exchange is supported.  */

static Boolean_t           MITMProtection;          /* Variable which flags whether or */
                                                    /* not Man in the Middle (MITM)    */
                                                    /* protection is to be requested   */
                                                    /* during a Secure Simple Pairing  */
                                                    /* procedure.                                                     */


     /* The following function is the Device Manager Event Callback       */
   /* function that is Registered with the Device Manager.  This        */
   /* callback is responsible for processing all Device Manager Events. */
   
char ReadData[256] = {0};
unsigned int bFirstTime = 0;
unsigned int ReadDataLength = 0;

static void BTPSAPI DEVM_Event_Callback(DEVM_Event_Data_t *EventData, void *CallbackParameter)
{
   char Buffer[32];
   int i = 0;

   if(EventData)
   {
      printf("\r\n");

      switch(EventData->EventType)
      {
         case detDevicePoweredOn:
            printf("Device Powered On.\r\n");
            break;
         case detDevicePoweringOff:
            printf("Device Powering Off Event, Timeout: 0x%08X.\r\n", EventData->EventData.DevicePoweringOffEventData.PoweringOffTimeout);
            break;
         case detDevicePoweredOff:
            printf("Device Powered Off.\r\n");
            break;
         case detLocalDevicePropertiesChanged:
         {
            printf("Local Device Properties Changed.\r\n");
            if(EventData->EventData.RemoteDeviceConnectionStatusEventData.Status !=1)
            	DEVM_StartAdvertising(&AdvertisingInfo);
            if(EventData->EventData.LocalDevicePropertiesChangedEventData.LocalDeviceProperties.ConnectableMode !=1)
            {
//            	update_connectable_mode();
            	DEVM_StartAdvertising(&AdvertisingInfo);
            }
         }
//             DisplayLocalDeviceProperties(EventData->EventData.LocalDevicePropertiesChangedEventData.ChangedMemberMask, &(EventData->EventData.LocalDevicePropertiesChangedEventData.LocalDeviceProperties));
            break;
         case detDeviceScanStarted:
            printf("LE Device Discovery Started.\r\n");
            break;
         case detDeviceScanStopped:
            printf("LE Device Discovery Stopped.\r\n");
            break;
         case detDeviceAdvertisingStarted:
         {
            printf("LE Advertising Started.\r\n");
            bFirstTime = 0;
         }
         break;
         case detDeviceAdvertisingStopped:
            printf("LE Advertising Stopped.\r\n");
            break;
         case detAdvertisingTimeout:
         {
        	 printf("LE Advertising Timeout.\r\n");
        	 DEVM_StartAdvertising(&AdvertisingInfo);
         }
            break;
         case detDeviceDiscoveryStarted:
            printf("Device Discovery Started.\r\n");
            break;
         case detDeviceDiscoveryStopped:
            printf("Device Discovery Stopped.\r\n");
            break;
         case detRemoteDeviceFound:
            printf("Remote Device Found.\r\n");

//             DisplayRemoteDeviceProperties(0, &(EventData->EventData.RemoteDeviceFoundEventData.RemoteDeviceProperties));
            break;
         case detRemoteDeviceDeleted:
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceDeletedEventData.RemoteDeviceAddress, Buffer);

            printf("Remote Device Deleted: %s.\r\n", Buffer);
            break;
         case detRemoteDevicePropertiesChanged:
            printf("Remote Device Properties Changed.\r\n");

//             DisplayRemoteDeviceProperties(EventData->EventData.RemoteDevicePropertiesChangedEventData.ChangedMemberMask, &(EventData->EventData.RemoteDevicePropertiesChangedEventData.RemoteDeviceProperties));
            break;
         case detRemoteDeviceAddressChanged:
            printf("Remote Device Address Changed.\r\n");

//             BD_ADDRToStr(EventData->EventData.RemoteDeviceAddressChangeEventData.RemoteDeviceAddress, Buffer);
            printf("Remote Device Address:          %s.\r\n", Buffer);
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceAddressChangeEventData.PreviousRemoteDeviceAddress, Buffer);
            printf("Previous Remote Device Address: %s.\r\n", Buffer);
            break;
         case detRemoteDevicePropertiesStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDevicePropertiesStatusEventData.RemoteDeviceProperties.BD_ADDR, Buffer);

            printf("Remote Device Properties Status: %s, %s.\r\n", Buffer, EventData->EventData.RemoteDevicePropertiesStatusEventData.Success?"SUCCESS":"FAILURE");

//             DisplayRemoteDeviceProperties(0, &(EventData->EventData.RemoteDevicePropertiesStatusEventData.RemoteDeviceProperties));
            break;
         case detRemoteDeviceServicesStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceServicesStatusEventData.RemoteDeviceAddress, Buffer);

            printf("Remote Device %s Services Status: %s, %s.\r\n", Buffer, (EventData->EventData.RemoteDeviceServicesStatusEventData.StatusFlags & DEVM_REMOTE_DEVICE_SERVICES_STATUS_FLAGS_LOW_ENERGY)?"LE":"BR/EDR", (EventData->EventData.RemoteDeviceServicesStatusEventData.StatusFlags & DEVM_REMOTE_DEVICE_SERVICES_STATUS_FLAGS_SUCCESS)?"SUCCESS":"FAILURE");
            break;
         case detRemoteDevicePairingStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDevicePairingStatusEventData.RemoteDeviceAddress, Buffer);

            printf("%s Remote Device Pairing Status: %s, %s (0x%02X)\r\n", ((EventData->EventData.RemoteDevicePairingStatusEventData.AuthenticationStatus & DEVM_REMOTE_DEVICE_PAIRING_STATUS_FLAGS_LOW_ENERGY)?"LE":"BR/EDR"), Buffer, (EventData->EventData.RemoteDevicePairingStatusEventData.Success)?"SUCCESS":"FAILURE", EventData->EventData.RemoteDevicePairingStatusEventData.AuthenticationStatus);
            break;
         case detRemoteDeviceAuthenticationStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceAuthenticationStatusEventData.RemoteDeviceAddress, Buffer);

            printf("Remote Device Authentication Status: %s, %d (%s)\r\n", Buffer, EventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status, (EventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status)?ERR_ConvertErrorCodeToString(EventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status):"SUCCESS");
            break;
         case detRemoteDeviceEncryptionStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceEncryptionStatusEventData.RemoteDeviceAddress, Buffer);

            printf("Remote Device Encryption Status: %s, %d (%s)\r\n", Buffer, EventData->EventData.RemoteDeviceEncryptionStatusEventData.Status, (EventData->EventData.RemoteDeviceEncryptionStatusEventData.Status)?ERR_ConvertErrorCodeToString(EventData->EventData.RemoteDeviceEncryptionStatusEventData.Status):"SUCCESS");
            break;
         case detRemoteDeviceConnectionStatus:
//             BD_ADDRToStr(EventData->EventData.RemoteDeviceConnectionStatusEventData.RemoteDeviceAddress, Buffer);

            printf("Remote Device Connection Status: %s, %d (%s)\r\n", Buffer, EventData->EventData.RemoteDeviceConnectionStatusEventData.Status, (EventData->EventData.RemoteDeviceConnectionStatusEventData.Status)?ERR_ConvertErrorCodeToString(EventData->EventData.RemoteDeviceConnectionStatusEventData.Status):"SUCCESS");
            break;
         default:
            printf("Unknown Device Manager Event Received: 0x%08X, Length: 0x%08X.\r\n", (unsigned int)EventData->EventType, EventData->EventLength);
            break;
      }
   }
   else
      printf("\r\nDEVM Event Data is NULL.\r\n");

   /* Make sure the output is displayed to the user.                    */
   fflush(stdout);
}

   /* The following function is used to search the Service Info list to */
   /* return an attribute based on the ServiceID and the                */
   /* AttributeOffset.  This function returns a pointer to the attribute*/
   /* or NULL if not found. */
static AttributeInfo_t *SearchServiceListByOffset(unsigned int ServiceID, unsigned int AttributeOffset)
{
   unsigned int     Index;
   unsigned int     Index1;
   AttributeInfo_t *AttributeInfo = NULL;

   /* Verify that the input parameters are semi-valid.                  */
   if((ServiceID) && (AttributeOffset))
   {
      /* Loop through the services and find the correct attribute.      */
      for(Index=0;(AttributeInfo==NULL)&&(Index<NUMBER_OF_SERVICES);Index++)
      {
         if(ServiceTable[Index].ServiceID == ServiceID)
         {
            /* Loop through the attribute list for this service and     */
            /* locate the correct attribute based on the Attribute      */
            /* Offset.                                                  */
            for(Index1=0;(AttributeInfo==NULL)&&(Index1<ServiceTable[Index].NumberAttributes);Index1++)
            {
               if(ServiceTable[Index].AttributeList[Index1].AttributeOffset == AttributeOffset)
                  AttributeInfo = &(ServiceTable[Index].AttributeList[Index1]);
            }
         }
      }
   }

   return(AttributeInfo);
}

/* The following function is a utility function which is used to     */
/* process a GATM Write Request Event.                               */
static void ProcessWriteRequestEvent(GATM_Write_Request_Event_Data_t *WriteRequestData)
{
int               Result;
Byte_t          **Value;
Byte_t            ErrorCode;
Boolean_t        *AllocatedMemory;
unsigned int     *ValueLength;
unsigned int      MaximumValueLength;
AttributeInfo_t  *AttributeInfo;

/* Verify that the input parameter is semi-valid.                    */
if(WriteRequestData)
{
   /* Initialize the error code.                                     */
   ErrorCode = 0;

   /* Wait on access to the Service Info List Mutex.                 */
   if(BTPS_WaitMutex(ServiceMutex, BTPS_INFINITE_WAIT))
   {
      /* Search for the Attribute for this write request (which must */
      /* be a characteristic or descriptor).                         */
      AttributeInfo = SearchServiceListByOffset(WriteRequestData->ServiceID, WriteRequestData->AttributeOffset);
      if((AttributeInfo) && ((AttributeInfo->AttributeType == atCharacteristic) || (AttributeInfo->AttributeType == atDescriptor)))
      {
         /* Handle the read based on the type of attribute.          */
         switch(AttributeInfo->AttributeType)
         {
            case atCharacteristic:
               MaximumValueLength = ((CharacteristicInfo_t *)AttributeInfo->Attribute)->MaximumValueLength;
               AllocatedMemory    = &(((CharacteristicInfo_t *)AttributeInfo->Attribute)->AllocatedValue);
               ValueLength        = &(((CharacteristicInfo_t *)AttributeInfo->Attribute)->ValueLength);
               Value              = &(((CharacteristicInfo_t *)AttributeInfo->Attribute)->Value);
               break;
            case atDescriptor:
               MaximumValueLength = ((DescriptorInfo_t *)AttributeInfo->Attribute)->MaximumValueLength;
               AllocatedMemory    = &(((DescriptorInfo_t *)AttributeInfo->Attribute)->AllocatedValue);
               ValueLength        = &(((DescriptorInfo_t *)AttributeInfo->Attribute)->ValueLength);
               Value              = &(((DescriptorInfo_t *)AttributeInfo->Attribute)->Value);
               break;
            default:
               /* Do nothing.                                        */
               break;
         }

         /* Verify that the length of the write is less than the     */
         /* maximum length of this characteristic.                   */
         if(WriteRequestData->DataLength <= MaximumValueLength)
         {
            /* Free any previously existing buffer.                  */
            if((*AllocatedMemory) && (*Value))
            {
               BTPS_FreeMemory(*Value);

               *AllocatedMemory = FALSE;
               *ValueLength     = 0;
               *Value           = NULL;
            }

            /* Allocate a buffer to hold this new value (note we will*/
            /* always allocate a worst case buffer for this attribute*/
            /* to make handling prepare writes later easier).        */
            if((WriteRequestData->DataLength == 0) || ((*Value = (Byte_t *)BTPS_AllocateMemory(MaximumValueLength)) != NULL))
            {
               /* Store the information on the write.                */
               if(WriteRequestData->DataLength)
               {
                  /* Copy the value over into the allocated buffer.  */
                  BTPS_MemCopy(*Value, WriteRequestData->Data, WriteRequestData->DataLength);

                  /* Save the Value Length and flag that a buffer is */
                  /* allocated for this attribute.                   */
                  *AllocatedMemory = TRUE;
                  *ValueLength     = WriteRequestData->DataLength;
               }
               else
               {
                  /* Simply reset the state since this is a 0 byte   */
                  /* write.                                          */
                  *AllocatedMemory = FALSE;
                  *ValueLength     = 0;
                  *Value           = NULL;
               }

               /* If we need to respond to this request go ahead and */
               /* do so.                                             */
               if(WriteRequestData->RequestID)
               {
                  if((Result = GATM_WriteResponse(WriteRequestData->RequestID)) == 0)
                     printf("GATM_WriteResponse() success.\n");
                  else
                     printf("Error - GATM_WriteResponse() %d, %s\n", Result, ERR_ConvertErrorCodeToString(Result));
               }
            }
            else
               ErrorCode = ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_RESOURCES;
         }
         else
            ErrorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH;
      }
      else
         ErrorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE;

      /* Release the mutex that was previously acquired.             */
      BTPS_ReleaseMutex(ServiceMutex);
   }
   else
   {
      /* Simply send an error response.                              */
      ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
   }

   /* If requested go ahead and issue an error response to the       */
   /* request (if a response is required for this request).          */
   if((ErrorCode) && (WriteRequestData->RequestID))
   {
      if((Result = GATM_ErrorResponse(WriteRequestData->RequestID, ErrorCode)) == 0)
         printf("GATM_ErrorResponse() success.\n");
      else
         printf("Error - GATM_ErrorResponse() %d, %s\n", Result, ERR_ConvertErrorCodeToString(Result));
   }
}
}

   /* The following function is a utility function which is used to     */
   /* process a GATM Read Request Event.                                */
static void ProcessReadRequestEvent(GATM_Read_Request_Event_Data_t *ReadRequestData)
{
   int              Result;
   Byte_t          *Value;
   Byte_t           ErrorCode;
   unsigned int     ValueLength;
   AttributeInfo_t *AttributeInfo;

   /* Verify that the input parameter is semi-valid.                    */
   if(ReadRequestData)
   {
      /* Initialize the error code.                                     */
      ErrorCode = 0;

      /* Wait on access to the Service Info List Mutex.                 */
     if(BTPS_WaitMutex(ServiceMutex, BTPS_INFINITE_WAIT))
      {
         //* Search for the Attribute for this read request (which must  */
         /* be a characteristic or descriptor).                         */
         AttributeInfo = SearchServiceListByOffset(ReadRequestData->ServiceID, ReadRequestData->AttributeOffset);
         if((AttributeInfo) && ((AttributeInfo->AttributeType == atCharacteristic) || (AttributeInfo->AttributeType == atDescriptor)) && (AttributeInfo->Attribute))
         {
            /* Handle the read based on the type of attribute.          */
            switch(AttributeInfo->AttributeType)
            {
               case atCharacteristic:
                  ValueLength = ((CharacteristicInfo_t *)AttributeInfo->Attribute)->ValueLength;
                  Value       = ((CharacteristicInfo_t *)AttributeInfo->Attribute)->Value;
                  break;
               case atDescriptor:
                  ValueLength = ((DescriptorInfo_t *)AttributeInfo->Attribute)->ValueLength;
                  Value       = ((DescriptorInfo_t *)AttributeInfo->Attribute)->Value;
                  break;
               default:
                  /* Do nothing.                                        */
                  break;
            }

            /* Next check the requested offset of the read versus the   */
            /* length.                                                  */
            if(ReadRequestData->AttributeValueOffset <= ValueLength)
            {
               /* Verify that the length and pointer is valid.          */
               if((ValueLength) && (Value))
               {
                  /* Calculate the length of the value from the         */
                  /* specified offset.                                  */
                  ValueLength -= ReadRequestData->AttributeValueOffset;

                  /* Get a pointer to the value at the specified offset.*/
                  if(ValueLength)
                     Value     = &(Value[ReadRequestData->AttributeValueOffset]);
                  else
                     Value     = NULL;
               }
               else
                  ValueLength = 0;

               /* Go ahead and respond to the read request.             */
               if((Result = GATM_ReadResponse(ReadRequestData->RequestID, ValueLength, Value)) == 0)
                  printf("GATM_ReadResponse() success.\n");
               else
                  printf("Error - GATM_ReadResponse() %d, %s\n", Result, ERR_ConvertErrorCodeToString(Result));
            }
            else
               ErrorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET;
         }
         else
            ErrorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE;

         /* Release the mutex that was previously acquired.             */
         BTPS_ReleaseMutex(ServiceMutex);
      }
      else
      {
         /* Simply send an error response.                              */
         ErrorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
      }

      /* If requested go ahead and issue an error response to the       */
      /* request.                                                       */
      if(ErrorCode)
      {
         if((Result = GATM_ErrorResponse(ReadRequestData->RequestID, ErrorCode)) == 0)
            printf("GATM_ErrorResponse() success.\n");
         else
            printf("Error - GATM_ErrorResponse() %d, %s\n", Result, ERR_ConvertErrorCodeToString(Result));
      }
   }
}
// 
   /* The following function is the GATM Event Callback function that is*/
   /* Registered with the Generic Attribute Profile Manager.  This      */
   /* callback is responsible for processing all GATM Events.           */
static void BTPSAPI GATM_Event_Callback(GATM_Event_Data_t *EventData, void *CallbackParameter)
{
   char Buffer[128];
   int i = 0;

   if(EventData)
   {
      switch(EventData->EventType)
      {
         /* GATM Connection Events.                                     */
         case getGATTConnected:
            printf("\r\nGATT Connection Event\r\n");

//             BD_ADDRToStr(EventData->EventData.ConnectedEventData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type: %s\r\n", (EventData->EventData.ConnectedEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:  %s\r\n", Buffer);
//             printf("    MTU:             %u\r\n", EventData->EventData.ConnectedEventData.MTU);

//            if(EventData->EventData.RemoteDeviceEncryptionStatusEventData.Status)
//            DEVM_EncryptRemoteDevice(EventData->EventData.ConnectedEventData.RemoteDeviceAddress, 0x80000000);

            break;
         case getGATTDisconnected:
//             printf("\r\nGATT Disconnect Event\r\n");

//             BD_ADDRToStr(EventData->EventData.DisconnectedEventData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type: %s\r\n", (EventData->EventData.DisconnectedEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:  %s\r\n", Buffer);

//            DEVM_UnPairRemoteDevice(EventData->EventData.DisconnectedEventData.RemoteDeviceAddress, 0x80000000);
            break;
         case getGATTConnectionMTUUpdate:
//             printf("\r\nGATT Connection MTU Update Event\r\n");

//             BD_ADDRToStr(EventData->EventData.ConnectionMTUUpdateEventData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type: %s\r\n", (EventData->EventData.ConnectionMTUUpdateEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:  %s\r\n", Buffer);
//             printf("    New MTU:         %u\r\n", EventData->EventData.ConnectionMTUUpdateEventData.MTU);
            break;
         case getGATTHandleValueData:
            printf("\r\nGATT Handle Value Data Event\r\n");

//             BD_ADDRToStr(EventData->EventData.HandleValueDataEventData.RemoteDeviceAddress, Buffer);

            printf("    Connection Type:  %s\r\n", (EventData->EventData.HandleValueDataEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
            printf("    Remote Address:   %s\r\n", Buffer);
            printf("    Type:             %s\r\n", (EventData->EventData.HandleValueDataEventData.HandleValueIndication?"Indication":"Notification"));
            printf("    Attribute Handle: 0x%04X (%u)\r\n", EventData->EventData.HandleValueDataEventData.AttributeHandle, EventData->EventData.HandleValueDataEventData.AttributeHandle);
            printf("    Value Length:     %u\r\n", EventData->EventData.HandleValueDataEventData.AttributeValueLength);
            printf("    Value:            \r\n");
//             DumpData(FALSE, EventData->EventData.HandleValueDataEventData.AttributeValueLength, EventData->EventData.HandleValueDataEventData.AttributeValue);
            break;

         /* GATM Server Events.                                         */
         case getGATTWriteRequest:
        	 if(bFirstTime == 0)
        	 {
        		 for (i = 0; i < 256; i++)
        			 ReadData[i] = NULL;
        		 bFirstTime = 1;
        	 }
//             printf("\r\nGATT Write Request Event\r\n");s

//             BD_ADDRToStr(EventData->EventData.WriteRequestData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type:  %s\r\n", (EventData->EventData.WriteRequestData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:   %s\r\n", Buffer);
//             printf("    Service ID:       %u\r\n", EventData->EventData.WriteRequestData.ServiceID);
//             printf("    Request ID:       %u\r\n", EventData->EventData.WriteRequestData.RequestID);
//             printf("    Attribute Offset: %u\r\n", EventData->EventData.WriteRequestData.AttributeOffset);
//             printf("    Data Length:      %u\r\n", EventData->EventData.WriteRequestData.DataLength);
//             printf("    Value:            \r\n");

             DumpData(TRUE, EventData->EventData.WriteRequestData.DataLength, EventData->EventData.WriteRequestData.Data);
// 
            Srv3Attr2.ValueLength = ReadDataLength;
            Srv3Attr2.Value = &ReadData;
//             printf("%s", Srv3Attr2.Value);
//             printf("%d", Srv3Attr2.ValueLength);
// 
//             printf("\r\n");

            ProcessWriteRequestEvent(&(EventData->EventData.WriteRequestData));


//            DEVM_DeleteRemoteDevice(EventData->EventData.WriteRequestData.RemoteDeviceAddress);

//            printf("1 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR0);
//            printf("2 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR1);
//            printf("3 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR2);
//            printf("4 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR3);
//            printf("5 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR4);
//            printf("6 %d\r\n",EventData->EventData.WriteRequestData.RemoteDeviceAddress.BD_ADDR5);
//
//            DEVM_DisconnectRemoteDevice(EventData->EventData.ConnectedEventData.RemoteDeviceAddress, DEVM_DISCONNECT_FROM_REMOTE_DEVICE_FLAGS_LOW_ENERGY );


            break;
         case getGATTSignedWrite:
            printf("\r\nGATT Signed Write Event\r\n");

//             BD_ADDRToStr(EventData->EventData.SignedWriteData.RemoteDeviceAddress, Buffer);

            printf("    Connection Type:  %s\r\n", (EventData->EventData.SignedWriteData.ConnectionType == gctLE)?"LE":"BR/EDR");
            printf("    Remote Address:   %s\r\n", Buffer);
            printf("    Service ID:       %u\r\n", EventData->EventData.SignedWriteData.ServiceID);
            printf("    Signature:        %s\r\n", (EventData->EventData.SignedWriteData.ValidSignature?"VALID":"INVALID"));
            printf("    Attribute Offset: %u\r\n", EventData->EventData.SignedWriteData.AttributeOffset);
            printf("    Data Length:      %u\r\n", EventData->EventData.SignedWriteData.DataLength);
            printf("    Value:            \r\n");
//             DumpData(FALSE, EventData->EventData.SignedWriteData.DataLength, EventData->EventData.SignedWriteData.Data);
            printf("\r\n");

            /* If the signature is valid go ahead and process the signed*/
            /* write command.                                           */
//             if(EventData->EventData.SignedWriteData.ValidSignature)
//                ProcessSignedWriteEvent(&(EventData->EventData.SignedWriteData));
//             break;
         case getGATTReadRequest:
            printf("\r\nGATT Read Request Event\r\n");

//             BD_ADDRToStr(EventData->EventData.ReadRequestData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type:        %s\r\n", (EventData->EventData.ReadRequestData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:         %s\r\n", Buffer);
//             printf("    Service ID:             %u\r\n", EventData->EventData.ReadRequestData.ServiceID);
//             printf("    Request ID:             %u\r\n", EventData->EventData.ReadRequestData.RequestID);
//             printf("    Attribute Offset:       %u\r\n", EventData->EventData.ReadRequestData.AttributeOffset);
//             printf("    Attribute Value Offset: %u\r\n", EventData->EventData.ReadRequestData.AttributeValueOffset);

            /* Go ahead and process the Read Request.                   */
            ProcessReadRequestEvent(&(EventData->EventData.ReadRequestData));

            break;
         case getGATTPrepareWriteRequest:
            printf("\r\nGATT Prepare Write Request Event\r\n");

//             BD_ADDRToStr(EventData->EventData.PrepareWriteRequestEventData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type:        %s\r\n", (EventData->EventData.PrepareWriteRequestEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:         %s\r\n", Buffer);
//             printf("    Service ID:             %u\r\n", EventData->EventData.PrepareWriteRequestEventData.ServiceID);
//             printf("    Request ID:             %u\r\n", EventData->EventData.PrepareWriteRequestEventData.RequestID);
//             printf("    Attribute Offset:       %u\r\n", EventData->EventData.PrepareWriteRequestEventData.AttributeOffset);
//             printf("    Attribute Value Offset: %u\r\n", EventData->EventData.PrepareWriteRequestEventData.AttributeValueOffset);
//             printf("    Data Length:            %u\r\n", EventData->EventData.PrepareWriteRequestEventData.DataLength);
//             printf("    Value:                  \r\n");
//             DumpData(FALSE, EventData->EventData.PrepareWriteRequestEventData.DataLength, EventData->EventData.PrepareWriteRequestEventData.Data);
            printf("\r\n");

            /* Go ahead and process the Prepare Write Request.          */
//             ProcessPrepareWriteRequestEvent(&(EventData->EventData.PrepareWriteRequestEventData));
            break;
         case getGATTCommitPrepareWrite:
            printf("\r\nGATT Commit Prepare Write Event\r\n");

//             BD_ADDRToStr(EventData->EventData.CommitPrepareWriteEventData.RemoteDeviceAddress, Buffer);

//             printf("    Connection Type:        %s\r\n", (EventData->EventData.CommitPrepareWriteEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
//             printf("    Remote Address:         %s\r\n", Buffer);
//             printf("    Service ID:             %u\r\n", EventData->EventData.CommitPrepareWriteEventData.ServiceID);
//             printf("    Commit Writes?:         %s\r\n", (EventData->EventData.CommitPrepareWriteEventData.CommitWrites?"YES":"NO"));

            /* Go ahead and process the Execute Write Request.          */
//             ProcessCommitPrepareWriteEvent(&(EventData->EventData.CommitPrepareWriteEventData));
            break;
         case getGATTHandleValueConfirmation:
            printf("\r\nGATT Handle-Value Confirmation Event\r\n");

//             BD_ADDRToStr(EventData->EventData.HandleValueConfirmationEventData.RemoteDeviceAddress, Buffer);

            printf("    Connection Type:  %s\r\n", (EventData->EventData.HandleValueConfirmationEventData.ConnectionType == gctLE)?"LE":"BR/EDR");
            printf("    Remote Address:   %s\r\n", Buffer);
            printf("    Service ID:       %u\r\n", EventData->EventData.HandleValueConfirmationEventData.ServiceID);
            printf("    Request ID:       %u\r\n", EventData->EventData.HandleValueConfirmationEventData.TransactionID);
            printf("    Attribute Offset: %u\r\n", EventData->EventData.HandleValueConfirmationEventData.AttributeOffset);
            printf("    Status:           %u\r\n", EventData->EventData.HandleValueConfirmationEventData.Status);
            break;
         default:
            printf("\r\nUnhandled event.\r\n");
            break;
      }

      puts("\r\n");
   }
   else
      printf("\r\nGATM Event Data is NULL.\r\n");

//    printf("GATM>");

   /* Make sure the output is displayed to the user.                    */
   fflush(stdout);
}

static void DumpData(Boolean_t String, unsigned int Length, Byte_t *Data)
{
   int  i, offset;
   char Buf[256], temp[256] = {0};
   int j = Length;

   if((Length) && (Data))
   {
      /* Initialize the temporary buffer.                               */
      BTPS_MemInitialize(Buf, 0, (sizeof(Buf)/sizeof(char)));

      offset = 0;
      i      = 0;
      while(Length-i)
      {
         if(!String)
            offset += sprintf(&Buf[offset], "%02X ", Data[i]);
         else
            offset += sprintf(&Buf[offset], "%c", (char)Data[i]);

         if(!(++i % 16))
         {
            offset = 0;
//             printf("%s", Buf);
         }
      }

//       if(i % 16)
//          printf("%s", Buf);
   }

//    printf("Before:%s\n", ReadData);
   strncat(ReadData, (unsigned char*)Data, j);
//    printf("After:%s\n", ReadData);
   ReadDataLength = strlen(ReadData);
//    printf("ReadDataLength:%d\n", ReadDataLength);
}

   /* The following function is a utility function that is used to      */
   /* dispay a GATT UUID.                                               */
static void DisplayGATTUUID(GATT_UUID_t *UUID, char *Prefix, unsigned int Level)
{
   if((UUID) && (Prefix))
   {
      if(UUID->UUID_Type == guUUID_16)
         printf("%*s %s: %02X%02X\r\n", (Level*INDENT_LENGTH), "", Prefix, UUID->UUID.UUID_16.UUID_Byte1, UUID->UUID.UUID_16.UUID_Byte0);
      else
      {
         if(UUID->UUID_Type == guUUID_128)
         {
            printf("%*s %s: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n", (Level*INDENT_LENGTH), "", Prefix, UUID->UUID.UUID_128.UUID_Byte15, UUID->UUID.UUID_128.UUID_Byte14, UUID->UUID.UUID_128.UUID_Byte13,
                                                                                               UUID->UUID.UUID_128.UUID_Byte12, UUID->UUID.UUID_128.UUID_Byte11, UUID->UUID.UUID_128.UUID_Byte10,
                                                                                               UUID->UUID.UUID_128.UUID_Byte9,  UUID->UUID.UUID_128.UUID_Byte8,  UUID->UUID.UUID_128.UUID_Byte7,
                                                                                               UUID->UUID.UUID_128.UUID_Byte6,  UUID->UUID.UUID_128.UUID_Byte5,  UUID->UUID.UUID_128.UUID_Byte4,
                                                                                               UUID->UUID.UUID_128.UUID_Byte3,  UUID->UUID.UUID_128.UUID_Byte2,  UUID->UUID.UUID_128.UUID_Byte1,
                                                                                               UUID->UUID.UUID_128.UUID_Byte0);
         }
      }
   }
}

   /* The following function is a utility function which is used to     */
   /* register the specified service.                                   */
static int RegisterService(unsigned int ServiceIndex)
{
   int                            ret_val;
   int                            Result;
   Boolean_t                      PrimaryService;
   GATT_UUID_t                    UUID;
   unsigned int                   Index;
   unsigned int                   NumberOfAttributes;
   DescriptorInfo_t              *DescriptorInfo;
   CharacteristicInfo_t          *CharacteristicInfo;
   GATT_Attribute_Handle_Group_t  ServiceHandleRangeResult;
   GATT_Attribute_Handle_Group_t  ServiceHandleRange;

    	ServiceIndex = 0;

    	ServiceTable[ServiceIndex].ServiceID = 0;
   /* Verify that the input parameter is semi-valid.                    */
      /* Verify that a GATM Event Callback is registered.               */
//       if(GATMCallbackID)
//       {
         /* Verify that the service is not already registered.          */
         if(ServiceTable[ServiceIndex].ServiceID == 0)
         {
            /* Calculate the Number of Attributes needed for this       */
            /* service.                                                 */
            NumberOfAttributes = 4;
//             CalculateNumberAttributes(&(ServiceTable));

            /* Check to see if we need to register a persistent UID.    */
            if((ServiceTable[ServiceIndex].Flags & SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID) && (ServiceTable[ServiceIndex].PersistentUID == 0))
            {
               /* Attempt to register a persistent UID.                 */
                ret_val = GATM_RegisterPersistentUID(NumberOfAttributes, &(ServiceTable[ServiceIndex].PersistentUID), &ServiceHandleRangeResult);
               if(!ret_val)
               {
                  printf("Registered Persistent UID: 0x%08X.\r\n", (unsigned int)ServiceTable[ServiceIndex].PersistentUID);
                  printf("Start Handle:              0x%04X.\r\n", ServiceHandleRangeResult.Starting_Handle);
                  printf("End Handle:                0x%04X.\r\n", ServiceHandleRangeResult.Ending_Handle);
               }
               else
               {
                  printf("Error - GATM_RegisterPersistentUID() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                  ret_val = FUNCTION_ERROR;
               }
            }
            else
               ret_val = 0;

            /* Continue only if no error has occurred.                  */
            if(!ret_val)
            {
               /* Determine if this is a primary service.               */
               PrimaryService     = (Boolean_t)(!(ServiceTable[ServiceIndex].Flags & SERVICE_TABLE_FLAGS_SECONDARY_SERVICE));

               /* Configure the Service UUID.  Not this sample          */
               /* application only supports registering 128 bit UUIDs,  */
               /* however GATM allows other types as well.              */
               UUID.UUID_Type     = guUUID_128;
               UUID.UUID.UUID_128 = ServiceTable[ServiceIndex].ServiceUUID;

               DisplayGATTUUID(&UUID, "Registering Service, UUID", 0);

               /* Go ahead and attempt to register the service.         */
               ret_val = GATM_RegisterService(PrimaryService, NumberOfAttributes, &UUID, (ServiceTable[ServiceIndex].PersistentUID?&(ServiceTable[ServiceIndex].PersistentUID):NULL));
               if(ret_val > 0)
               {
                  printf(" Registered Service, Service ID: %u.\r\n", (unsigned int)ret_val);

                  /* Save the Service ID.                               */
                  ServiceTable[ServiceIndex].ServiceID = (unsigned int)ret_val;

                  /* Loop through all of the attributes and register    */
                  /* them.                                              */
                  for(Index=0,ret_val=0;(!ret_val) && (Index<ServiceTable[ServiceIndex].NumberAttributes);Index++)
                  {
                     /* Handle the registration based on the type of    */
                     /* attribute.                                      */
                     switch(ServiceTable[ServiceIndex].AttributeList[Index].AttributeType)
                     {
                        case atInclude:
                           /* We can only register an include in this   */
                           /* application if the previous entry in the  */
                           /* ServiceTable is already registered.  This */
                           /* is a functioning of the application and   */
                           /* not the GATM APIs.                        */
//                            if((ServiceIndex) && (ServiceTable[ServiceIndex-1].ServiceID))
//                            {
                              /* Attempt to an include reference to the */
                              /* previous service in ServiceTable[ServiceIndex].      */
                              ret_val = GATM_AddServiceInclude(0, ServiceTable[ServiceIndex].ServiceID, ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset, ServiceTable[ServiceIndex].ServiceID);
                              if(!ret_val)
                                 printf(" Registered Include to Service ID %u.\r\n", ServiceTable[ServiceIndex].ServiceID);
                              else
                              {
                                 printf("Error - GATM_AddServiceInclude() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                                 ret_val = FUNCTION_ERROR;
                              }
//                            }
                           break;
                        case atCharacteristic:
                           /* Get a pointer to the characteristic       */
                           /* information.                              */
                           if((CharacteristicInfo = (CharacteristicInfo_t *)ServiceTable[ServiceIndex].AttributeList[Index].Attribute) != NULL)
                           {
                              /* Configure the Characteristic UUID.  Not*/
                              /* this sample application only supports  */
                              /* registering 128 bit UUIDs, however GATM*/
                              /* allows other types as well.            */
                              UUID.UUID_Type     = guUUID_128;
                              UUID.UUID.UUID_128 = CharacteristicInfo->CharacteristicUUID;

                              /* Attempt to add this characteristic to  */
                              /* the table.                             */
                              ret_val = GATM_AddServiceCharacteristic(ServiceTable[ServiceIndex].ServiceID, ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset, CharacteristicInfo->CharacteristicPropertiesMask, CharacteristicInfo->SecurityPropertiesMask, &UUID);
                              if(!ret_val)
                                 DisplayGATTUUID(&UUID, "Registered Characteristic, UUID", 0);
                              else
                              {
                                 printf("Error - GATM_AddServiceCharacteristic() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                                 ret_val = FUNCTION_ERROR;
                              }
                           }
                           break;
                        case atDescriptor:
                           /* Get a pointer to the descriptor           */
                           /* information.                              */
                           if((DescriptorInfo = (DescriptorInfo_t *)ServiceTable[ServiceIndex].AttributeList[Index].Attribute) != NULL)
                           {
                              /* Configure the Descriptor UUID.  Not    */
                              /* this sample application only supports  */
                              /* registering 128 bit UUIDs, however GATM*/
                              /* allows other types as well.            */
                              UUID.UUID_Type     = guUUID_128;
                              UUID.UUID.UUID_128 = DescriptorInfo->CharacteristicUUID;

                              /* Attempt to add this descriptor to the  */
                              /* table.                                 */
                              ret_val = GATM_AddServiceDescriptor(ServiceTable[ServiceIndex].ServiceID, ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset, DescriptorInfo->DescriptorPropertiesMask, DescriptorInfo->SecurityPropertiesMask, &UUID);
                              if(!ret_val)
                                 DisplayGATTUUID(&UUID, "Registered Descriptor, UUID", 0);
                              else
                              {
                                 printf("Error - GATM_AddServiceDescriptor() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                                 ret_val = FUNCTION_ERROR;
                              }
                           }
                           break;
                     }
                  }

                  /* If no error occurred go ahead and publish the      */
                  /* service.                                           */
                  if(!ret_val)
                  {
                     /* Attempt to register the Service into the GATT   */
                     /* database.                                       */
                     if((ret_val = GATM_PublishService(ServiceTable[ServiceIndex].ServiceID, GATMCallbackID, (GATM_SERVICE_FLAGS_SUPPORT_LOW_ENERGY | GATM_SERVICE_FLAGS_SUPPORT_CLASSIC_BLUETOOTH), &ServiceHandleRange)) == 0)
                     {
                        /* Store the Published Handle Range for this    */
                        /* service.                                     */
                        ServiceTable[ServiceIndex].ServiceHandleRange = ServiceHandleRange;

                        printf(" Service Registered.\r\n");
                        printf("******************************************************************\r\n");
                        printf("   Service Start Handle: 0x%04X\r\n", ServiceHandleRange.Starting_Handle);
                        printf("   Service End Handle:   0x%04X\r\n\r\n", ServiceHandleRange.Ending_Handle);

                        /* Print the Handles of all of the attributes in*/
                        /* the table.                                   */
                        for(Index=0;Index<ServiceTable[ServiceIndex].NumberAttributes;Index++)
                        {
                           switch(ServiceTable[ServiceIndex].AttributeList[Index].AttributeType)
                           {
                              case atInclude:
                                 printf("Include Attribute @ Handle: 0x%04X\r\n", (ServiceHandleRange.Starting_Handle + ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset));
                                 break;
                              case atCharacteristic:
                                 /* Get a pointer to the characteristic */
                                 /* information.                        */
                                 if((CharacteristicInfo = (CharacteristicInfo_t *)ServiceTable[ServiceIndex].AttributeList[Index].Attribute) != NULL)
                                 {
                                    /* Configure the Characteristic     */
                                    /* UUID.  Not this sample           */
                                    /* application only supports        */
                                    /* registering 128 bit UUIDs,       */
                                    /* however GATM allows other types  */
                                    /* as well.                         */
                                    UUID.UUID_Type     = guUUID_128;
                                    UUID.UUID.UUID_128 = CharacteristicInfo->CharacteristicUUID;
                                    DisplayGATTUUID(&UUID, "Characteristic: ", 0);

                                    printf("Characteristic Declaration @ Handle: 0x%04X\r\n", (ServiceHandleRange.Starting_Handle + ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset));
                                    printf("Characteristic Value       @ Handle: 0x%04X\r\n", (ServiceHandleRange.Starting_Handle + ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset + 1));
                                 }
                                 break;
                              case atDescriptor:
                                 /* Get a pointer to the descriptor     */
                                 /* information.                        */
                                 if((DescriptorInfo = (DescriptorInfo_t *)ServiceTable[ServiceIndex].AttributeList[Index].Attribute) != NULL)
                                 {
                                    /* Configure the Descriptor UUID.   */
                                    /* Not this sample application only */
                                    /* supports registering 128 bit     */
                                    /* UUIDs, however GATM allows other */
                                    /* types as well.                   */
                                    UUID.UUID_Type     = guUUID_128;
                                    UUID.UUID.UUID_128 = DescriptorInfo->CharacteristicUUID;
                                    DisplayGATTUUID(&UUID, "Characteristic Descriptor: ", 0);

                                    printf("Characteristic Descriptor @ Handle: 0x%04X\r\n", (ServiceHandleRange.Starting_Handle + ServiceTable[ServiceIndex].AttributeList[Index].AttributeOffset));
                                 }
                                 break;
                           }
                        }

                        printf("******************************************************************\r\n");
                     }
                     else
                     {
                        printf("Error - GATM_PublishService() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                        ret_val = FUNCTION_ERROR;
                     }
                  }

                  /* Check to see if we need to delete the registered   */
                  /* service if an error occcurred.                     */
                  if(ret_val)
                  {
                     /* If an error occurred go ahead and delete the    */
                     /* service.                                        */
                     Result = GATM_DeleteService(ServiceTable[ServiceIndex].ServiceID);
                     if(Result)
                        printf("Error - GATM_DeleteService() %d, %s\n", Result, ERR_ConvertErrorCodeToString(Result));

                     /* Flag that this service is not registered.       */
                     ServiceTable[ServiceIndex].ServiceID = 0;
                  }
               }
               else
               {
                  printf("Error - GATM_RegisterService() %d, %s\n", ret_val, ERR_ConvertErrorCodeToString(ret_val));

                  ret_val = FUNCTION_ERROR;
               }
            }
         }
         else
         {
            printf("Service already registered.\r\n");

            ret_val = FUNCTION_ERROR;
         }
//       }
//       else
//       {
//          /* Callback not registered, go ahead and notify the user.      */
//          printf("Generic Attribute Profile Manager Event Callback is not registered.\r\n");
// 
//          ret_val = FUNCTION_ERROR;
//       }

//    }
   return(ret_val);
}

static void BTPSAPI DEVM_Authentication_Callback(DEVM_Authentication_Information_t *AuthenticationRequestInformation, void *CallbackParameter)
{
   int                               Result;
   char                              Buffer[32];
   Boolean_t                         LowEnergy;
   DEVM_Authentication_Information_t AuthenticationResponseInformation;

   if(AuthenticationRequestInformation)
   {
      printf("\r\n");

//      BD_ADDRToStr(AuthenticationRequestInformation->BD_ADDR, Buffer);

//      printf("Authentication Request received for %s.\r\n", Buffer);

      /* Check to see if this is an LE event.                           */
      if(AuthenticationRequestInformation->AuthenticationAction & DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK)
      {
         AuthenticationRequestInformation->AuthenticationAction &= ~DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;

         LowEnergy = TRUE;
      }
      else
         LowEnergy = FALSE;

      switch(AuthenticationRequestInformation->AuthenticationAction)
      {
         case DEVM_AUTHENTICATION_ACTION_PIN_CODE_REQUEST:
            printf("PIN Code Request.\r\n.");

            /* Note the current Remote BD_ADDR that is requesting the   */
            /* PIN Code.                                                */
            CurrentRemoteBD_ADDR = AuthenticationRequestInformation->BD_ADDR;

            /* Inform the user that they will need to respond with a PIN*/
            /* Code Response.                                           */
            printf("\r\nRespond with the command: PINCodeResponse\r\n");
            break;
         case DEVM_AUTHENTICATION_ACTION_USER_CONFIRMATION_REQUEST:
            printf("User Confirmation Request %s.\r\n", (LowEnergy?"LE":"BR/EDR"));

            /* Note the current Remote BD_ADDR that is requesting the   */
            /* User Confirmation.                                       */
            CurrentRemoteBD_ADDR = AuthenticationRequestInformation->BD_ADDR;
            CurrentLowEnergy     = FALSE;

            if(!LowEnergy)
            {
               if(IOCapability != icDisplayYesNo)
               {
                  /* Invoke Just works.                                 */

                  printf("\r\nAuto Accepting: %lu\r\n", (unsigned long)AuthenticationRequestInformation->AuthenticationData.Passkey);

                  BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

                  AuthenticationResponseInformation.BD_ADDR                         = AuthenticationRequestInformation->BD_ADDR;
                  AuthenticationResponseInformation.AuthenticationAction            = DEVM_AUTHENTICATION_ACTION_USER_CONFIRMATION_RESPONSE;
                  AuthenticationResponseInformation.AuthenticationDataLength        = sizeof(AuthenticationResponseInformation.AuthenticationData.Confirmation);

                  AuthenticationResponseInformation.AuthenticationData.Confirmation = (Boolean_t)TRUE;

                  if((Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation)) >= 0)
                     printf("DEVM_AuthenticationResponse() Success.\r\n");
                  else
                     printf("DEVM_AuthenticationResponse() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));

                  /* Flag that there is no longer a current             */
                  /* Authentication procedure in progress.              */
//                  ASSIGN_BD_ADDR(CurrentRemoteBD_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
               }
               else
               {
                  printf("User Confirmation: %lu\r\n", (unsigned long)AuthenticationRequestInformation->AuthenticationData.Passkey);

                  /* Inform the user that they will need to respond with*/
                  /* a PIN Code Response.                               */
                  printf("\r\nRespond with the command: UserConfirmationResponse\r\n");
                  BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

                  AuthenticationResponseInformation.BD_ADDR                         = CurrentRemoteBD_ADDR;
                  AuthenticationResponseInformation.AuthenticationAction            = DEVM_AUTHENTICATION_ACTION_USER_CONFIRMATION_RESPONSE;
                  AuthenticationResponseInformation.AuthenticationDataLength        = sizeof(AuthenticationResponseInformation.AuthenticationData.Confirmation);

                  AuthenticationResponseInformation.AuthenticationData.Confirmation = (Boolean_t)TRUE;

                  if(CurrentLowEnergy)
                     AuthenticationResponseInformation.AuthenticationAction |= DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;

                  /* Submit the Authentication Response.                      */
                  Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation);
               }
            }
            else
            {
               /* Flag that this is LE Pairing.                         */
               CurrentLowEnergy = TRUE;

               /* Inform the user that they will need to respond with a */
               /* PIN Code Response.                                    */
               printf("\r\nRespond with the command: UserConfirmationResponse\r\n");
               BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

               AuthenticationResponseInformation.BD_ADDR                         = CurrentRemoteBD_ADDR;
               AuthenticationResponseInformation.AuthenticationAction            = DEVM_AUTHENTICATION_ACTION_USER_CONFIRMATION_RESPONSE;
               AuthenticationResponseInformation.AuthenticationDataLength        = sizeof(AuthenticationResponseInformation.AuthenticationData.Confirmation);

               AuthenticationResponseInformation.AuthenticationData.Confirmation = (Boolean_t)TRUE;

               if(CurrentLowEnergy)
                  AuthenticationResponseInformation.AuthenticationAction |= DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;

               /* Submit the Authentication Response.                      */
               Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation);
            }
            break;
         case DEVM_AUTHENTICATION_ACTION_PASSKEY_REQUEST:

            printf("Passkey Request %s.\r\n", (LowEnergy?"LE":"BR/EDR"));

            /* Note the current Remote BD_ADDR that is requesting the   */
            /* Passkey.                                                 */
            CurrentRemoteBD_ADDR = AuthenticationRequestInformation->BD_ADDR;
            CurrentLowEnergy     = LowEnergy;

            /* Inform the user that they will need to respond with a    */
            /* Passkey Response.                                        */
            printf("\r\nRespond with the command: PassKeyResponse\r\n");

            BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

            AuthenticationResponseInformation.BD_ADDR                    = CurrentRemoteBD_ADDR;
            AuthenticationResponseInformation.AuthenticationAction       = DEVM_AUTHENTICATION_ACTION_PASSKEY_RESPONSE;
            AuthenticationResponseInformation.AuthenticationDataLength   = sizeof(AuthenticationResponseInformation.AuthenticationData.Passkey);

            AuthenticationResponseInformation.AuthenticationData.Passkey = AuthenticationRequestInformation->AuthenticationData.Passkey;

            if(CurrentLowEnergy)
               AuthenticationResponseInformation.AuthenticationAction |= DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;

            /* Submit the Authentication Response.                      */
            Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation);

            break;
         case DEVM_AUTHENTICATION_ACTION_PASSKEY_INDICATION:
            printf("PassKey Indication %s.\r\n", (LowEnergy?"LE":"BR/EDR"));
            printf("PassKey: %lu\r\n", (unsigned long)AuthenticationRequestInformation->AuthenticationData.Passkey);
            break;
         case DEVM_AUTHENTICATION_ACTION_KEYPRESS_INDICATION:
            printf("Keypress Indication.\r\n");

            printf("Keypress: %d\r\n", (int)AuthenticationRequestInformation->AuthenticationData.Keypress);
            break;
         case DEVM_AUTHENTICATION_ACTION_OUT_OF_BAND_DATA_REQUEST:
            printf("Out of Band Data Request: %s.\r\n", (LowEnergy?"LE":"BR/EDR"));

            /* This application does not support OOB data so respond    */
            /* with a data length of Zero to force a negative reply.    */
            BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

            AuthenticationResponseInformation.BD_ADDR                  = AuthenticationRequestInformation->BD_ADDR;
            AuthenticationResponseInformation.AuthenticationAction     = DEVM_AUTHENTICATION_ACTION_OUT_OF_BAND_DATA_RESPONSE;
            AuthenticationResponseInformation.AuthenticationDataLength = 0;

            if(LowEnergy)
               AuthenticationResponseInformation.AuthenticationAction |= DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;

            if((Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation)) >= 0)
               printf("DEVM_AuthenticationResponse() Success.\r\n");
            else
               printf("DEVM_AuthenticationResponse() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
            break;
         case DEVM_AUTHENTICATION_ACTION_IO_CAPABILITIES_REQUEST:
            printf("I/O Capability Request: %s.\r\n", (LowEnergy?"LE":"BR/EDR"));

            /* Note the current Remote BD_ADDR that is requesting the   */
            /* Passkey.                                                 */
            CurrentRemoteBD_ADDR = AuthenticationRequestInformation->BD_ADDR;
            CurrentLowEnergy     = LowEnergy;

            /* Respond with the currently configured I/O Capabilities.  */
            BTPS_MemInitialize(&AuthenticationResponseInformation, 0, sizeof(AuthenticationResponseInformation));

            AuthenticationResponseInformation.BD_ADDR              = AuthenticationRequestInformation->BD_ADDR;
            AuthenticationResponseInformation.AuthenticationAction = DEVM_AUTHENTICATION_ACTION_IO_CAPABILITIES_RESPONSE;

            if(!CurrentLowEnergy)
            {
               AuthenticationResponseInformation.AuthenticationDataLength                                    = sizeof(AuthenticationResponseInformation.AuthenticationData.IOCapabilities);

               AuthenticationResponseInformation.AuthenticationData.IOCapabilities.IO_Capability             = (GAP_IO_Capability_t)IOCapability;
//xxx Check if MITM should be set
               AuthenticationResponseInformation.AuthenticationData.IOCapabilities.MITM_Protection_Required  = MITMProtection;
               AuthenticationResponseInformation.AuthenticationData.IOCapabilities.OOB_Data_Present          = OOBSupport;

//xxx Here check if this will default to "just works" or auto accept.
            }
            else
            {
               AuthenticationResponseInformation.AuthenticationAction                                       |= DEVM_AUTHENTICATION_ACTION_LOW_ENERGY_OPERATION_MASK;
               AuthenticationResponseInformation.AuthenticationDataLength                                    = sizeof(AuthenticationResponseInformation.AuthenticationData.LEIOCapabilities);
               AuthenticationResponseInformation.AuthenticationData.LEIOCapabilities.IO_Capability           = (GAP_LE_IO_Capability_t)IOCapability;
               AuthenticationResponseInformation.AuthenticationData.LEIOCapabilities.Bonding_Type            = lbtBonding;
               AuthenticationResponseInformation.AuthenticationData.LEIOCapabilities.MITM                    = MITMProtection;
               AuthenticationResponseInformation.AuthenticationData.LEIOCapabilities.OOB_Present             = OOBSupport;
            }

            if((Result = DEVM_AuthenticationResponse(AuthenticationCallbackID, &AuthenticationResponseInformation)) >= 0)
               printf("DEVM_AuthenticationResponse() Success.\r\n");
            else
               printf("DEVM_AuthenticationResponse() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
            break;
         case DEVM_AUTHENTICATION_ACTION_IO_CAPABILITIES_RESPONSE:
            printf("I/O Capability Response.\r\n");

            /* Inform the user of the Remote I/O Capablities.           */
//            printf("Remote I/O Capabilities: %s, MITM Protection: %s.\r\n", IOCapabilitiesStrings[AuthenticationRequestInformation->AuthenticationData.IOCapabilities.IO_Capability], AuthenticationRequestInformation->AuthenticationData.IOCapabilities.MITM_Protection_Required?"TRUE":"FALSE");
            break;
         case DEVM_AUTHENTICATION_ACTION_AUTHENTICATION_STATUS_RESULT:
            printf("Authentication Status: .\r\n");

            printf("Status: %d\r\n", AuthenticationRequestInformation->AuthenticationData.AuthenticationStatus);
            break;
         case DEVM_AUTHENTICATION_ACTION_AUTHENTICATION_START:
            printf("Authentication Start.\r\n");
            break;
         case DEVM_AUTHENTICATION_ACTION_AUTHENTICATION_END:
            printf("Authentication End.\r\n");
            break;
         default:
            printf("Unknown Device Manager Authentication Event Received: 0x%08X, Length: 0x%08X.\r\n", (unsigned int)AuthenticationRequestInformation->AuthenticationAction, AuthenticationRequestInformation->AuthenticationDataLength);
            break;
      }
   }
   else
      printf("\r\nDEVM Authentication Request Data is NULL.\r\n");

//   printf("GATM>");

   /* Make sure the output is displayed to the user.                    */
   fflush(stdout);
}

// main program.
int main (int argc, char* argv[])
{
  int  Result = !EXIT_CODE;
  int ret_val;
  
  DEVM_Local_Device_Properties_t LocalDeviceProperties;
  
  int initialized = 0, poweredon = 0, advertised = 0; 
  
  struct sigaction SignalAction;
  

//  {-45, -45, -45, -45, -45, -45, -45, 20};

//  {31, 31, 31, 31, 31, 31, 31, 31};
//  Byte_t powerlevels_gfsk_lp[8] = {20, 20, 20, 20, 20, 20, 20, 20};
//  Byte_t powerlevels_ble_lp[8] = {20, 20, 20, 20, 20, 20, 20, 20};
//  Byte_t powerlevels_ant_lp[8] = {20, 20, 20, 20, 20, 20, 20, 20};
//  Byte_t powerlevels_btbr_lp[8] = {16, 16, 16, 16, 16, 16, 16, 16};
//  Byte_t powerlevels_btedr_lp[8] = {16, 16, 16, 16, 16, 16, 16, 16};
//
//  Byte_t powerlevels_ble_hp[8] = {31, 31, 31, 31, 31, 31, 31, 31};
//  Byte_t powerlevels_gfsk_hp[8] = {31, 31, 31, 31, 31, 31, 31, 31};
//  Byte_t powerlevels_ant_hp[8] = {31, 31, 31, 31, 31, 31, 31, 31};
//  Byte_t powerlevels_btbr_hp[8] = {20, 20, 20, 20, 20, 20, 20, 20};
//  Byte_t powerlevels_btedr_hp[8] = {20, 20, 20, 20, 20, 20, 20, 20};
  ServiceMutex     = BTPS_CreateMutex(FALSE);
  
   /* Initialize the signal set to empty.                               */
  sigemptyset(&SignalAction.sa_mask);
  
   /* Flag that we want to ignore this signal.                          */
  SignalAction.sa_handler = SIG_IGN;
  
  SignalAction.sa_flags = SA_RESTART;
  
     /* Set the SIGTTIN signal's action.  Note that the default behavior  */
   /* for SIGTTIN is to suspend the application.  We want to ignore the */
   /* signal instead so that this doesn't happen.  When the read()      */
   /* function fails the we can check the error number to determine if  */
   /* it was interrupted by a signal, and if it was then we can wait    */
   /* until we have re-entered the foreground before attempting to read */
   /* from standard input again.                                        */
  sigaction(SIGTTIN, &SignalAction, NULL);


  if(initialized!=1)
  {	
    Result = BTPM_Initialize((unsigned long)getpid(), NULL, NULL, NULL); 	//Initializes Bluetooth Platform manager input:- PID, Init info, Unregistration call back and Unregistration paramters
    initialized = 1;
  }
  
  if(!Result)
  {
    printf("BTPM_Initialize() Success: %d.\r\n", Result);

    if(poweredon!=1)
    {
     Result = DEVM_PowerOnDevice();			//Power on chip, enable's bt_en gpio, gives firmware file and tty* node to chip.
     poweredon=1;
    }


    if(!Result)
    {
      printf("DEVM_Power%sDevice() Success: %d.\r\n", "On", Result);
    }
    else
    {
       printf("DEVM_Power%sDevice() Failure: %d.\r\n", "On", Result);
    }

//    Result = PM_VS_DRPb_Set_Power_Vector(0x00, &powerlevels_gfsk_lp[0], 0x03F);
//  //   Result = VS_Set_Max_Output_Power(0x01);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector gfsk_lp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector gfsk_lp: Fail\r\n");
//
//    Result = PM_VS_DRPb_Set_Power_Vector(0x03, &powerlevels_ble_lp[0], 0x03F);
//  //   Result = VS_Set_Max_Output_Power(0x01);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector ble_lp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector ble_lp: Fail\r\n");
//
//    Result = PM_VS_DRPb_Set_Power_Vector(0x04, &powerlevels_ant_lp[0], 0x03F);
//  //   Result = VS_Set_Max_Output_Power(0x01);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector ant_lp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector ant_lp: Fail\r\n");
//
//    Result = PM_VS_DRPb_Set_Power_Vector(0x01, &powerlevels_btedr_lp[0], 0x03F);
//  //   Result = VS_Set_Max_Output_Power(0x01);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector btedr_lp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector btedr_lp: Fail\r\n");
//
//    Result = PM_VS_DRPb_Set_Power_Vector(0x02, &powerlevels_btbr_lp[0], 0x03F);
//  //   Result = VS_Set_Max_Output_Power(0x01);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector btbr_lp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector btbr_lp: Fail\r\n");
//
//
//    Result = PM_VS_DRPb_Set_Power_Vector(8, &powerlevels_gfsk_hp[0], 0x03F);
//    if(!Result)
//    	printf("PM_VS_DRPb_Set_Power_Vector gfsk_hp: Success %d\r\n", Result);
//	else
//		printf("PM_VS_DRPb_Set_Power_Vector gfsk_hp: Fail\r\n");
//
//    Result = PM_VS_DRPb_Set_Power_Vector(9, &powerlevels_ble_hp[0], 0x03F);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector ble_hp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector ble_hp: Fail\r\n");
//
//
//    Result = PM_VS_DRPb_Set_Power_Vector(10, &powerlevels_ant_hp[0], 0x03F);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector ant_hp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vectorant_hp: Fail\r\n");
//
//
//    Result = PM_VS_DRPb_Set_Power_Vector(11, &powerlevels_btedr_hp[0], 0x03F);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector btedr_hp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector btedr_hp: Fail\r\n");
//
//
//    Result = PM_VS_DRPb_Set_Power_Vector(12, &powerlevels_btbr_hp[0], 0x03F);
//    if(!Result)
//  	   printf("PM_VS_DRPb_Set_Power_Vector btbr_hp: Success %d\r\n", Result);
//    else
//  	   printf("PM_VS_DRPb_Set_Power_Vector btbr_hp: Fail\r\n");
//
//
//   Result = PM_VS_DRPb_Enable_RF_Calibration_Enhanced(0x0, 0xFF, 0x00000800, 0x01);
//   if(!Result)
// 	   printf("PM_VS_DRPb_Enable_RF_Calibration_Enhanced: Success %d\r\n", Result);
//   else
// 	   printf("PM_VS_DRPb_Enable_RF_Calibration_Enhanced: Fail\r\n");


    if((Result = DEVM_RegisterEventCallback(DEVM_Event_Callback, NULL)) > 0)		//Device callback function input:- Callback function and callback paramters
    {
      printf("DEVM_RegisterEventCallback() Success: %d.\r\n", Result);
    }
    else
    {                 
      printf("DEVM_RegisterEventCallback() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
				  
//       BTPM_Cleanup();
    }
  }	
   else
   {                            
      printf("Initialization Failure: Already initialized.\r\n");
   }
   
   BTPS_MemInitialize(&LocalDeviceProperties, 0, sizeof(LocalDeviceProperties));		//fills local memory with local devie information.
   
   LocalDeviceProperties.DeviceNameLength = 6;
   strcpy(LocalDeviceProperties.DeviceName, "matrix");
   
   LocalDeviceProperties.PairableMode = TRUE;
   LocalDeviceProperties.PairableModeTimeout = 0xFFFFFF;

   if((Result = DEVM_UpdateLocalDeviceProperties(DEVM_UPDATE_LOCAL_DEVICE_PROPERTIES_DEVICE_NAME, &LocalDeviceProperties)) >= 0)	//updates local device properties, used here to change name of device.
   {
     printf("DEVM_UpdateLocalDeviceProperties() Success: %d.\r\n", Result);
   }
  else
   {
         /* Error updating the Local Device Properties, inform the user */
         /* and flag an error.                                          */
      printf("DEVM_UpdateLocalDeviceProperties() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
    }
   
   if((Result = DEVM_UpdateLocalDeviceProperties(DEVM_UPDATE_LOCAL_DEVICE_PROPERTIES_PAIRABLE_MODE, &LocalDeviceProperties)) >= 0)	//updates local device properties, used here to change name of device.
   {
     printf("DEVM_UpdateLocalDeviceProperties() Success: %d.\r\n", Result);
   }
  else
   {
         /* Error updating the Local Device Properties, inform the user */
         /* and flag an error.                                          */
      printf("DEVM_UpdateLocalDeviceProperties() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
    }

   if((Result = GATM_RegisterEventCallback(GATM_Event_Callback, NULL)) > 0)		//Local GATT related events callback, input:- call back function and callback parameters
   {
     printf("GATM_RegisterEventCallback() Success: %d.\r\n", Result);

            /* Note the Callback ID and flag success.                   */
     GATMCallbackID = (unsigned int)Result;

     ret_val        = 0;
   }
   else
   {
            /* Error registering the Callback, inform user and flag an  */
            /* error.                                                   */
     printf("GATM_RegisterEventCallback() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));

     ret_val = FUNCTION_ERROR;
   }
//   update_connectable_mode();
// while(1)
// {



    if(advertised != 1)
    {
     BTPS_MemInitialize(&AdvertisingInfo, 0, sizeof(DEVM_Advertising_Information_t));	//Fills local memory with advertising parameters:- flags, timing
     
     AdvertisingInfo.AdvertisingFlags = 0x676;		//flags
     
     AdvertisingInfo.AdvertisingDuration = 0xFFFFFFFF;	//Duration
     
     RegisterService(argc);		//Registers service to GATT profile.
     
//     if(argc==0)
//    	 IOCapability = icDisplayOnly;
//     else if(argc==1)
//    	 IOCapability = icDisplayYesNo;
//     else if(argc==2)
//    	 IOCapability = icKeyboardOnly;
//     else if(argc==3)
//    	 IOCapability = icNoInputNoOutput;
//
//     MITMProtection = TRUE;

//     if((Result = DEVM_RegisterAuthentication(DEVM_Authentication_Callback, NULL)) >= 0)
//     {
//    	 printf("DEVM_RegisterAuthentication() Success: %d.\r\n", Result);
//
//    	 /* Note the Authentication Callback ID.                        */
//    	 AuthenticationCallbackID = (unsigned int)Result;
//
//    	 /* Flag success.                                               */
//     }
//     else
//     {
//    	 /* Error Registering for Authentication, inform the user and   */
//    	 /* flag an error.                                              */
//    	 printf("DEVM_RegisterAuthentication() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
//
//     }
    }
	else
	{
	/* Not initialized, flag an error.                                */
	printf("Platform Manager has not been initialized.\r\n");
	}

    if((Result = DEVM_StartAdvertising(&AdvertisingInfo)) == 0)		//Starts advertising. input:- Above given paramters.
    {
      advertised = 1;
      printf("DEVM_StartAdvertising() Success: Duration %lu seconds.\r\n", AdvertisingInfo.AdvertisingDuration);
    }
    else
    {
      printf("DEVM_StartAdvertising() Failure: %d, %s.\r\n", Result, ERR_ConvertErrorCodeToString(Result));
    }
//    }
//    }
// }
//   	Result = GAP_LE_Perform_Scan(0, stPassive, 3, 3, latRandom, fpNoFilter, FALSE, NULL, 0);
//   	if(!Result)
//   		printf("GAP_LE_Perform_Scan : %s", "success");
//   	else
//   		printf("GAP_LE_Perform_Scan : %s", "Failure");

  while(Result!=EXIT_CODE);
  advertised = 0;

  BTPS_CloseMutex(ServiceMutex);
}


