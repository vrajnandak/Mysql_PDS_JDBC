#include <stdio.h>
#include<string.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name)
{
	//Creating 2 strings to hold the filenames with the extensions, '.dat' and the '.ndx'.
	char filename_data[30];
	char filename_ndx[30];

	strcpy(filename_data,repo_name);
	strcpy(filename_ndx,repo_name);

	strcat(filename_data,".dat");
	strcat(filename_ndx,".ndx");

	//Opening the file in write binary mode so that the file will be created if it doesn't already exist.
	FILE *pds_data=fopen(filename_data,"wb");
	FILE *pds_ndx=fopen(filename_ndx,"wb");

	//Returning error status if there is an error opening the file.
	if(pds_data==NULL || pds_ndx==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Writing '0' into the '.ndx' file to show that there are 0 structs(i.e., key,offset pairs) in the index file.
	int zero=0;
	int ints_written=fwrite(&zero,sizeof(int),1,pds_ndx);
	if(ints_written!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the data file. Returns an error status if there is an error closing the file.
	int close_data_status=fclose(pds_data);
	if(close_data_status!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the index file. Returns an error status if there is an error closing the file.
	int close_ndx_status=fclose(pds_ndx);
	if(close_ndx_status!=0)
	{
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
}

// pds_open
// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open( char *repo_name, int rec_size )
{
	//Initializing the file name into the global struct.
	strcpy(repo_handle.pds_name,repo_name);

	//Creating 2 strings to open the filenames with the extensions, '.dat' and the '.ndx'.
	char filename_data[30];
	char filename_ndx[30];

	strcpy(filename_data,repo_handle.pds_name);
	strcpy(filename_ndx,repo_handle.pds_name);

	strcat(filename_data,".dat");
	strcat(filename_ndx,".ndx");

	//Opening the data and the index files. Returning an error status if there is an error opening either of the files.
	FILE *pds_data=fopen(filename_data,"rb+");
	FILE *pds_ndx=fopen(filename_ndx,"rb+");
	if(pds_data==NULL || pds_ndx==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Initializing the FILE pointers of the global struct.
	repo_handle.pds_data_fp=pds_data;
	repo_handle.pds_ndx_fp=pds_ndx;

	//Returning an error status if the repo_status is already initialized to the error status for 'PDS_REPO_OPEN'. Initializing the repo_status to 'PDS_REPO_OPEN' if not already.
	if(repo_handle.repo_status==PDS_REPO_OPEN)
	{
		return PDS_REPO_ALREADY_OPEN;
	}
	repo_handle.repo_status=PDS_REPO_OPEN;

	//Initializing the record size of the global struct.
	repo_handle.rec_size=rec_size;

	//Reading the records from the index file into the struct array of the global struct. Returning an error status if there is an error reading from the index file.
	int pds_load_ndx_status=pds_load_ndx();
	if(pds_load_ndx_status!=PDS_SUCCESS)
	{
		return PDS_FILE_ERROR;
	}

	//Closing only the index file as we have already read it into the array. Returning an error status if there is an error closing the file.
	int close_status=fclose(repo_handle.pds_ndx_fp);
	if(close_status!=0)
	{
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx()
{
	//Setting the location pointer to the start of the file. Returning an error code if there is an error setting the location pointer.
	int set_lp=fseek(repo_handle.pds_ndx_fp,0,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the first line, which is an integer denoting the number of structs holding the key, offset values. Returning an error status if there is an error reading from the file.
	int ints_read=fread(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(ints_read!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the following structs from the file. Returning an error status if there is an error when reading from the file.
	int structs_read=fread(repo_handle.ndx_array,sizeof(struct PDS_NdxInfo),repo_handle.rec_count,repo_handle.pds_ndx_fp);
	if(structs_read!=repo_handle.rec_count)
	{
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
}

// put_rec_by_key
// Seek to the end of the data file
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Write the record at the end of the file
// Return failure in case of duplicate key
int put_rec_by_key( int key, void *rec )
{
	//Returning an error status if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_ADD_FAILED;
	}

	//Setting the location pointer to the end of the file to write the record. Returning an error status if there is an error setting the location pointer.
	int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_END);
	if(set_lp!=0)
	{
		return PDS_ADD_FAILED;
	}

	//Getting the current location of the location pointer using ftell(). Returning an error if there is an error in getting the location of the location pointer.
	long curr_location=ftell(repo_handle.pds_data_fp);
	if(curr_location==-1)
	{
		return PDS_ADD_FAILED;
	}

	//Returning an error status if the key already exists in the array.
	int size=repo_handle.rec_count;
	for(int i=0;i<size;i++)
	{
		if(repo_handle.ndx_array[i].key==key)
		{
			return PDS_ADD_FAILED;
		}
	}

	//Adding the key,offset values of the added record into the array.
	int arr_indice=repo_handle.rec_count;
	repo_handle.ndx_array[arr_indice].key=key;
	repo_handle.ndx_array[arr_indice].offset=curr_location;

	//Incrementing the number of records present in the array.
	repo_handle.rec_count++;

	//Writing the key into the data file first. Returning an error status if there is an error writing the key into the file.
	int ints_written=fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
	if(ints_written!=1)
	{
		return PDS_ADD_FAILED;
	}

	//Writing the record into the data file. Returning an error status if there is an error writing the record into the data file.
	int recs_written=fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(recs_written!=1)
	{
		return PDS_ADD_FAILED;
	}

	return PDS_SUCCESS;
}


// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the key at the current location 
// Read the record from the current location
int get_rec_by_key( int key, void *rec )
{
	//Returning an error status if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_ADD_FAILED;
	}

	//Performing linear search on the entire array to check if the key already exists.
	int size=repo_handle.rec_count;
	for(int i=0;i<size;i++)
	{
		if(repo_handle.ndx_array[i].key==key)
		{
			//If the key matches, then the location pointer is set to the offset value from the start of the file.
			fseek(repo_handle.pds_data_fp,repo_handle.ndx_array[i].offset,SEEK_SET);

			//Reading the key first, then the record. Returning an error status if there is an error reading from the file.
			int key_read;
			int ints_read=fread(&key_read,sizeof(int),1,repo_handle.pds_data_fp);
			if(ints_read!=1)
			{
				return PDS_FILE_ERROR;
			}
			int records_read=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
			if(records_read!=1)
			{
				return PDS_FILE_ERROR;
			}

			return PDS_SUCCESS;
		}
	}
	return PDS_REC_NOT_FOUND;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing it (overwrite the entire index file)
// Close the index file and data file
int pds_close()
{
	//Returning an error status if the repo_status is set to 'PDS_REPO_CLOSED'.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_FILE_ERROR;
	}

	//Creating a character array to hold the index file name.
	char ndx_filename[30];
	strcpy(ndx_filename,repo_handle.pds_name);
	strcat(ndx_filename,".ndx");

	//Opening the index filename in 'wb' mode to overwrite the contents of the file. Returning an error status if there is an error opening the file.
	FILE *wfptr=fopen(ndx_filename,"wb");
	if(wfptr==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Writing the count of the records. Returning an error status if there is an error writing into the file.
	int ints_written=fwrite(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(ints_written!=1)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Writing the records into the file. Returning an error status if there is an error writing into the file.
	int structs_written=fwrite(repo_handle.ndx_array,repo_handle.rec_size,repo_handle.rec_count,repo_handle.pds_ndx_fp);
	if(structs_written!=repo_handle.rec_count)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Closing the data file. Returning an error status if there is an error closing the file.
	int close_data_status=fclose(repo_handle.pds_data_fp);
	if(close_data_status!=0)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Closing the index file. Returning an error status if there is an error closing the file.
	int close_ndx_status=fclose(repo_handle.pds_ndx_fp);
	if(close_ndx_status!=0)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Resetting the members of the global struct.
	strcpy(repo_handle.pds_name,"\0");
	repo_handle.pds_data_fp=NULL;
	repo_handle.pds_ndx_fp=NULL;
	repo_handle.repo_status=PDS_REPO_CLOSED;
	repo_handle.rec_size=0;
	repo_handle.rec_count=0;
	memset(repo_handle.ndx_array,0,MAX_NDX_SIZE*sizeof(struct PDS_NdxInfo));

	return PDS_SUCCESS;
}
