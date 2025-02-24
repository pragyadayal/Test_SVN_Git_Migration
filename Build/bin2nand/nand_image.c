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
*   Filename   :  nand_image.c
*
*   Description:  
*_____________________________________________________________________________
*
*   History:
*   25-Jul-08    Martin Zarzycki
*                Creation
*_____________________________________________________________________________*/


/*--------------------------------- Include Files ----------------------------*/

#include "romcode_types.h"

#include "nand_image.h"
#include "nand_errors.h"
#include "nand.h"
#include "bch_encoder.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*----------------------------------- Defines --------------------------------*/

/*------------------------------- Local Constants ----------------------------*/

/*------------------------------- Local Variables ----------------------------*/

/*--------------------------- Local Functions Declarations -------------------*/

STATUS ni_CreateOneNand( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile );
STATUS ni_CreateRawNand( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile );

/*------------------------------- Global Functions ---------------------------*/

/*------------------------------------------------------------------------------
* FUNCTION    : ni_CreateImage
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
STATUS ni_CreateImage( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile )
{
  STATUS status;
    
  if( ipParams->NandType == NAND_TYPE_ONE )
  {
    /* Create a (Flex)OneNAND image. */
    status = ni_CreateOneNand( ipParams, ipData, ipFile );
  }  
  else
  {  
    /* Create a RAW NAND image. */
    status = ni_CreateRawNand( ipParams, ipData, ipFile );
  }    
  
  return status;
}


/*-------------------------------- Local Functions ---------------------------*/

