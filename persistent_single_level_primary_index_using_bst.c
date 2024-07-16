#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;


int fill_ndx(struct BST_Node *root);

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

	//Opening the files in write binary mode so that the files will be created if they dont already exist and overwrite their contents if they do exist.
	FILE *pds_data=fopen(filename_data,"wb");
	FILE *pds_ndx=fopen(filename_ndx,"wb");

	//Returning error status if there is an error opening the file.
	if(pds_data==NULL || pds_ndx==NULL) 
	{
		return PDS_FILE_ERROR;
	}

	//Writing '0' into the index file and returning appropriate error status if there is an error writing into the file.
	int zero=0;
	int ints_written=fwrite(&zero,sizeof(int),1,pds_ndx);
	if(ints_written!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the data file and returning an appropriate error status if there is an error closing the file.
	int close_status_data=fclose(pds_data);
	if(close_status_data!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the data file and returning an appropriate error status if there is an error closing the file.
	int close_status_ndx=fclose(pds_ndx);
	if(close_status_ndx!=0)
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

	//Returning an error status if the repo_status is already initialized to the error status for 'PDS_REPO_OPEN'. Initializing the repo_status to 'PDS_REPO_OPEN' if not already.
	if(repo_handle.repo_status==PDS_REPO_OPEN || repo_handle.repo_status==PDS_REPO_ALREADY_OPEN)
	{
		return PDS_REPO_OPEN;
	}
	repo_handle.repo_status=PDS_REPO_OPEN;

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

	//Initializing the record size of the global struct.
	repo_handle.rec_size=rec_size;

	//Initializing the record count by reading from the index file. If there is an error reading from the file then return a 'PDF_FILE_ERROR'.
	int integers_read=fread(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(integers_read!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the index entries from the ndx file.
	int reading_from_ndx_status=pds_load_ndx();
	if(reading_from_ndx_status!=PDS_SUCCESS)
	{
		return PDS_FILE_ERROR;
	}

	//Closing only the index file.
	int close_status_ndx=fclose(repo_handle.pds_ndx_fp);
	if(close_status_ndx!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Making the FILE pointer null as the file has been closed.
	repo_handle.pds_ndx_fp=NULL;

	return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx()
{
	//Reading the index entries from the index file.
	int entries=repo_handle.rec_count;
	struct BST_Node *head=NULL;

	for(int i=0;i<entries;i++)
	{
		//Using a temporary struct to store a single entry.
		struct PDS_NdxInfo tmp_ndx;
		int struct_read_status=fread(&tmp_ndx,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		if(struct_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Storing the key and offset from the struct into temporary variables.
		int node_key=tmp_ndx.key;
		int *node_offset=(int*)malloc(sizeof(int));
		*node_offset=tmp_ndx.offset;

		//Adding the index entry read from the index file into the bst.
		int status=bst_add_node(&head,node_key,node_offset);
		if(status!=BST_SUCCESS)
		{
			return PDS_FILE_ERROR;
		}
	}

	//Storing the head of the bst into the global struct.
	repo_handle.ndx_root=head;
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
		return PDS_FILE_ERROR;
	}

	//Returning an error status if the record count is already greater than or equal to maximum size.
	if(repo_handle.rec_size >= MAX_NDX_SIZE)
	{
		return PDS_ADD_FAILED;
	}

	//Setting the location pointer to the end of the file.
	int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_END);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Getting the current location by using ftell and assigning it to appropriate pointer created using malloc.
	int *curr_offset=(int*)malloc(sizeof(int));
	*curr_offset=(int)ftell(repo_handle.pds_data_fp);

	//Adding a node with the key and offset into the bst. If the node already exists or if the root node is null, then the function returns a 'BST_DUP_KEY' or 'BST_NULL' status which is handled by simply returning the error status "PDS_ADD_FAILED".
	int add_node_status=bst_add_node(&repo_handle.ndx_root,key,curr_offset);
	if(add_node_status!=BST_SUCCESS)
	{
		return PDS_ADD_FAILED;
	}

	//Writing only the record into the data file.
	int rec_written_status=fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(rec_written_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Incrementing the record count by one.
	repo_handle.rec_count++;

	return PDS_SUCCESS;
}

// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_key( int key, void *rec )
{
	//Returning an error if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_FILE_ERROR;
	}

	//Searching for the node in the bst with the matching key. Returns an error status if the node is NULL.
	struct BST_Node *searched_node=bst_search(repo_handle.ndx_root,key);
	if(searched_node==NULL)
	{
		return PDS_REC_NOT_FOUND;
	}

	//Storing the offset pointer of the node by converting it into an int pointer and setting the location pointer to the value pointed to by the offset pointer.
	int *offset=(int*)searched_node->data;
	int set_lp=fseek(repo_handle.pds_data_fp,*offset,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the record from the data file after setting the location pointer.
	int record_read_status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(record_read_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
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
	repo_handle.repo_status=PDS_REPO_CLOSED;

	//Creating a character array to hold the index file name.
	char ndx_filename[30];
	strcpy(ndx_filename,repo_handle.pds_name);
	strcat(ndx_filename,".ndx");

	//Opening the index filename in 'wb' mode to overwrite the contents of the file. Returning an error status if there is an error opening the file.
	FILE *wfptr=fopen(ndx_filename,"wb+");
	if(wfptr==NULL)
	{
		return PDS_FILE_ERROR;
	}
	repo_handle.pds_ndx_fp=wfptr;

	//Writing the count of the records. Returning an error status if there is an error writing into the file.
	int ints_written=fwrite(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(ints_written!=1)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Overwriting the index file with the record count followed by the index entries.
	int filling_ndx_file_status=fill_ndx(repo_handle.ndx_root);
	if(filling_ndx_file_status!=PDS_SUCCESS)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Closing the data file. Returning an error status if there is an error closing the file.
	int close_data_status=fclose(repo_handle.pds_data_fp);
	if(close_data_status!=0)
	{
		return PDS_FILE_ERROR;
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
	repo_handle.rec_size=0;
	repo_handle.rec_count=0;

	//Freeing the bst.
	bst_destroy(repo_handle.ndx_root);

	return PDS_SUCCESS;
}

//A recursive function to traverse the given binary tree in pre-order traversal and write the struct PDS_NdxInfo into the index file.
int fill_ndx(struct BST_Node *root)
{
	if(root==NULL)
	{
		return PDS_SUCCESS;
	}

	//Creating a temporary struct to write into the file and storing the key.
	struct PDS_NdxInfo tmp;
	tmp.key=root->key;

	//Storing the offset value into the struct.
	tmp.offset=*((int*)root->data);
	/*
	int *offset_ptr=(int*)malloc(sizeof(int));
	offset_ptr=(int*)root->data;
	tmp.offset=*offset_ptr;
	*/

	//Writing the struct into the index file.
	int write_rec_status=fwrite(&tmp,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
	if(write_rec_status!=1)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Calling recursively on the left child.
	int recursive_lc=fill_ndx(root->left_child);
	if(recursive_lc!=PDS_SUCCESS)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Calling recursively on the right child.
	int recursive_rc=fill_ndx(root->right_child);
	if(recursive_rc!=PDS_SUCCESS)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	return PDS_SUCCESS;
}
