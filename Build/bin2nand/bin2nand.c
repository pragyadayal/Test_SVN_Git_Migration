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
*   Filename   :  bin2nand.c
*
*   Description:  
*_____________________________________________________________________________
*
*   History:
*   25-Jul-08    Martin Zarzycki
*                Creation
*_____________________________________________________________________________*/


/*--------------------------------- Include Files ----------------------------*/
#ifdef WIN32
#include "stdafx.h"
#endif

#include "romcode_types.h"

#include "nand.h"
#include "nand_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*----------------------------------- Defines --------------------------------*/

/*------------------------------- Local Constants ----------------------------*/

static const char LC_pVersion[] = "3.0";

/*------------------------------- Local Variables ----------------------------*/

/*--------------------------- Local Functions Declarations -------------------*/

void LF_PrintUsage( void );
void LF_PrintDisclaimer( const char *iVersion );      

/*------------------------------- Global Functions ---------------------------*/

int main(int argc, char **argv)
{
#ifdef WIN32
  errno_t fopen_error;
#endif
  NAND_Params_t lv_Params;
  
  FILE *lv_pFileInput;
  FILE *lv_pFileOutput;
  U8   *lv_pDataInput = 0;  
  U32   lv_Size;
  int i;
  
  BOOL  lv_SuppressDisclaimer = FALSE;

  /* nand parameters */
  lv_Params.PageSize  = 2048;
  lv_Params.BlockSize = 64;
  lv_Params.SpareSize = 64;
  
  lv_Params.CorrectableCount = 0;
  lv_Params.UnCorrectableCount = 0;  
  
  /* Default ECC algorithm is: Hamming */
  lv_Params.EccType = NAND_ECC_NONE;
  
  /* Default NAND type is: Raw */
  lv_Params.NandType = NAND_TYPE_RAW;
  
  /* flags */
  lv_Params.Verbose   = FALSE;
  lv_Params.BadBlock  = FALSE;
  lv_Params.BigSpare  = FALSE; 

#ifdef WIN32
  srand((unsigned int)time(NULL));
#else
  srand(times(NULL));
#endif

  //-------------------------------------------------------------------
  // check command syntaxt
  //-------------------------------------------------------------------
  if( argc <= 2 ) 
    goto bad_params;

  for( i=1; i < (argc-2); i++ )
  {
    //--------------------------------------
    // type
    //--------------------------------------
    if( strcmp(argv[i], "-type" ) == 0 )
    {
      if( i+1 == argc-2 )
      {
        printf("  Error! Missing value for '-type' option.\n");  
        goto bad_params;
      }
      
      if( strcmp( argv[i+1], "raw" ) == 0 )
      {
        lv_Params.NandType = NAND_TYPE_RAW;
      }
      else if( strcmp( argv[i+1], "one" ) == 0 )
      {
        lv_Params.NandType = NAND_TYPE_ONE;
      }
      else
      {
        printf("  Error! Wrong value for '-type' option.\n");
        goto bad_params;
      }
      
      i++;
    }
    
    //--------------------------------------
    // page
    //--------------------------------------
    else if(strcmp(argv[i],"-page")==0)
    {
      if( i+1 == argc-2 )
      {
        printf("  Error! Missing value for '-page' option.\n");    
        goto bad_params;
      }
      
      if( strcmp( argv[i+1], "512" ) == 0 )
      {
        lv_Params.PageSize = 512;
      }
      else if( strcmp( argv[i+1], "2048" ) == 0 ) 
      {
        lv_Params.PageSize = 2048;
      }
      else if( strcmp( argv[i+1], "4096" ) == 0 )
      {
        lv_Params.PageSize = 4096;
      }
      else if( strcmp( argv[i+1], "8192" ) == 0 )
      {
        lv_Params.PageSize = 8192;
      }
      else
      {
        printf("  Error! Wrong value for '-page' option.\n");
        goto bad_params;
      }
      i++;
    }
    //--------------------------------------
    // block
    //--------------------------------------
    else if(strcmp(argv[i],"-block")==0)
    {
      if((i+1)==(argc-2) )
      {
        printf("  Error! Missing value for '-block' option.\n");    
        goto bad_params;
      }
      
      lv_Params.BlockSize = atoi(argv[i+1]);
      i++;
    }
    //--------------------------------------
    // verbose
    //--------------------------------------
    else if((strcmp(argv[i],"-verbose")==0) ||
            (strcmp(argv[i],"-v")==0) )
    {
      lv_Params.Verbose = TRUE;
    }
    //--------------------------------------
    // correctable
    //--------------------------------------
    else if((strcmp(argv[i],"-correctable")==0) ||
            (strcmp(argv[i],"-corr")==0))
    {
      if((i+1)==(argc-2))
      {
        printf("  Error! Missing value for '-corr' option.\n");    
        goto bad_params;
      }

      lv_Params.CorrectableAddresses[lv_Params.CorrectableCount] = atoi(argv[i+1]);
      lv_Params.CorrectableCount++;
      i++;
    }
    //--------------------------------------
    // uncorrectable
    //--------------------------------------
    else if((strcmp(argv[i],"-uncorrectable")==0) ||
            (strcmp(argv[i],"-uncorr")==0))
    {
      if((i+1)==(argc-2))
      {
        printf("  Error! Missing value for '-uncorr' option.\n");  
        goto bad_params;
      }

      lv_Params.UnCorrectableAddresses[lv_Params.UnCorrectableCount] = atoi(argv[i+1]);
      lv_Params.UnCorrectableCount++;
      i++;
    }
    //--------------------------------------
    // spare
    //--------------------------------------
    else if(strcmp(argv[i],"-spare") ==0 )
    {
      if((i+1)==(argc-2))
      {
        printf("  Error! Missing value for '-spare' option.\n");  
        goto bad_params;
      }
      
      lv_Params.SpareSize = atoi(argv[i+1]);
      i++;
    } 
    //--------------------------------------
    // bigspare
    //--------------------------------------
    else if(strcmp(argv[i],"-bigspare")==0)
    {
      lv_Params.BigSpare = TRUE;
    }
    //--------------------------------------
    // badblock
    //--------------------------------------
    else if(strcmp(argv[i],"-badblock")==0)
    {
      lv_Params.BadBlock = TRUE;
    }
    //--------------------------------------
    // ECC (BCH)
    //-------------------------------------- 
    else if(strcmp(argv[i],"-bch")==0)
    {
      if( i+1 == argc-2 )
      {
        printf("  Error! Missing value for '-bch' option.\n");    
        goto bad_params;
      }
      
      if(strcmp(argv[i+1],"8")==0)
      {
        lv_Params.EccType = NAND_ECC_BCH8;
      }
      else
      {
        printf("  Error! Wrong value for '-bch' option.\n");
        goto bad_params;
      }  
      i++;  
    }
    //--------------------------------------
    // suppress
    //--------------------------------------
    else if( strcmp( argv[i], "-suppress" ) == 0 )
    {
      lv_SuppressDisclaimer = TRUE;
    }
    else
    {
      printf("  Error! Unrecognized option: %s\n", argv[i] );
      goto bad_params;
    }
  }
  
  // print software version info and disclaimer
  if( lv_SuppressDisclaimer == FALSE )
    LF_PrintDisclaimer( LC_pVersion );

  printf("\n");
  
  /* NAND type */
  switch( lv_Params.NandType )
  {
    case NAND_TYPE_RAW:
      printf("  NAND type.................. RAW\n" ); break;
    case NAND_TYPE_ONE:
      printf("  NAND type.................. (FLEX)ONENAND\n" ); break;
    default:
      printf("  NAND type.................. unknown!\n" );
      goto bad_params;
  }
    
  /* page size */
  printf("  NAND page size............. %d\n",lv_Params.PageSize);
  
  /* spare size */
  if( lv_Params.BigSpare )
    lv_Params.SpareSize = lv_Params.PageSize;
   
  printf("  NAND spare area (bytes).... %d\n",lv_Params.SpareSize);
   
  /* pages per block */
  printf("  NAND block size (pages).... %d\n",lv_Params.BlockSize);
  
  /* correctable errors */
  printf("  Correctable sectors........" );
  for( i=0; i < lv_Params.CorrectableCount; i++ )
    printf(" %d", lv_Params.CorrectableAddresses[i]);
  printf("\n");
  
  /* uncorrectable errors */
  printf("  Uncorrectable sectors......" );
  for( i=0; i < lv_Params.UnCorrectableCount; i++ )
    printf(" %d", lv_Params.UnCorrectableAddresses[i]);
  printf("\n");  
  
  /* source and Destination */
  printf("  Source file................ %s\n",argv[argc-2]);
  printf("  Output file................ %s\n",argv[argc-1]);

  //-------------------------------------------------------------------
  // read input image
  //-------------------------------------------------------------------
  
/* open input file */
#ifdef WIN32
  /* In WIN32, fopen_s() is "safe", fopen() is not */
  fopen_error = fopen_s(&lv_pFileInput, argv[argc-2],"r");
#else
  lv_pFileInput = fopen(argv[argc-2],"r");
#endif

  if( lv_pFileInput == NULL )
  {
    printf("  Error! Can not open file : %s\n",argv[argc-2]);
    return 0;
  }
  
  /* reserve memory block */
  lv_pDataInput = (U8*)malloc( lv_Params.BlockSize * lv_Params.PageSize );
  
  /* init memory block */
  memset( lv_pDataInput, 0, ( lv_Params.BlockSize * lv_Params.PageSize ) );
 
  /* read input file into memory block */
  lv_Size = fread( lv_pDataInput, 1, (lv_Params.BlockSize*lv_Params.PageSize), lv_pFileInput);
  
  /* close input file */
  fclose(lv_pFileInput);

  printf("  Input image size........... %d\n", lv_Size );
  printf("  Output image size.......... %d\n", lv_Params.BlockSize*lv_Params.PageSize);

  //-------------------------------------------------------------------
  // create image
  //-------------------------------------------------------------------

#ifdef WIN32
  /* In WIN32, fopen_s() is "safe", fopen() is not */
  fopen_error = fopen_s(&lv_pFileOutput, argv[argc-1],"wb");
#else
  lv_pFileOutput = fopen(argv[argc-1],"wb");
#endif

  if(lv_pFileOutput == NULL)
  {
     printf(" Error! Can not open file : %s\n",argv[argc-1]);
     goto end_1;
  }

  printf("  Generating image...\n");
  ni_CreateImage( &lv_Params, lv_pDataInput, lv_pFileOutput ); 

  fclose(lv_pFileOutput);
  printf("  Done.\n");

end_1:
  free( lv_pDataInput );
  return 0;

bad_params:  
  LF_PrintUsage();
  return 1;
}

