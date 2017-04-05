#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

// test output files
#define TESTPF "test_pagefile.bin"

// prototypes for test functions
static void testMultiBlockContents(void);

// main function running all tests
int main (void)
{
	testName = "";

	initStorageManager();

	testMultiBlockContents();

	return 0;
}

void testMultiBlockContents(void)
{
	SM_FileHandle fh;
	SM_PageHandle ph;
	int i;

	testName = "Test Multiple Page Contents";

	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	// create a new page file
	TEST_CHECK(createPageFile (TESTPF));
	TEST_CHECK(openPageFile (TESTPF, &fh));
	printf("created and opened file\n");

	// read first page into handle
	TEST_CHECK(readFirstBlock (&fh, ph));
	// the page should be empty (zero bytes)
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
	printf("first block was empty\n");

	// change ph to be a string and write that one to disk
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = (i % 10) + '0';
	TEST_CHECK(writeBlock (0, &fh, ph));
	printf("writing first block\n");

	// change ph to be a string and write that one to disk
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = (i % 10) + '0';
	TEST_CHECK(writeBlock (1, &fh, ph));
	printf("writing second block\n");

	// read back the page containing the string and check that it is correct
	TEST_CHECK(readCurrentBlock (&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
	printf("reading current block\n");

	// read back the page containing the string and check that it is correct
	TEST_CHECK(readPreviousBlock (&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
	printf("reading previous block\n");

	// read back the page containing the string and check that it is correct
	TEST_CHECK(readNextBlock (&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
	printf("reading next block\n");

	//Get Block Position
	TEST_CHECK(getBlockPos (&fh));
	ASSERT_TRUE((fh.curPagePos == getBlockPos (&fh)), "SUCCESS");
	printf("GetBlockPos Successfully Tested\n");

	// read the page containing the string and check it is correct
	TEST_CHECK(readBlock (3,&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
	printf("reading given block\n");

	// change ph to be a string and write that one to disk
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = (i % 10) + '0';
	TEST_CHECK(writeCurrentBlock (&fh, ph));
	printf("writing current block\n");

	// read the page containing the string and check it is correct
	TEST_CHECK(readLastBlock (&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
	printf("reading last block\n");

	//Ensure Capacity
	TEST_CHECK(ensureCapacity(6,&fh));
	printf("Ensure capacity Successful\n");

	// Append Empty Block
	printf("Empty block Appended\n");
	TEST_CHECK(appendEmptyBlock(&fh));

	// destroy new page file
	TEST_CHECK(destroyPageFile (TESTPF));

	TEST_DONE();
}

