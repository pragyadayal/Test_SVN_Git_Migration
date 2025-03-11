/*******************************************************************************
*
*    File Name:  bch_encoder.c
*     Revision:  1.0
*         Date:  August, 2006
*        Email:  nandsupport@micron.com
*      Company:  Micron Technology, Inc.
* 
*  Description:  Micron NAND BCH Encoder
*
*   References:
* 		  1. Error Control Coding, Lin & Costello, 2nd Ed., 2004
* 		  2. Error Control Codes, Blahut, 1983
**
*   Disclaimer   This software code and all associated documentation, comments or other
*  of Warranty:  information (collectively "Software") is provided "AS IS" without
*                warranty of any kind. MICRON TECHNOLOGY, INC. ("MTI") EXPRESSLY
*                DISCLAIMS ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
*                TO, NONINFRINGEMENT OF THIRD PARTY RIGHTS, AND ANY IMPLIED WARRANTIES
*                OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE. MTI DOES NOT
*                WARRANT THAT THE SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE
*                OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE.
*                FURTHERMORE, MTI DOES NOT MAKE ANY REPRESENTATIONS REGARDING THE USE OR
*                THE RESULTS OF THE USE OF THE SOFTWARE IN TERMS OF ITS CORRECTNESS,
*                ACCURACY, RELIABILITY, OR OTHERWISE. THE ENTIRE RISK ARISING OUT OF USE
*                OR PERFORMANCE OF THE SOFTWARE REMAINS WITH YOU. IN NO EVENT SHALL MTI,
*                ITS AFFILIATED COMPANIES OR THEIR SUPPLIERS BE LIABLE FOR ANY DIRECT,
*                INDIRECT, CONSEQUENTIAL, INCIDENTAL, OR SPECIAL DAMAGES (INCLUDING,
*                WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS INTERRUPTION,
*                OR LOSS OF INFORMATION) ARISING OUT OF YOUR USE OF OR INABILITY TO USE
*                THE SOFTWARE, EVEN IF MTI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
*                DAMAGES. Because some jurisdictions prohibit the exclusion or
*                limitation of liability for consequential or incidental damages, the
*                above limitation may not apply to you.
*
*                Copyright 2006 Micron Technology, Inc. All rights reserved.
*
* Rev  Author			Date		Changes
* ---  ---------------	----------	-------------------------------
* 1.0  ZS				08/07/2006	Initial release
*
*
/*******************************************************************************/

/*--------------------------------- Include Files ----------------------------*/
#include "romcode_types.h"
#include "bch_encoder.h"
#include "bch_global.c"
#include <string.h>

/*----------------------------------- Defines --------------------------------*/
// #define cmDebugInfo

/*--------------------------- Local Functions Declarations -------------------*/
STATUS bch_EncodeParallel( void );

/*------------------------------- Local Variables ----------------------------*/

/*------------------------------- Global Functions ---------------------------*/

/*------------------------------------------------------------------------------
* FUNCTION    : BCH_Init
*
* DESCRIPTION : 
*
* RETURNS     : NO_ERROR
*-----------------------------------------------------------------------------*/
STATUS BCH_Init( U8 iBitCorrection )
{
  if( iBitCorrection == 0 )
    return FAILED;
    
  //---------------------------------------------
  // Setup BCH code paramters
  //---------------------------------------------
  
  /* Galois field, GF, for code. */ 
  bch_MM = 13;
  
  /* Bit correction power of the code. */
  bch_TT = iBitCorrection;

  /* Code length. */
  bch_NN = (int)pow((double)2.0, (double)bch_MM) - 1;
  bch_ShortNN = bch_NN;
  
  /* Number of substreams to calculate in parallel. */
  bch_Parallel = 8;  
  
  // generate the Galois Field GF(2**mm)
  generate_gf() ;

  // Compute the generator polynomial and lookahead matrix for BCH code
  gen_poly() ;
  
  return NO_ERROR;  
}


/*------------------------------------------------------------------------------
* FUNCTION    : BCH_Encode
*
* DESCRIPTION : 
*
* RETURNS     : NO_ERROR
*-----------------------------------------------------------------------------*/
STATUS BCH_Encode(U8* ipInput, U8* ipOutput, U32 iLength )
{
  U32 i,j, bits; 
  int k;

  //--------------------------------------------------------
  // Pump data into the engine.
  //--------------------------------------------------------

  bits = 0;
  
  for( j=0; j<iLength; j++ )
  {
    for(i=0; i<8; i++)
    {
      // Store each bit.
      bch_Data[bits++]=( (*ipInput)>>i ) & 1;
    }
    
    // Grab previous byte.
    ipInput++;
  }

  // Number of message bits.
  bch_ShortKK = iLength * 8;
  bch_ShortNN = bch_ShortKK + bch_RR;

	//------------------------------
  // Encode!
	//------------------------------
	bch_EncodeParallel();

  //--------------------------------------------------------
  // Save remainder. 
  //--------------------------------------------------------
  
  j=0;
  
	for( k=0; k<bch_RR; k+=8 )
	{
    *ipOutput = 0;
    for( i=0; i<8; i++ )
    {
      *ipOutput |= (U8)( bch_BB[j*8+i] << i );
    }

    ipOutput++;
    j++;
  }

  return NO_ERROR;
}


/*-------------------------------- Local Functions ---------------------------*/


/*------------------------------------------------------------------------------
* FUNCTION    : bch_EncodeParallel
*
* DESCRIPTION : Parallel computation of n - k parity check bits.
*               Use lookahead matrix T_G_R.
*               The incoming streams are fed into registers from the right hand.
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
STATUS bch_EncodeParallel( void )
{
  int i, j, iii, Temp, bb_temp[cmMaxRR] ;
	int loop_count ;
	
	// Determine the number of loops required for parallelism.  
	loop_count = (int)ceil( bch_ShortKK / (double)bch_Parallel ) ;	
	
	// Serial to parallel data conversion
	for( i=0; i < bch_Parallel; i++ )
	{
	  for( j=0; j < loop_count; j++ )
		{
		  if( i + j * bch_Parallel < bch_ShortKK )
				bch_DataP[i][j] = bch_Data[ i+ j*bch_Parallel ];
			else
				bch_DataP[i][j] = 0;
		}
	}
	
	// Initialize the parity bits.
	memset( bch_BB, 0, bch_RR * sizeof(int) );
	
	// Compute parity checks
	// S(t) = T_G_R [ S(t-1) + M(t) ]
	// Ref: Parallel CRC, Shieh, 2001
	for (iii = loop_count - 1; iii >= 0; iii--)
	{
	  for (i = 0; i < bch_RR; i++)
			bb_temp[i] = bch_BB[i] ;
		
		for (i = bch_Parallel - 1; i >= 0; i--)
		{
			bb_temp[bch_RR - bch_Parallel + i] =
			  bb_temp[bch_RR - bch_Parallel + i] ^ bch_DataP[i][iii];
		}
		
		for( i=0; i < bch_RR; i++)		
		{
		  Temp = 0;
			
			for( j=0; j < bch_RR; j++)
				Temp = Temp ^ (bb_temp[j] * bch_TGR[i][j]);
			
			bch_BB[i] = Temp;
		}
	}
	
  return NO_ERROR;
}


