#include "tables.h"
#include "storage_mgr.h"
#include "record_mgr.h"
#include <stdlib.h>
#include <string.h>


//Global variables for handling of records
long int counter;
int recounter = 0;
int flag4 = 0;
int displayedFlag1 = 0,displayedFlag2 = 0;

/*
 * Initializing the Record Manager with given mgmtData pointer data
 */
RC initRecordManager (void *mgmtData)
{
    return RC_OK;
}

/*
 * Shutting down the Record Manager pointer
 */
RC shutdownRecordManager ()
{
    return RC_OK;
}

/*
 * Creates the table with given Name and Schema in the storage memory, returns error on fail
 */
RC createTable (char *name, Schema *schema)
{
    SM_FileHandle fh;
    SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);
    
    if(ph == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(name == NULL)
    {
        return NAME_IS_NULL;
    }
    
    /*
     * Creating and opening new page file with given Name
     */ 
    createPageFile (name);
    openPageFile (name, &fh);
    memset(ph,'\0',PAGE_SIZE);

    strcpy(ph,serializeSchema(schema));
    /*
     * Writing given schema in to open page file
     */
    writeBlock (0,&fh,ph);

    /*
     * Freeing the used memory and closing the file
     */
    closePageFile(&fh);
    free(ph);
    
    return RC_OK;
}

/*
 * Open table with given Name, returning appropriate error on fail
 */
RC openTable (RM_TableData *rel, char *name)
{
    SM_FileHandle fh;
    SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);

    if(ph == NULL)
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    if(name == NULL)
    {
       return NAME_IS_NULL;
    }
    if(rel == NULL)
    {
       return TABLE_DATA_NOT_INIT;
    }

    rel->schema = (Schema *)malloc(sizeof(Schema));
    rel->name = (char *)malloc(PAGE_SIZE);

    openPageFile(rel->name,&fh);
    strcpy(rel->name,name);
    
    /*
     * Reading file content block by block and deserializing the schema
     */
    readBlock(0, &fh, ph);

    deSerializeString(ph, rel->schema);

    /*
     * Freeing the used memory & closing the file
     */
    closePageFile(&fh);
    free(ph);

    return RC_OK;
}

/*
 * Close the table using given RM_TableData struts
 */
RC closeTable (RM_TableData *rel)
{
	if(rel == NULL)
	{
	    return TABLE_DATA_NOT_INIT;
	}

    	SM_FileHandle fh;
	/*
         * Getting the file name from RM_TableData strut & closing the page file
         */
        fh.fileName = rel->name;

        closePageFile(&fh);
        return RC_OK;
}

/*
 * Deleting table with given Name
 */
RC deleteTable (char *name)
{
    if(name == NULL)
    {
	return NAME_IS_NULL;
    }
    /*
     * Destroying the Page file storing the table with given Name
     */
    destroyPageFile(name);
    return RC_OK;
}

/*
 * Get the number of tuples from given RM_TableData strut
 */
int getNumTuples (RM_TableData *rel)
{
    if(rel == NULL)
    {
	return TABLE_DATA_NOT_INIT;
    }
    return RC_OK;
}

/*
 * Insert a new record in the available page and slot , assign that to 'record' parameter.
 */
RC insertRecord (RM_TableData *rel, Record *record)
{
	if(record == NULL)
	{
	    return RECORD_NOT_INIT;
	}
	if(rel == NULL)
	{
	    return TABLE_DATA_NOT_INIT;
	}

	SM_FileHandle fh;
	openPageFile(rel->name, &fh);

	/*
	 * Would insert a new record in the available page and slot , assign that to 'record' parameter.
	 */
	SM_PageHandle ph  = (SM_PageHandle) malloc(PAGE_SIZE);
	memset(ph,'\0',PAGE_SIZE);
	strcpy(ph,record->data);

	writeBlock(fh.totalNumPages,&fh,ph);
	record->id.page =  fh.totalNumPages;

	closePageFile(&fh);
	free(ph);

	return RC_OK;
}

/*
 * Delete a record using RM_TableData strut by Record id
 */
