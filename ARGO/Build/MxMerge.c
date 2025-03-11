/****************************************************************************
 *
 *     MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
 *     MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
 *     MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
 *     MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
 *     MM       MM   AA       AA       TT      RR  RR       II      XX XX
 *     MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
 *
 *     Company      : Matrix Comsec Pvt Ltd, Baroda, India
 *     Owner        : Ronak Shah
 *     File         : MxMerge.c
 *     Description  : This program is used to concate multiple files 
 *                    by location for production image and
 *		      		  currently specific to Matrix ARGO product.
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/****************************************************************************/

#ifndef SUCCESS 
#define SUCCESS  1
#endif

#ifndef FAIL
#define FAIL     0
#endif

#ifndef TRUE
#define TRUE     1
#endif

#ifndef FALSE
#define FALSE     0
#endif

#define FILE_BLOCK_SIZE  102400  /* 100KB */

/* Image files are with ecc */
/* size in bytes */
#define UBOOT_SPL_SIZE						135168			//   1		*	132 	*   1024			
#define UBOOT_SPL_BACKUP1_SIZE				135168			// 	 1		*	132  	*	1024
#define UBOOT_SPL_BACKUP2_SIZE				135168			// 	 1		*	132  	*	1024 			
#define UBOOT_SPL_BACKUP3_SIZE				135168			// 	 1		*	132  	*	1024		
#define UBOOT_SIZE							946176			// 	 7		*	132  	*	1024		
#define UBOOT_BACKUP_SIZE					946176			// 	 7		*	132  	*	1024  		
#define KERNEL_SIZE							4730880			// 	 35		*	132  	*	1024	
#define ROOTFS_SIZE							540672000		// 	 4000	*	132  	*	1024		

/****************************************************************************/

typedef unsigned char bool;

typedef struct imgInfo
{

    char imgName[50];
    int  imgSize;

}IMGINFO;

/****************************************************************************/

int 	pagenumCalculator(int srcFilesize);
bool 	createImage(int, char**);
bool 	validateSize(int, char**);
void 	closeFile(int, int);
int 	generateEcc(char * inputFilename,char * outputFilename,int pagenum);

/****************************************************************************/

int outputFileSize;
IMGINFO img[8] = {
			"MLO",	UBOOT_SPL_SIZE, \
			"MLO_backup1",	UBOOT_SPL_BACKUP1_SIZE, \
			"MLO_backup2",		UBOOT_SPL_BACKUP2_SIZE, \
			"MLO_backup3",		UBOOT_SPL_BACKUP3_SIZE, \
			"uboot",		UBOOT_SIZE, \
			"uboot_backup",		UBOOT_BACKUP_SIZE, \
			"kernel",		KERNEL_SIZE, \
			"ubifs",		ROOTFS_SIZE
		 };

/****************************************************************************/

int main(int argc, char **argv)
{

    char    cmd[100];
	
    if(argc < 5)
    {
        printf("\nUsage:%s <file1>  <location in hex> <file2> <location in hex>... <outputFile> <outputFile size in MB>\n", argv[0]);
        return -1;
    }

    if( ((argc-5) % 2) != 0 )
    {
        printf("\nUsage:%s <file1>  <location in hex> <file2> <location in hex>... <outputFile> <outputFile size in MB>\n", argv[0]);
        return -1;
    }

    outputFileSize = atoi(argv[argc-1]);
    
    if(!(outputFileSize > 0))
    {
        printf("\noutputFile size is not valid.\n");
        return -1;
    }

    outputFileSize = outputFileSize * 1024 * 1024;

    printf("\nstart validating args...\n");

    if(validateSize(argc, argv) == FAIL)
    {
        return -1;
    }

    printf("validation successful.\n");

    printf("start generating image...\n");

    if(createImage(argc,argv) == FAIL)
    {
        sprintf(cmd, "rm -f %s", argv[argc-2]);
        system(cmd);
        printf("Fail to generate image.\n");
        return -1;
    }
	
    printf("Image generated successfully.\n");
    /* The next two lines removes the unnecessary generated temp file in the folder */
	sprintf(cmd, "rm -f temp");
    system(cmd);
    return 0;
}

/****************************************************************************
 * 
 *  Name: validateSize
 *  Param:
 *        IN: int argc
 *            char **argv
 *        OUT:
 *  Returns: FAIL/SUCCESS
 *  Description: This function checks for given size of output file.
 * 
 ****************************************************************************/

