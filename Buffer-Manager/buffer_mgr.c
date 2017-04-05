#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <stdlib.h>
#include <string.h>

int writeIO;
int readIO;

/*
 * Stores details of buffered page, including Disk Page Number, Buffer Page Number, Whether page is dirty or not, Frequency for LFU method, fix count & Next Buffer Page reference
 */
typedef struct Buffer_Page_Details
{
	int buffer_pnum;
	bool page_dirty;
	int fix_count;
	int frequency;
	int disc_pnum;
	struct Buffer_Page_Details *next_node;
}Buffer_Page_Details;

/*
 * buf_fptr = First buffer page pointer
 * buf_pptr = Previous buffer page pointer
 * buf_cptr = Current buffer page pointer
 */
Buffer_Page_Details *buf_fptr, *buf_pptr, *buf_cptr;

PageNumber *frameContents;
bool *dirtyFlags;
int *fixCount;

SM_FileHandle fh;
SM_PageHandle ph;
char temp[PAGE_SIZE];

/*
 * Initializes Buffer Pool with given size and data with given Page Replacement Strategy
 */
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{

    bm->pageFile = (char *)pageFileName;
    bm->numPages = (int)numPages;
    bm->strategy = strategy;

    ph = (SM_PageHandle)malloc(sizeof(char)*PAGE_SIZE);
    frameContents = (PageNumber *)malloc(sizeof(PageNumber)*numPages);
    dirtyFlags = (bool *)malloc(sizeof(bool)*numPages);
    fixCount = (int *)malloc(sizeof(int)*numPages);

	int i=0;
	/*
	 * Initializing Buffer Pages up-to given number of pages
	 */
	while(i<numPages)
	{
		buf_cptr = (Buffer_Page_Details *) malloc (sizeof(Buffer_Page_Details));
		if(i==0)
		{
		    /*
		     * Storing First Buffer Page Reference
		     */
			buf_fptr = buf_cptr;
		}

		else
		{
		    /*
		     * Linking Buffer Pages
		     */
			buf_pptr->next_node = buf_cptr;
		}

		buf_cptr->buffer_pnum =i;
		buf_cptr->disc_pnum = -1;
		buf_cptr->fix_count =0;
		buf_cptr->page_dirty = false;
		buf_cptr->frequency = 1;
		buf_pptr = buf_cptr;

		i++;
	}
	buf_cptr->next_node = NULL;

	readIO =0;
	writeIO =0;

	openPageFile((char *)pageFileName,&fh);

	return RC_OK;
}
/*
 * Shut downs a Buffer Pool & Frees up all the associated resources
 */
RC shutdownBufferPool(BM_BufferPool *const bm)
{
    /*
     * Freeing up all the Buffer Pool related reference values
     */
	free(frameContents);
	free(dirtyFlags);
	free(fixCount);

	buf_cptr =NULL;
	buf_pptr =NULL;

	Buffer_Page_Details *tempBPD = buf_fptr;
    int i=0;
    /*
     * Freeing up all the Buffer Pool pages and memory
     */
	while(i < bm->numPages)
	{
		tempBPD = buf_fptr;
		buf_fptr = buf_fptr->next_node;
		free(tempBPD);

		tempBPD = NULL;
		i++;
	}
	tempBPD = NULL;

	return RC_OK;
}
/*
 * Forces Buffer Manager to Write all Dirty Pages to disk
 */
RC forceFlushPool(BM_BufferPool *const bm)
{

	Buffer_Page_Details *node = buf_fptr;

	while(node!=NULL)
	{
		if(node->fix_count != 0)
			return RC_FIX_COUNT_NOT_NULL;

		if(node->page_dirty == true)
		{
			node->page_dirty = false;
		}
		node = node->next_node;
	}
	return RC_OK;
}

