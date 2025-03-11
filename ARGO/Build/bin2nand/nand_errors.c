/*____________________________________________________________________________
*
*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION
*
*   Property of Texas Instruments
*   For Unrestricted Internal Use Only
*   Unauthorized reproduction and/or distribution is strictly prohibited.
*   This product is protected under copyright law and trade secret law as an
*   unpublished work.
*   Created 2008, (C) Copyright 2009 Texas Instruments.  All rights reserved.
*
*   Component  :  bin2nand
*
*   Filename   :  nand_errors.c
*
*   Description:  
*_____________________________________________________________________________
*
*   History:
*   26-Aug-08    Martin Zarzycki
*                Creation
*_____________________________________________________________________________*/


/*--------------------------------- Include Files ----------------------------*/

#include "romcode_types.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
/*----------------------------------- Defines --------------------------------*/

#define cmEmpty 0xFFFFFFFF

/*------------------------------- Local Constants ----------------------------*/

/*------------------------------- Local Variables ----------------------------*/

/*--------------------------- Local Functions Declarations -------------------*/

/*------------------------------- Global Functions ---------------------------*/

/*------------------------------------------------------------------------------
* FUNCTION    : NE_GenerateRandomErrors
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
STATUS NE_GenerateRandomErrors( U8*   ipData,
                                U32   ipSize,  
                                U32   iErrors )
{
  U32   i;
 
  U32   lv_Error = 0;
  U32   lv_ErrorBitPosition;
  U32*  lv_pErrorBitPositions;
  
  U32   lv_ErrorBytePosition;
  U32   lv_ErrorByteOffset;
    
  // Reserve space for error positions.
  lv_pErrorBitPositions = (U32*)malloc( iErrors * sizeof(U32) );
  
  // Reset the position array. */
  memset( (U8*)lv_pErrorBitPositions, 0xFF, iErrors * sizeof(U32) );
  
  while( lv_Error < iErrors )
  {
    // Generate a random bit error position withing the sector.
    lv_ErrorBitPosition = rand() % ( ipSize * 8 );
    
    // Check that this position didn't occur before.
    for( i=0; i<iErrors; i++ )
    {
      // Leave FOR loop if no more positions to check.
      if( lv_pErrorBitPositions[i] == cmEmpty )
        break; // FOR ( i=0; i<iErrors; i++ )
      
      // Leave FOR loop, if position already exists. 
      if( lv_pErrorBitPositions[i] == lv_ErrorBitPosition )
      {
        lv_ErrorBitPosition = cmEmpty;
        break; // FOR ( i=0; i<iErrors; i++ )
      }
    } // ENDFOR 
    
    // Generate another position.
    if( lv_ErrorBitPosition == cmEmpty ) 
      continue; // WHILE ( lv_Error < iErrors )
       
    // Process new error location.
    lv_ErrorBytePosition = lv_ErrorBitPosition / 8; 
    lv_ErrorByteOffset = lv_ErrorBitPosition % 8;
      
    // Flip bit:
    ipData[lv_ErrorBytePosition] ^= ( 1 << lv_ErrorByteOffset );
      
    // Update position list.
    lv_pErrorBitPositions[lv_Error] = lv_ErrorBitPosition;
    lv_Error++;
    
  } // ENDWHILE ( lv_Error < iErrors )
  
  // Cleanup.
  free( lv_pErrorBitPositions );
  
  return NO_ERROR;    
}
//ENDFUNCTION NE_GenerateRandomErrors  



/*-------------------------------- Local Functions ---------------------------*/



/*---------------------------------- End of File -----------------------------*/