bool validateSize(int argc, char **argv)
{

    int    sFd;
    int    srcFileIdx = argc-4;
    int    srcFileSize;
    int    totalSize;
    int    offSet;
    int    idx;
    char   *endPtr;    

    sFd = open(argv[srcFileIdx], O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);

    if(sFd < 0)
    {
        printf("%s...\n", argv[srcFileIdx]);
        perror("open");
        return FAIL;
    }

    //  printf("%s opened successfully.\n", argv[srcFileIdx]);

    srcFileSize = lseek(sFd, 0 , SEEK_END);
    close(sFd);

    for(idx=0; idx<8; idx++)
    {
        if( strstr( argv[srcFileIdx], img[idx].imgName ) != NULL )
        {
            break;
        }
    }

    if(idx == 8)
    {
        printf("File name is not valid, Should contain "\
		"{MLO/MLO_backup1/MLO_backup2/"\
		"MLO_backup3/uboot/uboot_backup/"\
		"kernel/ubifs} keyword.\n");

        return FAIL;
    }

    if(srcFileSize > img[idx].imgSize)
    {
        printf("%s file size is more than %d bytes.\n", argv[srcFileIdx], img[idx].imgSize);
        return FAIL;
    }

    if( strstr( argv[srcFileIdx+1], "0x" ) != argv[srcFileIdx+1])
    {
        printf("Location specified for %s file is not in hex.\n", argv[srcFileIdx]);
        return FAIL;
    }

    offSet = strtol(argv[srcFileIdx+1], &endPtr, 16);
    totalSize = offSet + srcFileSize;

    if(totalSize > outputFileSize)
    {
        printf("%s file size not sufficient, Actual size: %d bytes, Required %d bytes.\n", argv[argc-2], outputFileSize, totalSize);
        return FAIL;
    }

    return SUCCESS;
}

void closeFile(int sFd, int outFd)
{
    close(sFd);
    close(outFd);
}

/************************************************************************************
 * 
 *  Name: createImage
 *  Param:
 *        IN: int argc
 *            char **argv
 *        OUT:     
 *  Returns: FAIL/SUCCESS
 *  Description: This function merges all given source files to given output file.
 * 
 ************************************************************************************/