/*
 * Marks given page as Dirty in Buffer Pool
 */
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    Buffer_Page_Details *node = buf_fptr;
    while(node!= NULL && node->disc_pnum != page->pageNum)
    {
    	node = node->next_node;
    }
    node->page_dirty = true;
    return RC_OK;
}
/*
 * Unpins the given page from Buffer Pool
 */
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int i=0;
	Buffer_Page_Details *unpin_ptr = buf_fptr;

	while(i<bm->numPages)
	{
		if( unpin_ptr->disc_pnum != page->pageNum)
		{
			unpin_ptr = unpin_ptr->next_node;
		}
		i++;
	}
    unpin_ptr->fix_count--;
	if(unpin_ptr != NULL && unpin_ptr->page_dirty == true)
	{
		char temp[PAGE_SIZE];
		memset(temp,'\0',PAGE_SIZE );
		strcpy(temp,page->data);

		writeBlock(page->pageNum + 1,&fh,temp);
		writeIO =  writeIO +1;
	}
	return RC_OK;
}
/*
 * Writes the current content of given Page back to the page file on Disk
 */
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	Buffer_Page_Details *node = buf_fptr;

	while(node!=NULL)
	{
		if(node->disc_pnum == page->pageNum)
		{
			if(node->page_dirty == true && node->fix_count ==0)
					{
						writeBlock(node->disc_pnum,&fh,page->data);
						node->page_dirty= false;
						writeIO = writeIO+1;
					}
		}
		node =node->next_node;
	}
	return RC_OK;
}
/*
 * Pins the Page with given page number based on Buffer Manager Strategy
 */
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

    int i=0;

	Buffer_Page_Details *node = buf_fptr;
	Buffer_Page_Details *previous_node;
    /*
     * Checking if the page exists in Buffer Pool
     */
	while(i<bm->numPages)
    {
        /*
         * Checks if Current node matches given page
         */
        if(node->disc_pnum == pageNum)
        {

            page->pageNum = node->disc_pnum;
            node->fix_count = node->fix_count+1;
            node->frequency += 1;
            if(bm->strategy == RS_FIFO)
			{
				return RC_OK;

			}
			/*
             * If Buffer Management Strategy is either LRU or LFU, then making changes in position of Buffer Pool Page order
             */
            if(bm->strategy == RS_LRU || bm->strategy == RS_LFU)
            {
                /*
                 * If the requested page is First page in Buffer Pool, changing its position to last page
                 */
            	if(buf_fptr == node)
            	{
            		previous_node = buf_fptr;
					buf_fptr = buf_fptr->next_node;
					node = buf_fptr;
					previous_node->next_node =NULL;
					while(node->next_node != NULL)
					{
						node =  node->next_node;
					}
					node->next_node = previous_node;
                    return RC_OK;
            	}
            	/*
            	 * If the requested node is in any other page than First in Buffer Pool, Putting it to last position and rearranging other nodes
            	 */
            	else
            	{
            		previous_node->next_node = node->next_node;
            		node->next_node = NULL;
            		while(previous_node->next_node!=NULL)
            		{
            			previous_node = previous_node->next_node;
            		}
            		previous_node->next_node = node;
            		 return RC_OK;
            	}
            }

        }
        /*
         * If the requested page is not present in the Buffer Pool but if there is space in the Buffer Pool, get the page from the disc and put into the Buffer Pool
         */
        else if(node->disc_pnum == -1)
        {
        	int ix;
        	for(ix=0;ix<PAGE_SIZE;ix++)
        		ph[ix]='\0';
            node->disc_pnum = i;
            memset(temp,'\0',PAGE_SIZE);
            readBlock(pageNum+1,&fh,ph);
            strcpy(temp,ph);
            page->data = temp;
            page->pageNum = node->disc_pnum;
            node->fix_count = node->fix_count+1;
            node->frequency += 1;
            readIO = readIO +1;
            return RC_OK;
        }
        i++;
        previous_node = node;
        node = node->next_node;
    }
    /*
     * If given page is not in Buffer Pool, creating a new page and inserting it to Buffer Pool with Buffer Manager Strategy
     */
    switch (bm->strategy)
            {
                case RS_FIFO: FIFO_insert(pageNum,page);
                			  readIO = readIO+1;
                              		  break;

                case RS_LRU: FIFO_insert(pageNum,page);
                			 readIO = readIO+1;
                			 break;

                case RS_LFU: LFU_insert(pageNum,page);
                			 readIO = readIO+1;
                			 break;

                default:return RC_STRATEGY_NOT_FOUND;
            }
    return RC_OK;
}
/*
 * Returns the array of disk page numbers & if empty returns No Page error
 */
PageNumber *getFrameContents (BM_BufferPool *const bm)
{
    int i=0;
    Buffer_Page_Details *node = buf_fptr;
    while(i < bm->numPages)
    {
        frameContents[node->buffer_pnum] = node->disc_pnum;
        node = node->next_node;
        i++;
    }
    return frameContents;
}

/*
 * Returns Array of Boolean values indicating whether page in Buffer Pool is dirty or not
 */
bool *getDirtyFlags (BM_BufferPool *const bm)
{
    int i=0;

    Buffer_Page_Details *node = buf_fptr;

    while(i < bm->numPages)
    {
        dirtyFlags[node->buffer_pnum] = node->page_dirty;
        node = node->next_node;
        i++;
    }
    return dirtyFlags;
}

/*
 * Returns Array of Integer values indicating fix count of pages stored in Buffer Pool
 */
