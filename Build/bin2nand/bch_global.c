/*******************************************************************************
*
*    File Name:  bch_global.c
*     Revision:  1.0
*         Date:  August, 2006
*        Email:  nandsupport@micron.com
*      Company:  Micron Technology, Inc.
*
*  Description:  Micron NAND BCH Global Package
*
*     Function:   1. Create Galois Field
*		  2. Create Generator Polynomial
*		  3. Create bch_Parallel Generator Polynomial 
*
*   References: 
* 		  1. Error Control Coding, Lin & Costello, 2nd Ed., 2004
* 		  2. Error Control Codes, Blahut, 1983
* 		  3. bch_Parallel CRC, Shieh, 2001
*
**
*   Disclaimer   This software code and all associated documentation, cobch_MMents or other 
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
*
* Rev  Author			Date		Changes
* ---  ---------------	----------	-------------------------------
* 1.0  ZS				08/07/2006	Initial release
* 
* 
*******************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Active debug output. */
// #define cmDebugInfo

#define cmMaxData     4096
#define cmMaxMM       14        /* Dimension of Galoise Field */
#define cmMaxNN       16384     /* Length of codeword, n = 2**m - 1 */
#define cmMaxTT       20        /* Number of errors that can be corrected */
#define cmMaxKK       16384     /* Length of information bit, KK = NN - RR  */
#define cmMaxRR       280		    /* Number of parity checks, RR = deg[g(x)] */
#define cmMaxParallel 32	      /* Number of parallel encoding/syndrome computations */

/* BCH code parameters */
static int bch_MM, bch_NN, bch_KK, bch_TT, bch_RR;

/* Parity checks */
static int bch_BB[cmMaxRR];

/* Shortened BCH code */
static int bch_ShortNN, bch_ShortKK, bch_Parallel;	

/* Galois field. */
static int bch_P[cmMaxMM + 1];
static int bch_AlphaTo[cmMaxNN];
static int bch_IndexOf[cmMaxNN];

/* Generator polynomial. */
static int bch_GG[cmMaxRR];

/* bch_Parallel lookahead table. */
static int bch_TG[cmMaxRR][cmMaxRR];
static int bch_TGR[cmMaxRR][cmMaxRR];
static int bch_TempTGR[cmMaxRR][cmMaxRR];

/* Info and results. */
static int bch_Data[cmMaxKK];
static int bch_DataP[cmMaxParallel][cmMaxKK];
static int bch_Recd[cmMaxNN];

