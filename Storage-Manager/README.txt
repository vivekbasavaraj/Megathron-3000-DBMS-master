********************************************************************************************************
		       Project Assignment 1: STORAGE MANAGER
		Project team-mates: asureshh, asrikant, kvaghas1, pbalasu5 
********************************************************************************************************

                                       **********
                                        CONTENTS
                                       **********
1) Procedure to Run the Project
2) Project Split-up Process
3) Description of Functions/Methods
4) List of Additional Test cases incorporated


*********************************************************************************************************
                                    PROCEDURE TO RUN THE PROJECT
*********************************************************************************************************
We have created two makefiles for our project (one for test_assign1_1.c and one for test_assign1_2.c).
The procedure to run the makefiles are as follows:

PROCEDURE TO RUN THE FIRST MAKEFILE (Makefile1):
1) Through the Linux terminal, navigate to the directory with assignment contents using the command 'cd'
2) Now Type: 
	make -f Makefile1
			
PROCEDURE TO RUN THE FIRST MAKEFILE (Makefile2):
1) Through the Linux terminal, navigate to the directory with assignment contents using the command 'cd'
2) Now Type: 
	make -f Makefile2
			
*********************************************************************************************************
                                      PROJECT SPLIT-UP PROCESS
*********************************************************************************************************
The implementation of entire project was split up into various phases
1) PHASE 1 - Implemented page manipulation methods.
2) PHASE 2 - Implemented methods on Reading blocks from disc.
3) PHASE 3 - Implemented methods on writing blocks to a page file.
  
*********************************************************************************************************
                                    DESCRIPTION OF FUNCTIONS/METHODS
*********************************************************************************************************

Function Name: createPageFile	
Arguments: char *fileName
-----------------------------
1) Creating an empty file. 	 
2) Checking if file is successfully created
3) If yes, filling up file with '\0'
   If not, returning error message. 


Function Name: openPageFile	
Arguments: char *fileName, SM_FileHandle *fHandle
-------------------------------------------------
1) Opens a file for input operations.
2) Checks if file exists with given name
3) If yes, initializing file handle with all file related information
   If not, returning appropriate file not found error message


Function Name: closePageFile
Arguments: SM_FileHandle *fHandle
---------------------------------
1) Closes an open page file. 
2) Checks if file handler is already initialized
   If yes, closing a file and removing file handle references
   If not, returning appropriate error of file not initialized


Function Name: destroyPageFile	
Arguments: char *fileName
------------------------------
1) Deletes a page file.
2) Removing a file with given name if exists, returning error message otherwise.


Function Name: readBlock	
Arguments: int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage
---------------------------------------------------------------------
1) Reads given page number block from file and stores its content into page handle memory pointer.
2) Checks if file handler is already initialized
    If yes, reads block and stores into memPage
    If not, returns appropriate error of file not initialized
     
 
Function Name: getBlockPos	
Arguments: SM_FileHandle *fHandle
---------------------------------
1) Return the current page position of the file.
2) Checks if file handler is already initialized
   If yes, get the current page position and return it
   If not, returns appropriate error of file not initialized


Function Name: readFirstBlock	
Arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
--------------------------------------------------------
1) Reads first page block from file and stores its content into page handle memory pointer
2) Checks if file handler is already initialized
   If yes, reads first block and stores into memPage
   If not, returns appropriate error of file not initialized


Function Name: readPreviousBlock	
Arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
--------------------------------------------------------
1) Reads immediate previous block from file and stores its content into page handle memory pointer
2) Checks if file handler is already initialized
   If yes, reads previous block and stores into memPage
   If not, returns appropriate error of file not initialized


Function Name: readCurrentBlock	
Arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
--------------------------------------------------------
1) Reads current block from file and stores its content into page handle memory pointer
2) Checks if file handler is already initialized
   If yes, reads current block and stores into memPage
   If not, returns appropriate error of file not initialized


Function Name: readNextBlock	
Arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
--------------------------------------------------------
1) Reads next block from file and stores its content into page handle memory pointer
2) Checks if file handler is already initialized
   If yes, reads next block and stores into memPage
   If not, returns appropriate error of file not initialized


Function Name: readLastBlock
Expected arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
-----------------------------------------------------------------
1) Reads last block from file and stores its content into page handle memory pointer
2) Checks if file handler is already initialized
   If yes, reads last block of file and stores into memPage
   If not, returns appropriate error of file not initialized


Function Name: writeBlock 
Arguments: int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage
---------------------------------------------------------------------
1) Writes at given page number block in file from page handle memory pointer content
2) Checks if file handler is already initialized
   If yes, writes to given block from memPage
   If not, returns appropriate error of file not initialized


Function Name: writeCurrentBlock
Arguments: SM_FileHandle *fHandle, SM_PageHandle memPage
--------------------------------------------------------
1) Writes at current page number block in file from page handle memory pointer content
2) Checks if file handler is already initialized
   If yes, writes to current position block from memPage
   If not, returns appropriate error of file not initialized

Function Name: appendEmptyBlock
Arguments: SM_FileHandle *fHandle
---------------------------------
1) Adds new page at end of file and fills it with '\0' bytes.
2) Checks if file handler is already initialized
   If yes, adds a new page with "\0" bytes
   If not, returns appropriate error of file not initialized


Function Name: ensureCapacity
Arguments: int numberOfPages, SM_FileHandle *fHandle
----------------------------------------------------
1) If the file has less than numberOfPages pages then increase the size to given number of pages.
2)  Checks if file handler is already initialized
    If yes, increases pages to size of given number of pages
    If not, returns appropriate error of file not initialized


*********************************************************************************************************
                               LIST OF ADDITIONAL TEST CASES INCORPORATED
*********************************************************************************************************
In this project we have incorporated additional test cases to check the remaining 9 methods.
1) ReadBlock
2) GetBlockPos
3) ReadPreviousBlock
4) ReadCurrentBlock
5) ReadNextBlock
6) ReadLastBlock
7) WriteCurrentBlock
8) EnsureCapacity
9) AppendEmptyBlock