/*------------------------------------------------------------------------------
* FUNCTION    : ni_CreateOneNand
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
STATUS ni_CreateOneNand( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile )
{
  const U32 lc_SectorSpareSize = 16;
  
  U32 k;    
  
  U32 lv_Page, lv_Sector;
  
  U8* lv_pSectorPtr = 0;
  
  // Get the number of sectors per page.
  U32 lv_Sectors = ipParams->PageSize / NAND_SECTOR_SIZE;  
  U32 lv_SectorsLimit = lv_Sectors;  
  
  // Cut the number of usable sectors per page to the OneNand limit.
  if( lv_SectorsLimit > NAND_ONENAND_SECTOR_LIMIT )
    lv_SectorsLimit = NAND_ONENAND_SECTOR_LIMIT;
  
  // Setup page and sector iteration.
  lv_pSectorPtr = ipData;
  
  // FOR( all pages in the block )
  for( lv_Page = 0; lv_Page < ipParams->BlockSize; lv_Page++)
  {                
    // FOR( all usable SECTORS in a page )
    for( lv_Sector = 0; lv_Sector < lv_SectorsLimit; lv_Sector++ )
    {
      // Store SECTOR DATA into FILE
      for( k=0; k < NAND_SECTOR_SIZE; k++ )
        fputc( lv_pSectorPtr[k], ipFile );
        
      // Store empty (0xFF) SECTOR SPARE
      for( k=0; k < lc_SectorSpareSize; k++ )
        fputc( 0xFF, ipFile );   

      // Update SECTOR pointer.
      lv_pSectorPtr += NAND_SECTOR_SIZE;
      
      // Display progress
      printf("  Page: %.3d/%.3d Sector: %d/%d\r",
        lv_Page+1, ipParams->BlockSize,
        lv_Sector+1, lv_Sectors ); 
    
      fflush(stdout);   
      
    } // ENDFOR( all SECTOR in a page )
    
    // Fill remaining sectors with '0xFF' if applicable.
    for( lv_Sector = lv_SectorsLimit; lv_Sector < lv_Sectors; lv_Sector++ )
    { 
      // Store SECTOR DATA into FILE
      for( k=0; k < NAND_SECTOR_SIZE; k++ )
        fputc( 0xFF, ipFile );
      
      // Store empty SECTOR SPARE
      for( k=0; k < lc_SectorSpareSize; k++ )
        fputc( 0xFF, ipFile );    
    }            
  } // ENDFOR( all pages in the block )
  
  printf("\n");
  
  return NO_ERROR;  
}

/*------------------------------------------------------------------------------
* FUNCTION    : ni_CreateRawNand
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
STATUS ni_CreateRawNand( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile )
{
  U32 i,k;    
  
  U32 lv_Page, lv_Sector;
  
  U32 lv_SectorBchSize;
  U32 lv_SectorSpareSize;
  
  U8* lv_pPageSpare = 0;
  U8* lv_pSectorSpare = 0;
  U8* lv_pSectorData = 0;  
  
  U8* lv_pPagePtr = 0;
  U8* lv_pSectorPtr = 0;
  U8* lv_pSparePtr = 0;
  
  U32 lv_CorrectableErrors;
  
  // Get the number of sectors per page.
  U32 lv_Sectors = ipParams->PageSize / NAND_SECTOR_SIZE;  
  
  // Get the length of ECC data in the spare area per sector.
  switch( ipParams->EccType )
  {
    case NAND_ECC_BCH8:
      lv_SectorBchSize = 13; // 13 bytes BCH remainder
      lv_SectorSpareSize = 14;
      lv_CorrectableErrors = 8;
      break;
    
    case NAND_ECC_NONE:
    default:
      lv_SectorBchSize = 0; 
      lv_SectorSpareSize = 0;
      lv_CorrectableErrors = 0;
      break;
  }
      
  // Reserve a buffer for the PAGE SPARE.
  lv_pPageSpare = (U8*)malloc( ipParams->SpareSize ); 
  if(  lv_pPageSpare == 0 )
  {  
    printf("  Error! Failed to allocate 'page spare'.\n");
    goto cleanup;
  }
  
  // Reserve buffers for BCH encoding.
  if( ipParams->EccType != NAND_ECC_NONE )
  {
    // Reserve a buffer for SECTOR DATA
    lv_pSectorData = (U8*)malloc( NAND_SECTOR_SIZE );
    if(  lv_pSectorData == 0 )
    {  
      printf("  Error! Failed to allocate 'sector data'.\n");
      goto cleanup;
    }
    
    // Reserve a buffer for SPARE BCH DATA.
    lv_pSectorSpare = (U8*)malloc( lv_SectorBchSize );
    if(  lv_pSectorSpare == 0 )
    {
      printf("  Error! Failed to allocate 'sector spare'.\n");  
      goto cleanup;
    }    
  }
  
  // Execute one-time setup of BCH engine.
  BCH_Init( ipParams->EccType );
  
  // Setup page and sector iteration.
  lv_pSectorPtr = ipData;
  
  // FOR( all pages in the block )
  for( lv_Page = 0; lv_Page < ipParams->BlockSize; lv_Page++)
  {      
    // Reset buffer containing the spare area for this page.
    memset( lv_pPageSpare, 0xFF, ipParams->SpareSize );
    
    if( ipParams->BadBlock == TRUE )
    {
      // Mark block as 'bad' by setting the first 2 spare bytes to 0. 
      lv_pPageSpare[0] = 0x00;
      lv_pPageSpare[1] = 0x00;
    }      
    
    // Setup iterating pointers before processing this page.
    lv_pSparePtr = lv_pPageSpare + 2;
          
    // FOR( all usable SECTORS in a page )
    for( lv_Sector = 0; lv_Sector < lv_Sectors; lv_Sector++ )
    {
      if( ipParams->EccType != NAND_ECC_NONE )
      {
        //--------------------------------------------------
        // BCH Encoding.
        //--------------------------------------------------
        
        // Copy SECTOR data and (byte-)reverse the order.
        for( k=0; k < NAND_SECTOR_SIZE; k++)
          lv_pSectorData[k] = lv_pSectorPtr[ NAND_SECTOR_SIZE - 1 - k ];

        // Encode SECTOR.
        BCH_Encode( lv_pSectorData, lv_pSectorSpare, NAND_SECTOR_SIZE );

        // (Byte-)Reverse BCH result.
        for( k=0; k < lv_SectorBchSize; k++ )
          lv_pSparePtr[k] = lv_pSectorSpare[lv_SectorBchSize-1-k];
      
        //--------------------------------------------------
        // Include sector errors if necessary.
        //--------------------------------------------------
        
        //FOR( all correctable errors )
        for( i=0; i < ipParams->CorrectableCount; i++ )
        {        
          if( ipParams->CorrectableAddresses[i] == ( lv_Sectors * lv_Page + lv_Sector ) )
          {
            // Mark sector as done.
            ipParams->CorrectableAddresses[i] = 0xFFFFFFFF;
            
            // Generate correctable errors for this sector.
            NE_GenerateRandomErrors( lv_pSectorPtr, NAND_SECTOR_SIZE, lv_CorrectableErrors );
          }
        } //ENDFOR all correctable errors
  
        //FOR( all uncorrectable errors )
        for( i=0; i<ipParams->UnCorrectableCount; i++ )
        {
          if( ipParams->UnCorrectableAddresses[i] == ( lv_Sectors * lv_Page + lv_Sector )  )
          {
            // Mark sector as done.
            ipParams->CorrectableAddresses[i] = 0xFFFFFFFF;
            
            // Generate uncorrectable errors for this sector.
            NE_GenerateRandomErrors( lv_pSectorPtr, NAND_SECTOR_SIZE, lv_CorrectableErrors + 1 );
          }        
        } //ENDFOR all uncorrectable errors  
      
      }  

      // Store SECTOR DATA into FILE
      for( k=0; k < NAND_SECTOR_SIZE; k++ )
        fputc( lv_pSectorPtr[k], ipFile );

      // Update SECTOR pointers.
      lv_pSectorPtr += NAND_SECTOR_SIZE;
      lv_pSparePtr  += lv_SectorSpareSize; // 0, 14, 26
      
      // Display progress
      printf("  Page: %.3d/%.3d Sector: %d/%d\r",
        lv_Page+1, ipParams->BlockSize,
        lv_Sector+1, lv_Sectors ); 
    
      fflush(stdout);   
      
    } // ENDFOR( all SECTOR in a page )
    
    // Store PAGE SPARE.
    for( k=0; k < ipParams->SpareSize; k++ )
      fputc( lv_pPageSpare[k], ipFile );  
            
  } // ENDFOR( all pages in the block )

cleanup:
  
  // Clean up
  if( lv_pPageSpare != NULL )
    free( lv_pPageSpare );
  
  if( lv_pSectorSpare != NULL )
    free( lv_pSectorSpare );
  
  if( lv_pSectorData != NULL )
    free( lv_pSectorData );
  
  printf("\n");
  
  return NO_ERROR;  
}


/*---------------------------------- End of File -----------------------------*/