/*-------------------------------- Local Functions ---------------------------*/

/*------------------------------------------------------------------------------
* FUNCTION    : lf_PrintDisclaimer
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
void LF_PrintDisclaimer( const char *iVersion )
{
  printf("**********************************************************************************\n");
  printf("       (C) 2013 TEXAS INSTRUMENTS. All rights reserved                          \n");
  printf("                  Image Formating Tool for NAND Memories                        \n");
  printf("                                                                                \n");
  printf("**********************************************************************************\n");
}
//ENDFUNCTION lf_PrintDisclaimer  


/*------------------------------------------------------------------------------
* FUNCTION    : lf_PrintUsage
*
* DESCRIPTION : 
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
void LF_PrintUsage()
{
  printf("\n");
  printf("  Usage: bin2nand [options] input output\n");
  printf("  Options: \n");
  printf("    -type raw|one        - NAND type, default: raw\n");
  printf("    -page 512|2048|...   - Page size, default: 2048\n");
  printf("    -spare 16|64|218|... - Size of the page spare area, default: 64\n");
  printf("    -block 16|32|64|128  - Block size (number of pages), default: 64\n");
  printf("    -verbose             - Print ecc's. \n");
  printf("    -corr <sector>       - Generates correctable error(s).\n");
  printf("    -uncorr <sector>     - Generates uncorrectable error(s).\n");
  printf("    -bigspare            - Spare area size = page size (Denali).\n");
  printf("    -badblock            - Marks blocks as invalid.\n");
  printf("    -bch 8               - Use BCH as ECC algorithm.\n");
  printf("                           Error correction capability is 8 bits only.\n");
  printf("                           If not set, then no ECC is used.\n");
  printf("\n");
}
//ENDFUNCTION lf_PrintUsage  

/*---------------------------------- End of File -----------------------------*/



