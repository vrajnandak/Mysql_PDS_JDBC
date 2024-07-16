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

	//Returning an error status if the repo_status is already initialized to the status for 'PDS_REPO_OPEN' or 'PDS_REPO_ALREADY_OPEN'. Initializing the repo_status to 'PDS_REPO_OPEN' if not already.
	if(repo_handle.repo_status==PDS_REPO_OPEN || repo_handle.repo_status==PDS_REPO_ALREADY_OPEN)
	{
		return PDS_REPO_ALREADY_OPEN;
	}
	repo_handle.repo_status=PDS_REPO_OPEN;

	//Opening the data and the index files in 'rb+' mode to read and write records and more. Returning an error status if there is an error opening either of the files.
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

	//Returning a success status.
	return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx()
{
	//Reading the index entries from the index file.

	//Storing the EOF position of the location pointer after setting it to the end of the index file.
	int set_lp_to_end=fseek(repo_handle.pds_ndx_fp,0,SEEK_END);
	if(set_lp_to_end!=0)
	{
		return PDS_FILE_ERROR;
	}

	long EOF_location=ftell(repo_handle.pds_ndx_fp);

	//Settling the location pointer to the start of the index file.
	int set_lp_to_start=fseek(repo_handle.pds_ndx_fp,0,SEEK_SET);
	if(set_lp_to_start!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Initializing the record count by reading from the index file. If there is an error reading from the file then return a 'PDF_FILE_ERROR'.
	int integers_read=fread(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(integers_read!=1)
	{
		return PDS_FILE_ERROR;
	}

	int entries=repo_handle.rec_count;
	struct BST_Node *head=NULL;

	//Iterating through all the available records in the index file.
	for(int i=0;i<entries;i++)
	{
		//Using a temporary struct to store a single entry.
		struct PDS_NdxInfo *tmp_ndx=(struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));

		//This indicates that the end of file has been reached. 
		long curr_loc=ftell(repo_handle.pds_ndx_fp);
		if(curr_loc==EOF_location)
		{
			break;
		}

		//Returning an error status if there is an error reading the struct from the index file.
		int struct_read_status=fread(tmp_ndx,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		if(struct_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Adding the index entry read from the index file into the bst and returning an error code in case of any failure.
		int status=bst_add_node(&head,tmp_ndx->key,tmp_ndx);
		if(status!=BST_SUCCESS)
		{
			return PDS_FILE_ERROR;
		}
	}

	//Storing the head of the bst into the global struct and returning a success code.
	repo_handle.pds_bst=head;
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
		return PDS_REPO_CLOSED;
	}

	//Returning an error status if the record count is already greater than or equal to maximum size.
	if(repo_handle.rec_size >= MAX_NDX_SIZE)
	{
		return PDS_ADD_FAILED;
	}

	//Searching for a node with the given key. If the node exists and is deleted, then the attribute is changed and a success code is returned.
	struct BST_Node *deleted_node=bst_search(repo_handle.pds_bst,key);
	if(deleted_node!=NULL && (((struct PDS_NdxInfo *)deleted_node->data)->is_deleted==1))
	{
		((struct PDS_NdxInfo *)deleted_node->data)->is_deleted=0;
		return PDS_SUCCESS;
	}

	//Setting the location pointer to the end of the file and returning an error code in case of any failure.
	int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_END);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	struct PDS_NdxInfo *node_data=(struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
	node_data->key=key;
	node_data->offset=(int)ftell(repo_handle.pds_data_fp);
	node_data->is_deleted=0;

	//Adding a node with the key and offset into the bst. If the node already exists or if the root node is null, then the function returns a 'BST_DUP_KEY' or 'BST_NULL' status which is handled by simply returning the error status "PDS_ADD_FAILED".
	int add_node_status=bst_add_node(&repo_handle.pds_bst,key,node_data);
	if(add_node_status!=BST_SUCCESS)
	{
		return PDS_ADD_FAILED;
	}

	//Writing the key into the data file and handling errors if any.
	int key_written_status=fwrite(&key,sizeof(int),1,repo_handle.pds_data_fp);
	if(key_written_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Writing only the record into the data file and handling errors if any.
	int rec_written_status=fwrite(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(rec_written_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Incrementing the record count by one.
	repo_handle.rec_count++;

	//Returning a success code.
	return PDS_SUCCESS;
}

// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_ndx_key( int key, void *rec )
{
	//Returning an error if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_CLOSED;
	}

	//Searching for the node in the bst with the matching key. Returns an error status if the node is NULL or if the record has been deleted (which would mean that the record does not exist).
	struct BST_Node *searched_node=bst_search(repo_handle.pds_bst,key);
	if(searched_node==NULL)
	{
		return PDS_REC_NOT_FOUND;
	}
	if(((struct PDS_NdxInfo *)searched_node->data)->is_deleted==1)
	{
		return PDS_REC_NOT_FOUND;
	}

	//Storing the offset pointer of the node by converting it into an int pointer and setting the location pointer to the value pointed to by the offset pointer.
	int offset=((struct PDS_NdxInfo*)searched_node->data)->offset;
	int set_lp=fseek(repo_handle.pds_data_fp,offset,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the key from the data file and handling errors if any.
	int read_key;
	int key_read_status=fread(&read_key,sizeof(int),1,repo_handle.pds_data_fp);
	if(key_read_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the record from the data file and handling errors if any.
	int record_read_status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(record_read_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Returning a success status.
	return PDS_SUCCESS;
}

// Brute-force retrieval using an arbitrary search key
// io_count = 0
// Loop through data file till EOF
//	fread the key and record 
//	io_count++
//      Invoke the matcher with the current record and input non-ndx search key
//	if mathcher returns success, return the current record, else continue the loop
// end loop
int get_rec_by_non_ndx_key(void *non_ndx_key, void *rec, int (*matcher)(void *rec, void *non_ndx_key), int *io_count)
{
	//Returning an error status if the pds status is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_CLOSED;
	}

	//Setting the variable to zero.
	*io_count=0;

	//Setting the location pointer to the start of the data file.
	int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Comparing the records by the phone key and returning a success code if the record exists. Returns an error status if the record is not found.
	int size=repo_handle.rec_count;
	for(int i=0;i<size;i++)
	{
		//Reading the key from the data file and handling errors if any.
		int key_read;
		int key_read_status=fread(&key_read,sizeof(int),1,repo_handle.pds_data_fp);
		if(key_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Reading the record from the data file and handling errors if any.
		int record_read_status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
		if(record_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		(*io_count)++;

		//Comparing the given key with that of the record. On a match, a success status is returned if the node exists, else an error status is returned.
		int compare_status=matcher(rec,non_ndx_key);
		if(compare_status==0)
		{
			//Searching for the node with the key of the record.
			struct BST_Node *tmp=bst_search(repo_handle.pds_bst,key_read);

			//Returning an success status if the node exists and is not deleted, else returns an error.
			if(tmp!=NULL && (((struct PDS_NdxInfo*)tmp->data)->is_deleted==0))
			{
				return PDS_SUCCESS;
			}

			return PDS_REC_NOT_FOUND;
		}
	}

	//Returning an error status as there was no match from the data file.
	return PDS_REC_NOT_FOUND;

}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Store the number of records
// Unload the ndx_array into the index file by traversing it (overwrite the entire index file)
// Close the index file and data file
int pds_close()
{
	//Returning an error status if the repo_status is already set to 'PDS_REPO_CLOSED'.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_CLOSED;
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
	//The rec_count is not updated to track only the records which are not deleted because doing so will require updates in the 'get_rec_by_non_ndx' function since it iterates through the data file linearly. It is possible that a record is present at the end of the data file and since the function iterates linearly, the updated rec_count will not allow the function to read that record.
	int ints_written=fwrite(&repo_handle.rec_count,sizeof(int),1,repo_handle.pds_ndx_fp);
	if(ints_written!=1)
	{
		return PDS_NDX_SAVE_FAILED;
	}

	//Overwriting the index file with the record count followed by the index entries.
	int filling_ndx_file_status=fill_ndx(repo_handle.pds_bst);
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
	bst_destroy(repo_handle.pds_bst);

	return PDS_SUCCESS;
}

//A recursive function to traverse the given binary tree in pre-order traversal and write the struct PDS_NdxInfo into the index file.
int fill_ndx(struct BST_Node *root)
{
	//Base case of when the root is NULL and so there is nothing to write.
	if(root==NULL)
	{
		return PDS_SUCCESS;
	}

	if(((struct PDS_NdxInfo*)root->data)->is_deleted==0)
	{
		//Writing the struct into the index file and handling errors if any.
		int write_ndx_info_status=fwrite(root->data,sizeof(struct PDS_NdxInfo),1,repo_handle.pds_ndx_fp);
		if(write_ndx_info_status!=1)
		{
			return PDS_NDX_SAVE_FAILED;
		}
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

// delete by ndx_key
int delete_rec_by_ndx_key( int key )
{
	//Searching for the node with the given key. If the node doesn't exist or if the node exists and has been deleted, then an error status is returned.
	struct BST_Node *searched_node=bst_search(repo_handle.pds_bst,key);
	if(searched_node==NULL)
	{
		return PDS_DELETE_FAILED;
	}
	else if(((struct PDS_NdxInfo*)searched_node->data)->is_deleted==1)
	{
		return PDS_DELETE_FAILED;
	}

	((struct PDS_NdxInfo*)searched_node->data)->is_deleted=1;
	return PDS_SUCCESS;
}
