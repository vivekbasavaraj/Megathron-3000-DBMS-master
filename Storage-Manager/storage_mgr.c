#include "dberror.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "storage_mgr.h"


void initStorageManager (void)
{

}

/*
 * Creates a new one page file with given file name and fills it with '\0' bytes.
 */
RC createPageFile (char *fileName)
{

	FILE * fptr;
	/*
	 * Creating an empty file
	 * If file with the same name already exists, its contents are discarded and the file is treated as a new empty file.
	 */
	fptr=  fopen(fileName,"wb");
	/*
	 * Checking if file is successfully created
	 * If yes, filling up file with '\0'
	 * If not, returning error message
	 * Note : Since there is no appropriate message given in template returning relatively similar message File not found.
	 */
	if(fptr == NULL)
		return RC_FILE_NOT_FOUND;
	else
	{
	    int i=0;
		while(i<PAGE_SIZE)
		{
			fprintf(fptr,"%c",'\0');
			i++;
		}
		fclose(fptr);
	}
	return RC_OK;
}
/*
 * Opens an existing page file and initializing file handle for further use.
 */
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {

	FILE *fptr;
    /*
     * Opens a file for input operations.
     */
	fptr = fopen(fileName,"r+b");
	/*
     * Checks if file exists with given name
     * If yes, initializing file handle with all file related information
     * If not, returning appropriate file not found error message
     */
	if(fptr == NULL)
        return RC_FILE_NOT_FOUND;
    else
    {
		fHandle->fileName = fileName;
		fHandle->curPagePos = 0;
		if(fseek(fptr,0,SEEK_END)==0);
		{
		    fHandle->totalNumPages = ftell(fptr)/PAGE_SIZE;
            fHandle->mgmtInfo = fptr ;
            fclose(fptr);
            return RC_OK;
		}

	}
}

/*
 * Closes an open page file
 */