int *getFixCounts (BM_BufferPool *const bm)
{
    int i=0;

    Buffer_Page_Details *node = buf_fptr;

    while(i < bm->numPages)
    {
        fixCount[node->buffer_pnum]=node->fix_count;
        node = node->next_node;
        i++;
    }
    return fixCount;
}

/*
 * Returns the count of pages read from Disk
 */
int getNumReadIO (BM_BufferPool *const bm)
{
	return readIO;
}
/*
 * Returns the count of  pages written to the page file from the Buffer Pool
 */
int getNumWriteIO (BM_BufferPool *const bm)
{
	return writeIO;
}
/*
 * Moves the first node to Last
 */
void moveHeadToLastNode()
{
	Buffer_Page_Details *start = buf_fptr;
	Buffer_Page_Details *temp = buf_fptr;
	while(start->next_node != NULL)
	{
		start = start->next_node;
	}
	start->next_node = buf_fptr;
	buf_fptr = buf_fptr->next_node;
	temp->next_node = NULL;
}
/*
 * Inserting a new Page node in Buffer Pool as per First In First Out Strategy
 */
int FIFO_insert(const PageNumber disk_pnum,BM_PageHandle *const page)
{
	if(buf_fptr->fix_count > 0)
		moveHeadToLastNode();
	Buffer_Page_Details *temp1, *new_Node;
	int frameNumber;

	temp1 = buf_fptr;
	frameNumber = temp1->buffer_pnum;

	buf_fptr = buf_fptr->next_node;
	free(temp1);
	temp1 = NULL;

    /*
     * Creating a new node with all the required details of a Buffer page
     */
	new_Node = (Buffer_Page_Details *) malloc (sizeof(Buffer_Page_Details));
	new_Node->buffer_pnum = frameNumber;
	new_Node->disc_pnum = disk_pnum;
	new_Node->fix_count = 1;
	new_Node->page_dirty = false;
	new_Node->next_node = NULL;
	temp1 = buf_fptr;

	page->pageNum = disk_pnum;
	int ix;
	for(ix=0;ix<PAGE_SIZE;ix++)
		ph[0]='\0';

	memset(temp,'\0',PAGE_SIZE);
	readBlock(disk_pnum+1,&fh,ph);
	strcpy(temp,ph);
	page->data = temp;
	while(temp1->next_node != NULL)
	{
		temp1 = temp1->next_node;
	}
	temp1->next_node = new_Node;
	return RC_OK;
}
/*
 * Inserting a new Page node in Buffer Pool as per Least Frequently Used Strategy
 */
int LFU_insert(const PageNumber disk_pnum,BM_PageHandle *const page)
{
	Buffer_Page_Details *temp, *new_Node, *previous, *current;
	int frameNumber;

    /*
     * Creating a new node with all the required details of a Buffer page
     */
	new_Node = (Buffer_Page_Details *) malloc (sizeof(Buffer_Page_Details));
	new_Node->disc_pnum = disk_pnum;
	new_Node->fix_count = 1;
	new_Node->frequency = 1;
	new_Node->page_dirty = false;
	new_Node->next_node = NULL;

    bool flag = true; // To check whether page is inserted or not
    int i = 1; // To check least frequency starting from 1
    previous = NULL;
    while(flag)
    {
        previous = NULL;
        current = buf_fptr;
        while(current!= NULL && flag)
        {
            /*
             * Checks if least frequency matches current page frequency
             */
            if(current->frequency == i)
             {
                 /*
                  * If current page is First in Buffer Pool
                  */
                 if(previous == NULL)
                 {
                    buf_fptr = current->next_node;
                    frameNumber = current->buffer_pnum;
                    new_Node->buffer_pnum = frameNumber;
                    page->pageNum = disk_pnum;
                    temp = current;
                    while(current->next_node != NULL)
                    {
                        current = current->next_node;
                    }
                    free(temp);
                    temp =NULL;
                    current->next_node = new_Node;
                 }
                 /*
                  * If current page is other than First in Buffer Pool
                  */
                 else
                 {
                    previous->next_node = current->next_node;
                    frameNumber = current->buffer_pnum;
                    new_Node->buffer_pnum = frameNumber;
                    page->pageNum = disk_pnum;
                    temp = current;
                    while(current->next_node != NULL)
                    {
                        current = current->next_node;
                    }
                    if(temp->next_node != NULL)
                    {
                        current->next_node = new_Node;
                    }
                    else
                    {
                        previous->next_node = new_Node;
                    }
                    free(temp);
                 }

                 flag = false;
                 break;
             }
             previous = current;
             current = current->next_node;
        }
        i++;
    }
    return RC_OK;
}