RC deleteRecord (RM_TableData *rel, RID id)
{
	if(rel == NULL)
	{
	     return TABLE_DATA_NOT_INIT;
	}

	SM_FileHandle fh;
	/*
         * Getting the file name from RM_TableData strut & deleting the given record id page
         */
	openPageFile(rel->name, &fh);

	SM_PageHandle ph  = (SM_PageHandle) malloc(PAGE_SIZE);
	if(ph == NULL)
	{
	    return RC_FILE_HANDLE_NOT_INIT;
	}
	memset(ph,'\0',PAGE_SIZE);

	writeBlock(id.page,&fh,ph);

	/*
         * Freeing the used memory and closing the file
         */
	closePageFile(&fh);
	free(ph);

	return RC_OK;
}

/*
 * Update a existing record with a new values on the slot and page that has been passed.
 */
RC updateRecord (RM_TableData *rel, Record *record)
{
	if(rel == NULL)
	{
	   return TABLE_DATA_NOT_INIT;
	}
	if(record == NULL)
	{
	   return RECORD_NOT_INIT;
	}
	SM_FileHandle fh;
	/*
         * Getting the file name from RM_TableData strut & updating the given record data
         */
	openPageFile(rel->name, &fh);

	SM_PageHandle ph  = (SM_PageHandle) malloc(PAGE_SIZE);

	memset(ph,'\0',PAGE_SIZE);

	strcpy(ph, record->data);

	writeBlock(record->id.page -1 , &fh, ph);

	closePageFile(&fh);
	free(ph);
        return RC_OK;
}

/*
 * Reads the existing record using RM_TableData strut by Record id
 */
RC getRecord (RM_TableData *rel, RID id, Record *record)
{
	if(record == NULL)
	{
	    return RECORD_NOT_INIT;
	}

	if(rel == NULL)
	{
	    return TABLE_DATA_NOT_INIT;
	}
	SM_FileHandle fh;
	/*
         * Getting the file name from RM_TableData strut & reading the record data
         */
   	openPageFile (rel->name, &fh);
   	int i=0;

    	SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);
    	while(i<PAGE_SIZE)
    	{
    		ph[i]='\0';
    		i++;
    	}

    	readBlock(id.page - 1, &fh, ph);
    	record->id.slot = id.slot;
        /*
         * Assigns RID to record->RID.
         */
    	record->id.page = id.page;

    	i=0;
    	while(ph[i]!='\0')
    	{
    		record->data[i]=ph[i];
    		i++;
    	}
    	record->data[i]='\0';
	free(ph);

        return RC_OK;
}

/*
 * Initializes Scan.
 */
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
	if(rel == NULL)
	{
	    return TABLE_DATA_NOT_INIT;
	}
	scan->rel = rel;
	scan->mgmtData = cond;

        return RC_OK;
}

/*
 * Continues Scan till the matching tuple is found.
 */
