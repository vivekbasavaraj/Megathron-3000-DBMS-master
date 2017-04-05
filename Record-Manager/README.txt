********************************************************************************************************
		       Project Assignment 3: RECORD MANAGER
		Project team-mates: asureshh, asrikant, kvaghas1, pbalasu5 
********************************************************************************************************

                                       **********
                                        CONTENTS
                                       **********
1) Procedure to Run the Project
2) Project Split-up Process
3) Description of Functions/Methods
4) List of Additional Error Checks.

*********************************************************************************************************
                                    PROCEDURE TO RUN THE PROJECT
*********************************************************************************************************
We have created one makefile for our project (which generates binaries for both test_assign3_1.c and test_expr.c).
The procedure to run the makefile is as follows:

PROCEDURE TO RUN THE MAKEFILE (Makefile):
1) Through the Linux terminal, navigate to the directory with assignment contents using the command 'cd'
2) Now Type: 
		make -f Makefile

TO RUN test_assign3_1.c
3) Then Type:
		./assign3

TO RUN test_expr.c			
4) Type:
		./test_expr
			
*********************************************************************************************************
                                      PROJECT SPLIT-UP PROCESS
*********************************************************************************************************
The implementation of entire project was split up into various phases
1) PHASE 1 - Designing the data structures and architecture for the implementation
2) PHASE 2 - Methods/Operations pertaining to table and manager
3) PHASE 3 - Methods/Operations pertaining to handling records in a table
4) PHASE 4 - Methods/Operations pertaining to scans
5) PHASE 5 - Methods/Operations pertaining to dealing with schemas
6) PHASE 6 - Methods/Operations pertaining to dealing with records and attribute values
7) PHASE 7 - Making sure that the given test cases (test_assign3_1.c) executes correctly for our implementation
  
*********************************************************************************************************
                                    DESCRIPTION OF FUNCTIONS/METHODS
*********************************************************************************************************

Function Name: initRecordManager
Arguments: void *mgmtData
-----------------------------
Initialzes the record manager.

Function Name: shutdownRecordManager
Arguments: NO ARGUMENTS
-----------------------------
Shut downs current record manager and frees up all the resources.

Function Name: createTable
Arguments: char *name, Schema *schema
-----------------------------
1) Creates a new page file for the given table
2) Writes the schema to first page.

Function Name: openTable
Arguments: RM_TableData *rel, char *name
-----------------------------
1) It opens the table by accessing the page file.
2) Reading file content block by block and deserializing the schema

Function Name: closeTable
Arguments: RM_TableData *rel
-----------------------------
1) Close the table using given RM_TableData struts
2) Getting the file name from RM_TableData strut & closing the page file

Function Name: deleteTable
Arguments: char *name
-----------------------------
Deletes the table by destroying the relative page file.

Function Name: getNumTuples
Arguments: RM_TableData *rel
-----------------------------
Getting the file name from RM_TableData strut & returning Total Number of Pages

Function Name: insertRecord
Arguments: RM_TableData *rel, Record *record
-----------------------------
Insert a new record in the available page and slot , assign that to 'record' parameter.

Function Name: deleteRecord
Arguments: RM_TableData *rel, RID id
-----------------------------
Delete a record using RM_TableData strut by Record id

Function Name: updateRecord
Arguments: RM_TableData *rel, Record *record
-----------------------------
Update a existing record with a new values on the slot and page that has been passed.

Function Name: getRecord
Arguments: RM_TableData *rel, RID id, Record *record
-----------------------------
1) Fetches the existing record based on RID parameter.
2) Assigns RID to record -> RID.

Function Name: startScan
Arguments: RM_TableData *rel, RM_ScanHandle *scan, Expr *cond
-----------------------------
This Initializes the Scan.

Function Name: next
Arguments: RM_ScanHandle *scan, Record *record
-----------------------------
It continues to scan till the matching tuple is found.

Function Name: closeScan
Arguments: RM_ScanHandle *scan
-----------------------------
It closes the current scan.

Function Name: getRecordSize
Arguments: Schema *schema
-----------------------------
1) It gets the size of the record within the schema specified.
2) Adds all the attribute size to get the total size of record
3) Checking the data type of an attribute and adding size accordingly.
(For String the given size is added.)

Function Name: *createSchema
Arguments: int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys
-----------------------------
1) Creating new Schema with given attributes, data types, attribute sizes & setting the key attribute.
2) Setting all the given attributes to new Schema including setting of Primary attribute.

Function Name: freeSchema
Arguments: Schema *schema
-----------------------------
It frees schema's memory by freeing the space ocupied by each schema.

Function Name: createRecord
Arguments: Record **record, Schema *schema
-----------------------------
1) Creating new record with given Record pointer and Schema values.
2) Returns the status of the record created.

Function Name: freeRecord
Arguments: Record *record
-----------------------------
It frees the memory occupied by given record pointer.

Function Name: getAttr
Arguments: Record *record, Schema *schema, int attrNum, Value **value
-----------------------------
Finding the attribute based on given Record Pointer, Schema, Number of attributes and record value.

Function Name: setAttr
Arguments: Record *record, Schema *schema, int attrNum, Value *value
-----------------------------
Setting the attributes based on given Record Pointer, Schema, Number of attributes and Record value.

Function Name: deSealizeString
Arguments: char *sc, Schema *schemaDetails
-----------------------------
Converts the String to Schema

*********************************************************************************************************
                                    LIST OF ADDITIONAL ERROR CHECKS
*********************************************************************************************************
In this project we have incorporated additional error checks.
1) #define NAME_IS_NULL  600
2) #define TABLE_DATA_NOT_INIT 601 
3) #define RECORD_NOT_INIT 602
4) #define SCHEMA_NOT_INIT 604
5) #define RC_FIX_COUNT_NOT_NULL 400	 
6) #define RC_STRATEGY_NOT_FOUND 500 