/*------------------------------------------------------------------------------
* FUNCTION    : generate_gf
*
* DESCRIPTION : Generate GF(2**MM) from the primitive polynomial p(X) in p[0]..p[MM]
*               The lookup table looks like:  
*               index -> polynomial form:   bch_AlphaTo[ ] contains j = alpha**i;
*               polynomial form -> index form:  IndexOf[j = alpha**i] = i
*               AlphaTo[1] = 2 is the primitive element of GF(2**bch_MM)
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
static void generate_gf()
{
  int i;
	int mask ;	// Register states
	
	// Primitive polynomials
  for (i = 1; i < bch_MM; i++)
		bch_P[i] = 0;
	
	bch_P[0] = bch_P[bch_MM] = 1;
	
	if      (bch_MM == 2)   bch_P[1] = 1;
	else if (bch_MM == 3)   bch_P[1] = 1;
	else if (bch_MM == 4)   bch_P[1] = 1;
	else if (bch_MM == 5)	  bch_P[2] = 1;
	else if (bch_MM == 6)	  bch_P[1] = 1;
	else if (bch_MM == 7)	  bch_P[1] = 1;
	else if (bch_MM == 8) 	bch_P[4] = bch_P[5] = bch_P[6] = 1;
	else if (bch_MM == 9)	  bch_P[4] = 1;
	else if (bch_MM == 10)	bch_P[3] = 1;
	else if (bch_MM == 11)	bch_P[2] = 1;
	else if (bch_MM == 12)	bch_P[3] = bch_P[4] = bch_P[7] = 1;
	else if (bch_MM == 13)	bch_P[1] = bch_P[3] = bch_P[4] = 1;
	else if (bch_MM == 14)	bch_P[2] = bch_P[4] = bch_P[6] = bch_P[7] = bch_P[8] = 1;	// 41D5
	else if (bch_MM == 15)	bch_P[1] = 1;
	else if (bch_MM == 16)	bch_P[2] = bch_P[3] = bch_P[5] = 1;
	else if (bch_MM == 17)	bch_P[3] = 1;
	else if (bch_MM == 18)	bch_P[7] = 1;
	else if (bch_MM == 19)	bch_P[1] = bch_P[5] = bch_P[6] = 1;
	else if (bch_MM == 20)	bch_P[3] = 1;
	
#ifdef cmDebugInfo
	fprintf(stderr, "# primitive polynomial:\n");
		
	for (i = 0; i <= bch_MM; i++) 
	  fprintf(stderr, " %d", bch_P[i]);
		
  fprintf(stderr, "\n\n");
#endif
	
	// Galois field implementation with shift registers
	// Ref: L&C, Chapter 6.7, pp. 217
	mask = 1;
	
	bch_AlphaTo[bch_MM] = 0 ;
	for (i = 0; i < bch_MM; i++)
	{
	  bch_AlphaTo[i] = mask ;
		bch_IndexOf[bch_AlphaTo[i]] = i ;
		
		if (bch_P[i] != 0)
			bch_AlphaTo[bch_MM] ^= mask ;
		
		mask <<= 1 ;
	}
	
	bch_IndexOf[bch_AlphaTo[bch_MM]] = bch_MM ;
	mask >>= 1 ;
	for (i = bch_MM + 1; i < bch_NN; i++)
	{
	  if (bch_AlphaTo[i-1] >= mask)
			bch_AlphaTo[i] = bch_AlphaTo[bch_MM] ^ ((bch_AlphaTo[i-1] ^ mask) << 1) ;
		else
		  bch_AlphaTo[i] = bch_AlphaTo[i-1] << 1 ;
		
		bch_IndexOf[bch_AlphaTo[i]] = i ;
	}
	bch_IndexOf[0] = -1 ;
}

/*------------------------------------------------------------------------------
* FUNCTION    : gen_poly
*
* DESCRIPTION : Compute generator polynomial of the t-error correcting Binary BCH code 
*               g(x) = LCM{M_1(x), M_2(x), ..., M_2t(x)},
*               where M_i(x) is the minimal polynomial of alpha^i by cyclotomic cosets
*
* RETURNS     : -
*-----------------------------------------------------------------------------*/
static void gen_poly()
{
  // Roots of generator polynomial
  int gen_roots[cmMaxNN + 1];
  int gen_roots_true[cmMaxNN + 1];
 
	int i, j, iii, jjj, Temp ;
	
	// Initialization of gen_roots
	for( i = 0; i <= bch_NN; i++) 
	{
	  gen_roots_true[i] = 0;
		gen_roots[i] = 0;
	}

	// Cyclotomic cosets of gen_roots
  for (i = 1; i <= 2*bch_TT ; i++)
	{
	  for (j = 0; j < bch_MM; j++) 
		{
		  Temp = ((int)pow((double)2.0, (double)j) * i) % bch_NN;
			gen_roots_true[Temp] = 1;
		}
	}
	
  bch_RR = 0;		// Count the number of parity check bits
  for (i = 0; i < bch_NN; i++) 
	{
	  if (gen_roots_true[i] == 1) 
		{
		  bch_RR++;
			gen_roots[bch_RR] = i;
		}
	}
	
	bch_KK = bch_NN - bch_RR;
	
	// Compute generator polynomial based on its roots
	bch_GG[0] = 2 ;	// g(x) = (X + alpha) initially
	bch_GG[1] = 1 ;
	for (i = 2; i <= bch_RR; i++)
	{
	  bch_GG[i] = 1 ;
		for (j = i - 1; j > 0; j--)
		if (bch_GG[j] != 0)  
			bch_GG[j] = bch_GG[j-1]^ bch_AlphaTo[(bch_IndexOf[bch_GG[j]] + bch_IndexOf[bch_AlphaTo[gen_roots[i]]]) % bch_NN] ;
		else 
			bch_GG[j] = bch_GG[j-1] ;
		bch_GG[0] = bch_AlphaTo[(bch_IndexOf[bch_GG[0]] + bch_IndexOf[bch_AlphaTo[gen_roots[i]]]) % bch_NN] ;
	}
	
#ifdef cmDebugInfo
  printf("# generator polynomial:\n"); 
  
  c = 0;
	for( i=0; i <= bch_RR; i++ )
	{
	  if( i%8 == 0 )
	  {
	    printf( "%X", c );
	    c = 0;
	  }
	  	  
	  c |= ( bch_GG[i] << (i%8) );
	}
	printf( "\n\n" );
#endif

	// for parallel encoding and syndrome computation max parallalism is bch_RR
	if (bch_Parallel > bch_RR)
		bch_Parallel = bch_RR ;

	// Construct parallel lookahead matrix T_g, and T_g**r from gg(x)
	// Ref: bch_Parallel CRC, Shieh, 2001
	for (i = 0; i < bch_RR; i++)
	{
	  for (j = 0; j < bch_RR; j++)
			bch_TG[i][j] = 0;
	}

	for (i = 1; i < bch_RR; i++)
		bch_TG[i][i-1] = 1 ;

	for (i = 0; i < bch_RR; i++)
		bch_TG[i][bch_RR - 1] = bch_GG[i] ;

	for (i = 0; i < bch_RR; i++)
	{
	  for (j = 0; j < bch_RR; j++)
			bch_TGR[i][j] = bch_TG[i][j];
	}
	
	// Compute T_G**R Matrix
	for (iii = 1; iii < bch_Parallel; iii++)
	{
	  for (i = 0; i < bch_RR; i++)
		{
		  for (j = 0; j < bch_RR; j++)
			{
			  Temp = 0;
				for (jjj = 0; jjj < bch_RR; jjj++)
					Temp = Temp ^ bch_TGR[i][jjj] * bch_TG[jjj][j];
				
				bch_TempTGR[i][j] = Temp;
			}
		}
		
		for (i = 0; i < bch_RR; i++)
		{
		  for (j = 0; j < bch_RR; j++)
				bch_TGR[i][j] = bch_TempTGR[i][j];
		}
	}
	
}