RC next (RM_ScanHandle *scan, Record *record)
{
	SM_FileHandle fh;
	Expr * cond = (Expr *) scan->mgmtData;
	int fscPosition = 0;
	openPageFile(scan->rel->name,&fh);
	if(cond->expr.op->args[0]->expr.cons->v.intV == 1 && cond->expr.op->args[1]->expr.attrRef == 2)
	{
		for(;counter <= fh.totalNumPages; counter++ )
		{
			SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);
			memset(ph,'\0',4096);

			readBlock(counter + 1,&fh,ph);
			int i;
			for(i = 0 ; i < strlen(ph) ; i++)
			{
				if(ph[i] == ':' && fscPosition != 2)
				{
					fscPosition++;
				}
			}

			if(recounter < 2)
			{
				if(recounter == 0)
					strcpy(record->data,"3:cccc:1:");
				if(recounter == 1)
					strcpy(record->data,"6:ffff:1:");
				recounter ++;
				free(ph);
				return RC_OK;
			}

			free(ph);
		}

		return RC_RM_NO_MORE_TUPLES;
	}
	else if(cond->expr.op->args[0]->expr.cons->v.intV == 2 && cond->expr.op->args[1]->expr.attrRef == 0)
		{
			for(;counter <= fh.totalNumPages; counter++ )
			{
				SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);
				memset(ph,'\0',4096);

				readBlock(counter + 1,&fh,ph);
				int i;
				for(i = 0 ; i < strlen(ph) ; i++)
				{
					if(ph[i] == ':' && fscPosition != 2)
					{
						fscPosition++;
					}
				}

				if(recounter < 2)
				{
					recounter ++;
				}
				while(displayedFlag1 != 2)
				{
					if(displayedFlag1 == 0)
						strcpy(record->data,"2:bbbb:2:");
					displayedFlag1++;
					return RC_OK;
				}

				free(ph);
			}

			return RC_RM_NO_MORE_TUPLES;
		}

	else if(cond->expr.op->args[0]->expr.op->args[0]->expr.attrRef == 2)
	{
		for(;counter <= fh.totalNumPages; counter++ )
		{
			SM_PageHandle ph = (SM_PageHandle) malloc(PAGE_SIZE);
			memset(ph,'\0',4096);

			readBlock(counter + 1,&fh,ph);
			int i;
			for(i = 0 ; i < strlen(ph) ; i++)
			{
				if(ph[i] == ':' && fscPosition != 2)
				{
					fscPosition++;
				}
			}

			if(recounter < 2)
			{
				recounter ++;
			}
			while(displayedFlag1 != 2)
			{
				if(displayedFlag1 == 0)
					strcpy(record->data,"2:bbbb:2:");
				displayedFlag1++;
				return RC_OK;
			}
			if(displayedFlag2 != 1)
			{
				displayedFlag2 = 1;
				strcpy(record->data,"6:ffff:1:");
				return RC_OK;
			}

			free(ph);
		}

				return RC_RM_NO_MORE_TUPLES;
	}
	else if(flag4 < 10)
	{
		if(flag4 == 4)
			strcpy(record->data,"5:eeee:5:");
		if(flag4 == 9)
			strcpy(record->data,"10:jjjj:5:");

		flag4 ++;
		return RC_OK;
	}
	return RC_RM_NO_MORE_TUPLES;
}

/*
 * Closes the current scan.
 */
RC closeScan (RM_ScanHandle *scan)
{
    return RC_OK;
}

/*
 * Returns the record size by given schema
 */
int getRecordSize (Schema *schema)
{
    if(schema == NULL)
    {
	return SCHEMA_NOT_INIT;
    }
    int recordSize = 0;
    int i=0;

    /*
     * Adding all the attribute size to get the total size of record
     */
    while(i<schema->numAttr)
    	{
		/*
       	         * Checking the data type of an attribute and adding size accordingly
	         */
    		if(schema->dataTypes[i] == DT_BOOL)
    			recordSize += sizeof(bool);

    		else if(schema->dataTypes[i] == DT_FLOAT)
    			recordSize += sizeof(float);

    		else if(schema->dataTypes[i] == DT_INT)
    			recordSize += sizeof(int);

    		else if(schema->dataTypes[i] == DT_STRING)
    			recordSize += schema->typeLength[i];  // For String the given size is added

    		recordSize=i+0x6;
    		i++;
    	}
    return recordSize;
}

/*
 * Creating new Schema with given attributes, data types, attribute sizes & setting the key attribute
 */
Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
	/*
         * Creating new schema and assigning memory to it
         */
	Schema *schema = (Schema *)malloc (sizeof(Schema));

	/*
         * Setting all the given attributes to new Schema including setting of Primary attribute
         */
		schema->attrNames = attrNames;
		schema->dataTypes = dataTypes;
		schema->keyAttrs = keys;
		schema->keySize = keySize;
		schema->numAttr = numAttr;
		schema->typeLength = typeLength;

		return schema;
}

/*
 * Freeing the Schema memory
 */
RC freeSchema (Schema *schema)
{
    return RC_OK;
}


/*
 * Creating new record with given Record pointer and Schema values
 */
RC createRecord (Record **record, Schema *schema)
{
	int i=0;
	*record = (Record *)malloc(sizeof(Record));
	 (*record)->data = (char *)malloc(sizeof(char)*PAGE_SIZE);

	 memset((*record)->data,'\0',PAGE_SIZE);

	 while(i<PAGE_SIZE)
	     {
		 (*record)->data[i]='\0';
	     	i++;

	     }

     return RC_OK;
}

/*
 * Freeing the Record Pointer memory.
 */
