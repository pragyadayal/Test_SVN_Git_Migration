#if !defined MX_SELF_ENROLLMENT_H
#define MX_SELF_ENROLLMENT_H
///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Yashavadan Dalsaniya
//   File         : MxSelfEnrollment.h
//   Description  :
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxSlaveCommonMsgList.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
void SwitchToSelfEnrlInputState(void);
BOOL IsBioCredSelfEnrollReq(void);
void DcWaitForSelfEnrlInputState(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);

#endif /* MX_SELF_ENROLLMENT_H */
