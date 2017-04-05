********************************************************************************************************
		       Project Assignment 2: BUFFER MANAGER
		Project team-mates: asureshh, asrikant, kvaghas1, pbalasu5 
********************************************************************************************************

                                       **********
                                        CONTENTS
                                       **********
1) Procedure to Run the Project
2) Project Split-up Process
3) Description of Functions/Methods
4) List of Additional Error Checks.
5) List of Additional Test Cases.


*********************************************************************************************************
                                    PROCEDURE TO RUN THE PROJECT
*********************************************************************************************************
We have created two makefiles for our project (one for test_assign2_1.c and one for test_assign2_2.c).
The procedure to run the makefiles are as follows:

PROCEDURE TO RUN THE FIRST MAKEFILE (Makefile1):
1) Through the Linux terminal, navigate to the directory with assignment contents using the command 'cd'
2) Now Type: 
		make -f Makefile1
3) Then Type:
		./assign2
			
PROCEDURE TO RUN THE SECOND MAKEFILE (Makefile2):
1) Through the Linux terminal, navigate to the directory with assignment contents using the command 'cd'
2) Now Type: 
		make -f Makefile2
3) Then Type:
		./assign2
			
*********************************************************************************************************
                                      PROJECT SPLIT-UP PROCESS
*********************************************************************************************************
The implementation of entire project was split up into various phases
1) PHASE 1 - Methods/Operations pertaining to Buffer Manager Interface Pool Handling
2) PHASE 2 - Methods/Operations pertaining to Buffer Manager Interface Access Pages
3) PHASE 3 - Methods/Operations pertaining to Statistics Interface
4) PHASE 4 - Methods/Operations pertaining to Page Replacement Strategies
  
*********************************************************************************************************
                                    DESCRIPTION OF FUNCTIONS/METHODS
*********************************************************************************************************

Function Name: initBufferPool	
Arguments: BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData
-----------------------------
1) It Initializes Buffer Pool with given size and data with given Page Replacement Strategy.
2) It also Initializes Buffer Pages up-to given number of pages.
3) Stores the Page Reference of First Buffer and Links Buffer Pages.

Function Name: shutdownBufferPool	
Arguments: BM_BufferPool *const bm
-----------------------------
1) Shuts down the Buffer Pool and frees up all the associated resources when a page is not pinned.
2) Also frees up all the Buffer Pool related reference values, Buffer Pool pages and memory.

Function Name: forceFlushPool
Arguments: BM_BufferPool *const bm
-----------------------------
1) Forces Buffer Manager and writes all Dirty Pages to disk with a fix-count of 0.

Function Name: markDirty
Arguments: BM_BufferPool *const bm, BM_PageHandle *const page
-----------------------------
1) Marks given page as Dirty in Buffer Pool.

Function Name: unpinPage
Arguments: BM_BufferPool *const bm, BM_PageHandle *const page
-----------------------------
1) Unpins the given page from Buffer Pool.

Function Name: forcePage
Arguments: BM_BufferPool *const bm, BM_PageHandle *const page
-----------------------------
1) Writes the current content of given Page back to the page file on Disk.

Function Name: pinPage
Arguments: BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum
-----------------------------
1) Pins the Page with given page number based on Buffer Manager Strategy, strategy may be either RC_FIFO or RC_LRU or RC_LFU.
2) Checks if the page exists in the Buffer Pool.
3) Also checks if Current node matches given page.
4) If Buffer Management Strategy is either LRU or LFU, then making changes in position of Buffer Pool Page order
		i) If the requested page is First page in Buffer Pool, changing its position to last page.
		ii) If the requested page is in any other position other than the first in Buffer Pool, Putting it to last position and rearranging other nodes accordingly.
5) Else the requested page is not present in the Buffer Pool but if there is space in the Buffer Pool, get the page from the disc and put into the Buffer Pool.
6) If given page is not in Buffer Pool,  the Buffer Manager Strategy's is used to replace a page on the Buffer Pool.

Function Name: PageNumber *getFrameContents
Arguments: BM_BufferPool *const bm
-----------------------------
1) Returns the array of disk page numbers & if empty returns No Page error.

Function Name: bool *getDirtyFlags
Arguments: BM_BufferPool *const bm
-----------------------------
1) Returns Array of Boolean values indicating whether page in Buffer Pool is dirty or not.

Function Name: int *getFixCounts
Arguments: BM_BufferPool *const bm
-----------------------------
1) Returns Array of Integer values indicating fix count of pages stored in Buffer Pool.

Function Name: getNumReadIO
Arguments: BM_BufferPool *const bm
-----------------------------
1) Returns the count of pages read from Disk.

Function Name: getNumWriteIO
Arguments: BM_BufferPool *const bm
-----------------------------
1) Returns the count of  pages written to the page file from the Buffer Pool.

Function Name: moveHeadToLastNode
Arguments: NO ARGUMENTS
-----------------------------
1) Moves the first node to Last.

Function Name: FIFO_insert
Arguments: const PageNumber disk_pnum,BM_PageHandle *const page
-----------------------------
1) Inserting a new Page node in Buffer Pool as per (FIFO) First In First Out Strategy.
2) It Creates a new node with all the required details of a Buffer page.

Function Name: LFU_insert
Arguments: const PageNumber disk_pnum,BM_PageHandle *const page
-----------------------------
1) Inserting a new Page node in Buffer Pool as per Least Frequently Used Strategy.
2) Creates a new node with all the required details of a Buffer page.
3) Checks if the least frequency matches with current page frequency.
	i) Checks If current page is First in Buffer Pool.
	ii) Else Checks current page is other than First in Buffer Pool.


*********************************************************************************************************
                                   LIST OF ADDITIONAL ERROR CHECKS
*********************************************************************************************************
In this project we have incorporated additional test cases to check of additional page replacement strategies.
1) Incorporated test cases for LFU (Least Frequently Used) page replacement strategy.


*********************************************************************************************************
                                    LIST OF ADDITIONAL TEST CASES
*********************************************************************************************************
In this project we also incorporated additional error checks.
1) #define RC_FIX_COUNT_NOT_NULL 400	 /* Returned if page is unpined when fix count is greater than zero.*/
2) #define RC_STRATEGY_NOT_FOUND 500     /* Strategy passed is invalid (Valid strategies are RS_FIFO or RS_LRU or RS_LFU.*/