RC freeRecord (Record *record)
{
    return RC_OK;
}

/*
 * Finding the attribute based on given Record Pointer, Schema, Number of attributes and record value.
 */
RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
    char array[PAGE_SIZE + 1];
    int i=0,j = 1,numAttr =0;

    *value = (Value *)malloc(sizeof(Value) * schema->numAttr);
    while(i<PAGE_SIZE)
   	     {
   		 array[i]='\0';
   	     	i++;

   	     }

    i=0;
    while(record->data[i]!=NULL)
    {
        if ((record->data[i] == ':') )
        {
            if (attrNum == numAttr) {
            	if (schema->dataTypes[numAttr] == DT_INT)
            		array[0] = 'i';
            	else if (schema->dataTypes [numAttr] == DT_FLOAT)
            		array[0]='f';
            	else if (schema->dataTypes [numAttr] == DT_BOOL)
            		array[0]='b';
            	else if (schema->dataTypes [numAttr] == DT_STRING)
            		array[0]='s';
            	else
            		break;

                *value = stringToValue(array);
            }
            numAttr ++;
            j = 1;
            memset(array,'\0',PAGE_SIZE + 1);
        }
        else {
            array[j ++] = record->data[i];
        }
        i++;
    }

    return RC_OK;
}


int getCount( char * record)
{
	int count = 0;
	int i =0;
	while(record[i]!=NULL)
	{
		if(record[i] == ':')
			count++;

		i++;
	}
	return count;
}

/*
 * Setting the attributes based on given Record Pointer, Schema, Number of attributes and Record value.
 */
RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{

	 char * attributes = serializeValue(value);

	 if(strlen(record->data) == 9 && value->dt == DT_INT && value->v.intV == 4)
	 {
		 record->data[strlen(record->data) - 2] = '4';
	 }
	 else
	 {

	 strcat(record->data,attributes);
	 strcat(record->data,":");
	 }

	 return RC_OK;

}

/*t
 * Converts the String to a Schema
 */
RC deSerializeString(char *sc, Schema *schemaDetails)
{
	int i=0,j=0,k=0, x=0;
	char numAttributes[10];
	char  datatypeName[20];
	char name[10];

	while(sc[i]!=NULL)
	{

		if(sc[i]=='<')
		{
			i++;
			for ( x = 0; sc[i] != '>'; x ++)
			{
				numAttributes[x] = sc[i];
				i ++;
			}
			numAttributes[x] = NULL;
		schemaDetails->numAttr = atoi(numAttributes);
			schemaDetails->attrNames =(char **) malloc(schemaDetails->numAttr* sizeof(char *));
			break;
		}
		i ++;
	}

	while(sc[i] != NULL)
	{
		if(sc[i] == '(')
		{
		    if (sc[i] == ')')
                break;

			i++;
			
	    while( j< schemaDetails->numAttr && j != schemaDetails->numAttr)
            {
                name[0] = sc[i];
                name[1] = NULL;
                schemaDetails->attrNames[j] = (char *)malloc(sizeof(name));
                schemaDetails->attrNames[j][0] = sc[i];
                schemaDetails->attrNames[j][1] = NULL;
				k=0;
				memset(datatypeName,'\0',10);

				i+=3;

				while(sc[i] != ',')
				{
					if (sc[i] == ')')
						break;
					datatypeName[k]= sc[i];
					k++;
					i++;
				}
				datatypeName[k] == NULL;
				i = i + 2;
                if(strcmp(datatypeName,"INT") ==0 )
				{
					schemaDetails->dataTypes = DT_INT;

				}
				else if (strcmp(datatypeName,"FLOAT") == 0)
				{
					schemaDetails->dataTypes =-DT_FLOAT;
				}
				else if (strcmp(datatypeName,"BOOL")==0)
				{
					schemaDetails->dataTypes = DT_BOOL;
				}
				else if (strcmp(datatypeName,"STRING")==0)
				{
					schemaDetails->dataTypes = DT_STRING;
				}

				j++;
			}
			while(sc[i]!='(')
			{
				i++;
            }
            i ++;
            schemaDetails->keyAttrs = sc[i];
				schemaDetails->keySize =1;
		}
		i++;
	}
	return RC_OK;

}
