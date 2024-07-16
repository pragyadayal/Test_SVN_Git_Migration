
#include "SS1BTPM.h"          /* BTPM Application Programming Interface.      */

typedef struct _tagCharacteristicInfo_t
{
   unsigned long  CharacteristicPropertiesMask;
   unsigned long  SecurityPropertiesMask;
   UUID_128_t     CharacteristicUUID;
   Boolean_t      AllocatedValue;
   unsigned int   MaximumValueLength;
   unsigned int   ValueLength;
   Byte_t        *Value;
} CharacteristicInfo_t;

   /* The following type definition represents the container structure  */
   /* for a Descriptor Attribute.                                       */
typedef struct _tagDescriptorInfo_t
{
   unsigned long  DescriptorPropertiesMask;
   unsigned long  SecurityPropertiesMask;
   UUID_128_t     CharacteristicUUID;
   Boolean_t      AllocatedValue;
   unsigned int   MaximumValueLength;
   unsigned int   ValueLength;
   Byte_t        *Value;
} DescriptorInfo_t;

   /* The following enumeration represents all of the different         */
   /* attributes that may be added in a service table.                  */
typedef enum
{
   atInclude,
   atCharacteristic,
   atDescriptor
} AttributeType_t;

   /* The following type definition represents the container structure  */
   /* for a Service Table.                                              */
   /* * NOTE * For an AttributeType of atInclude the AttributeParameter */
   /*          is not used (the include will automatically reference the*/
   /*          previous service that was registered).                   */
typedef struct _tagAttributeInfo_t
{
   AttributeType_t  AttributeType;
   unsigned int     AttributeOffset;
   void            *Attribute;
} AttributeInfo_t;

   /* The following type definition represents the container structure  */
   /* for an dynamically allocated service.                             */
typedef struct _tagServiceInfo_t
{
   unsigned long                  Flags;
   unsigned int                   ServiceID;
   DWord_t                        PersistentUID;
   UUID_128_t                     ServiceUUID;
   GATT_Attribute_Handle_Group_t  ServiceHandleRange;
   unsigned int                   NumberAttributes;
   AttributeInfo_t               *AttributeList;
} ServiceInfo_t;

#define SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID                 0x00000001
#define SERVICE_TABLE_FLAGS_SECONDARY_SERVICE                  0x00000002

   /*********************************************************************/
   /* Service Tables.                                                   */
   /*********************************************************************/
   /* * NOTE * For simplicity this application will not include Client  */
   /*          Characteristic Configuration Descriptors (CCCD) for      */
   /*          characteristics that are indicatable/notifiable.  This is*/
   /*          because the CCCD is a per client value that is stored    */
   /*          persistently for bonded devices.  This application, whose*/
   /*          only purpose is showing the usage of the APIs, does not  */
   /*          store per client values and also does not store values   */
   /*          persistently.                                            */
   /* * NOTE * To Calculate the AttributeOffset apply the following     */
   /*          formula:                                                 */
   /*                                                                   */
   /*             AttributeOffset = 1 + (NumPrevIncludes * 1) +         */
   /*                               (NumPrevCharacteristics * 2) +      */
   /*                               (NumPrevDescriptors * 1)            */
   /*                                                                   */
   /*          where:                                                   */
   /*                                                                   */
   /*             NumPrevIncludes = The number of previous Include      */
   /*                               Definition that exist in the        */
   /*                               service table prior to the attribute*/
   /*                               (Include, Characteristic or         */
   /*                               Descriptor) that is being added.    */
   /*                                                                   */
   /*             NumPrevCharacteristics = The number of previous       */
   /*                               Characteristics that exist in the   */
   /*                               service table prior to the attribute*/
   /*                               (Include, Characteristic or         */
   /*                               Descriptor) that is being added.    */
   /*                                                                   */
   /*             NumPrevDescriptors = The number of previous           */
   /*                               Descriptors that exist in the       */
   /*                               service table prior to the attribute*/
   /*                               (Include, Characteristic or         */
   /*                               Descriptor) that is being added.    */

   /*********************************************************************/
   /* Service Declaration.                                              */
   /*********************************************************************/


   /*********************************************************************/
   /* Service Declaration.                                              */
   /*********************************************************************/

   static CharacteristicInfo_t Srv3Attr1=
{
   /* Characteristic Properties.                                        */
   GATM_CHARACTERISTIC_PROPERTIES_WRITE,

   /* Characteristic Security Properties.                               */
//   GATM_SECURITY_PROPERTIES_UNAUTHENTICATED_ENCRYPTION_WRITE,
   GATM_SECURITY_PROPERTIES_NO_SECURITY,

   /* Characteristic UUID.                                              */
   { 0x08, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},

   /* Value is allocated dynamically.                                   */
   TRUE,

   /* Maximum value length.                                             */
   512,

   /* Current value length.                                             */
   0,

   /* Current value.                                                    */
   (Byte_t *)NULL
};

static CharacteristicInfo_t Srv3Attr2=
{
   /* Characteristic Properties.                                        */
   GATM_CHARACTERISTIC_PROPERTIES_READ,

   /* Characteristic Security Properties.                               */
   GATM_SECURITY_PROPERTIES_NO_SECURITY,

   /* Characteristic UUID.                                              */
   { 0x09, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},

   /* Value is allocated dynamically.                                   */
   TRUE,

   /* Maximum value length.                                             */
   512,

   /* Current value length.                                             */
   0,

   /* Current value.                                                    */
   (Byte_t *)NULL
};

static CharacteristicInfo_t Srv3Attr3=
{
   /* Characteristic Properties.                                        */
   GATM_CHARACTERISTIC_PROPERTIES_READ,

   /* Characteristic Security Properties.                               */
   GATM_SECURITY_PROPERTIES_NO_SECURITY,

   /* Characteristic UUID.                                              */
   { 0x1A, 0xFB, 0x08, 0xAF, 0xBC, 0xE1, 0x8A, 0x0D, 0x76, 0x63, 0xAA, 0xBD, 0xEF, 0xCA, 0x89, 0x0A },

   /* Value is allocated dynamically.                                   */
   FALSE,

   /* Maximum value length.                                             */
   165,

   /* Current value length.                                             */
   0,

   /* Current value.                                                    */
   (Byte_t *)NULL
};



   /* The following array is the array containing attributes that are   */
   /* registered by this service.                                       */
static AttributeInfo_t SrvTable3[] =
{
//   { atInclude, 1, NULL },
   { atCharacteristic, 1, (void *)&Srv3Attr1 },
   { atCharacteristic, 3, (void *)&Srv3Attr2 }
//    { atCharacteristic, 5, (void *)&Srv3Attr3 }
};

   /*********************************************************************/
   /* End of Service Declaration.                                       */
   /*********************************************************************/

                                
static ServiceInfo_t ServiceTable[] =
{
   {
      /* Service Flags.                                                 */
      SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID,

      /* Service ID.                                                    */
      0,

      /* PersistentUID.                                                 */
      0,

      /* Service UUID.                                                  */
      { 0x07, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},

      /* Service Handle Range.                                          */
      {0, 0},

      /* Number of Service Table Entries.                               */
      2,

      /* Service Attribute List.                                        */
      SrvTable3
   }
};

#define NUMBER_OF_SERVICES                                     (sizeof(ServiceTable)/sizeof(ServiceInfo_t))