bool createImage(int argc, char **argv)
{

    int     outFd;
    int     sFd;
    int     readSize;
    int     srcFileIdx = 1;
    int     srcFileSizeIdx = 0;
    int     offSet;
    int     curPos;
    int     blankBufSize;
    int     srcFileCount = (argc-3)/2;
    int     srcFileSize;
    int     idx;
    char    *endPtr;
    char    imgBuf[FILE_BLOCK_SIZE];
    char    *blankBuf;
    int		pagenum;
	int		check;
	
    outFd = open(argv[argc-2], O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    if(outFd < 0)
    {
        printf("%s...\n", argv[argc-2]);
        perror("open");
        return FAIL;
    }

    printf("%s opened successfully.\n", argv[argc-2]);

    do
    {
        sFd = open(argv[srcFileIdx], O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
		
        if(sFd < 0)
        {
            printf("%s...\n", argv[srcFileIdx]);
            perror("open");
            close(outFd);
            return FAIL;
        }

        printf("%s opened successfully.\n", argv[srcFileIdx]);

        srcFileSize = lseek(sFd, 0, SEEK_END);
        printf("%s file size is %d bytes.\n", argv[srcFileIdx], srcFileSize);
        pagenum=pagenumCalculator(srcFileSize);
        
        lseek(sFd, 0, SEEK_SET);

        for(idx=0; idx<8; idx++)
        {
            if( strstr( argv[srcFileIdx], img[idx].imgName ) != NULL )
            {
                break;
            }
        }

        if(idx == 8)
        {
			printf("File name is not valid, Should contain "\
			"{MLO/MLO_backup1/MLO_backup2/"\
			"MLO_backup3/uboot/uboot_backup/"\
			"kernel/ubifs} keyword.\n");

            closeFile(sFd,outFd);
            return FAIL;
        }

	printf("Image: %s, Size: %d\n", img[idx].imgName, img[idx].imgSize);

        if(srcFileSize > img[idx].imgSize)
        {
            printf("%s file size is more than %d bytes.\n", argv[srcFileIdx], img[idx].imgSize);
            closeFile(sFd,outFd);
            return FAIL;
        }    

        if( strstr( argv[srcFileIdx+1], "0x" ) != argv[srcFileIdx+1])
        {
            printf("Location specified for %s file is not in hex.\n", argv[srcFileIdx]);
            closeFile(sFd,outFd);
            return FAIL;
        }

        offSet = strtol(argv[srcFileIdx+1], &endPtr, 16);
        curPos = lseek(outFd, 0, SEEK_CUR);

        if(offSet > curPos)
        {
            blankBufSize = offSet - curPos;
        }
        else if(offSet == curPos)
        {
            blankBufSize = 0;
        }
        else
        {
            printf("%s file has invalid location, %s (Overwrite situation)\n", argv[srcFileIdx], argv[srcFileIdx + 1]);
            closeFile(sFd,outFd);
            return FAIL;
        }
		
        printf("Blank memory is %d bytes.\n", blankBufSize);

        if(blankBufSize)
        {
            blankBuf = (char *)malloc(blankBufSize);
            if(blankBuf)
            {
                memset((void *)blankBuf, 0XFF, blankBufSize);
                write(outFd, (void *)blankBuf, blankBufSize);
                free(blankBuf);
                blankBuf = NULL;
            }
            else
            {
                printf("Insufficient heap memory. Required memory: %d bytes.\n", blankBufSize);
                closeFile(sFd, outFd);
                return FAIL;
            }
        }
	check= generateEcc(argv[srcFileIdx], "temp", pagenum);
	
	if(check==-1)
		{
		    perror("system");
		    return FAIL;
		}
	
	close(sFd);
	sFd= open("temp", O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
		
        do
        {
            readSize = read(sFd, (void *)imgBuf, FILE_BLOCK_SIZE);
            if(readSize > 0)
            {
                if(write(outFd, (void *)imgBuf, readSize) != readSize)
                {
                    closeFile(sFd,outFd);
                    return FAIL;            
                }
            }
        }while(readSize != 0);

        printf("%s successfully written to %s.\n\n", argv[srcFileIdx], argv[argc-2]);
        close(sFd);
        srcFileCount--;
        srcFileIdx = srcFileIdx+2;
        srcFileSizeIdx++;

    }while(srcFileCount > 0);

    curPos = lseek(outFd, 0, SEEK_CUR);
    blankBufSize = outputFileSize - curPos;
    printf("Blank memory is %d bytes.\n", blankBufSize);

    if(blankBufSize)
    {
        blankBuf = (char *)malloc(blankBufSize);
        if(blankBuf)
        {
            memset((void *)blankBuf, 0XFF, blankBufSize);
            write(outFd, (void *)blankBuf, blankBufSize);
            free(blankBuf);
            blankBuf = NULL;
        }
        else
        {
            printf("Insufficient heap memory. Required memory: %d bytes.\n", blankBufSize);
            close(outFd);
            return FAIL;
        }
    }

    close(outFd);
    return SUCCESS;
}

/************************************************************************************
 * 
 *  Name: pagenumcalculator
 *  Param:
 *        IN: int srcfilesize
 *    
 *        OUT:     
 *  Returns: pagenum
 *  Description: This function calculates no of pages from the given input source file
 * 
 ************************************************************************************/


int pagenumCalculator(int srcFileSize)
{	
    int pagenum;
	
    if(srcFileSize % 2048==0)
	{
	    pagenum=srcFileSize/2048;
	}
    else
	{
	    pagenum =(srcFileSize/2048)	+ 1;
	}
	
return pagenum;

}


/************************************************************************************
 * 
 *  Name: generateEcc
 *  Param:
 *        IN: char * inputFilename 
 *            char * outputFilename
 * 			  int pagenum
 *        OUT:     
 *  Returns:  
 * 				ret
 *  Description: This function generates ecc file of the given input file.
 * 
 ************************************************************************************/
 
 
int generateEcc(char * inputFilename,char * outputFilename,int pagenum)
{	
    char cmd[300];
    int ret;
	
    sprintf(cmd, "./bin2nand -block %d -verbose -bch 8 %s %s",pagenum,inputFilename,outputFilename );
    ret=system(cmd);
    
    return ret;		
    
}