RC closePageFile (SM_FileHandle *fHandle)
{
    /*
     * Checks if file handler is already initialized
     * If yes, closing a file and removing file handle references
     * If not, returning appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;
	else
	{
        FILE * fptr;
		fptr = fopen(fHandle->fileName,"rb");
		if(fclose(fptr)==0)
			return RC_OK;
		else
			return RC_FILE_NOT_FOUND;
	}
}
/*
 * Deletes a page file.
 */
RC destroyPageFile (char *fileName)
{
    /*
     * Removing a file with given name if exists, returning error message otherwise.
     */
	if(remove(fileName) == 0)
	{
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}


/*
 * Reads given page number block from file and stores its content into page handle memory pointer
 */
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	/*
     * Checks if file handler is already initialized
     * If yes, reads block and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

    else if(fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;
	else
	{
		if(pageNum > fHandle->totalNumPages) // Checks if given page number exists in pointed file
			return RC_READ_NON_EXISTING_PAGE;
		else
		{
		    FILE * fptr;
			fptr = fopen(fHandle->fileName,"rb");
			if(fseek(fptr,PAGE_SIZE*pageNum,SEEK_SET) == 0)
			{
				fread(memPage,PAGE_SIZE,1,fptr);
				fHandle->curPagePos = pageNum;
				fclose(fptr);
				return RC_OK;
			}
		}
	}
}

/*
*Return the current page position of the file.
*/
int getBlockPos (SM_FileHandle *fHandle)
{
	/*
     * Checks if file handler is already initialized
     * If yes, get the current page position and return it
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
        FILE * fptr;
	    fptr = fopen(fHandle->fileName,"rb");
		int blockPos = fHandle->curPagePos;
		fclose(fptr);
		return blockPos;
	}
}
/*
 * Reads first page block from file and stores its content into page handle memory pointer
 */
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, reads first block and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

    else if(fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;
	else
	{
	    FILE * fptr;
		fptr = fopen(fHandle->fileName,"r");
		if(fseek(fptr,0,SEEK_SET)==0)
        {
            fread(memPage,PAGE_SIZE,1,fptr);
			fHandle->curPagePos = (ftell(fptr)/PAGE_SIZE) -1;
			fclose(fptr);
			return RC_OK;
        }

	}
}

/*
 * Reads immediate previous block from file and stores its content into page handle memory pointer
 */
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	/*
     * Checks if file handler is already initialized
     * If yes, reads previous block and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->curPagePos >= 1)
	{
	    FILE * fptr;
		fptr = fopen(fHandle->fileName,"rb");
		if(fseek(fptr,PAGE_SIZE * ((fHandle->curPagePos)-1),SEEK_SET)==0)
        {
            fread(memPage,PAGE_SIZE,1,fptr);
            fHandle->curPagePos = ((fHandle->curPagePos)-1);
            fclose(fptr);
            return RC_OK;
        }

	}
	else
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
}
/*
 * Reads current block from file and stores its content into page handle memory pointer
 */
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, reads current block and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
	    FILE * fptr;
		fptr = fopen(fHandle->fileName,"rb");
		if(fseek(fptr,(PAGE_SIZE*(fHandle->curPagePos)),SEEK_SET) == 0)
		{
			fread(memPage,PAGE_SIZE,1,fptr);
			fclose(fptr);
			return RC_OK;
		}
		fclose(fptr);
	}
}
/*
 * Reads next block from file and stores its content into page handle memory pointer
 */
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, reads next block and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
	    FILE * fptr;
		fptr = fopen(fHandle->fileName,"rb");
		if(fseek(fptr,(PAGE_SIZE*(fHandle->curPagePos)+1)-1,SEEK_SET) == 0)
		{
			fread(memPage,PAGE_SIZE,1,fptr);
			fHandle->curPagePos = (ftell(fptr)/PAGE_SIZE)-1;
			fclose(fptr);
			return RC_OK;
		}
		fclose(fptr);
	}
}
/*
 * Reads last block from file and stores its content into page handle memory pointer
 */
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, reads last block of file and stores into memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
	    FILE * fptr;
	    fptr = fopen(fHandle->fileName,"rb");
		if(fseek(fptr,(PAGE_SIZE * (fHandle->totalNumPages)),SEEK_SET)== 0)
		{
			fread(memPage,PAGE_SIZE,1,fptr);
			fHandle->curPagePos = (ftell(fptr) / PAGE_SIZE)-1;
			fclose(fptr);
			return RC_OK;
		}
		fclose(fptr);
	}
}
/*
 * Writes at given page number block in file from page handle memory pointer content
 */
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, writes to given block from memPage
     * If not, returns appropriate error of file not initialized
     */
	if (fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

	else if( pageNum <= fHandle->totalNumPages)
	{
	    FILE *fptr;
		fptr= fopen(fHandle->fileName,"r+b");
		if(fseek(fptr, PAGE_SIZE*(pageNum), SEEK_SET)==0)
        {
            fwrite(memPage,PAGE_SIZE,1,fptr);
            fHandle->curPagePos = (ftell(fptr) / PAGE_SIZE)-1 ;
            fHandle->totalNumPages = (fHandle->totalNumPages) +1;
            fclose(fptr);
            return RC_OK;
        }

	}
	else
	{
		return RC_WRITE_FAILED;
	}
}
/*
 * Writes at current page number block in file from page handle memory pointer content
 */
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    /*
     * Checks if file handler is already initialized
     * If yes, writes to current position block from memPage
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
	    FILE * fptr;
	    fptr= fopen(fHandle->fileName,"wb");
	    if(fseek(fptr,PAGE_SIZE*fHandle->curPagePos,SEEK_SET)==0)
		{
		    fwrite(memPage,PAGE_SIZE,1,fptr);
            fHandle->curPagePos = (ftell(fptr) / PAGE_SIZE)-1;
            fclose(fptr);
            return RC_OK;
		}
		fclose(fptr);
		return RC_WRITE_FAILED;
	}

}
/*
 * Adds new page at end of file and fills it with '\0' bytes.
 */
RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    /*
     * Checks if file handler is already initialized
     * If yes, adds a new page with "\0" bytes
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
	    FILE * fptr;
        int i=0;
		fptr= fopen(fHandle->fileName,"a+b");
		while(i<PAGE_SIZE)
		{
			fprintf(fptr,"%c",'\0');
			i++;
		}
		fHandle->curPagePos = (ftell(fptr) / PAGE_SIZE)-1 ;
		fHandle->totalNumPages = fHandle->totalNumPages +1;
		fclose(fptr);
	}
	return RC_OK;
}
/*
 * If the file has less than numberOfPages pages then increase the size to given number of pages.
 */
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
    /*
     * Checks if file handler is already initialized
     * If yes, increases pages to size of given number of pages
     * If not, returns appropriate error of file not initialized
     */
	if(fHandle->fileName == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else if(fHandle->mgmtInfo == NULL)
		return RC_FILE_HANDLE_NOT_INIT;

	else
	{
		if(fHandle->totalNumPages == numberOfPages)
			return RC_OK;

		else if(fHandle->totalNumPages < numberOfPages)
		{
			while((fHandle->totalNumPages)!=numberOfPages)
			{
				appendEmptyBlock(fHandle);
			}
		}
	}
	return RC_OK;
}
