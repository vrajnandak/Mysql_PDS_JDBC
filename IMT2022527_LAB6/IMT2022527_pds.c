#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pds.h"
#include "bst.h"
#include "structs.h"
#include "parent.h"
#include "child.h"

struct PDS_RepoInfo repo_handle;

int fill_ndx(struct BST_Node *root);		//A function to fill the index file in pre-order traversal of the keys of the nodes of the bst tree.
void read_all();				//A utility function to read all the data present in the data files of the parent structs and child structs.

// pds_create
// Open the data file and index file in "wb" mode
// Initialize index file by storing "0" to indicate there are zero entries in index file
// close the files
int pds_create(char *repo_name, char *linked_repo_name)
{
	//Returning error status if the repo is not well defined.
	if(repo_name==NULL)
	{
		return PDS_REPO_NAME_IS_NULL;
	}

	//Creating 2 strings to hold the filenames with the extensions, '.dat' and the '.ndx'.
	char filename_data[30];
	char filename_ndx[30];

	strcpy(filename_data,repo_name);
	strcpy(filename_ndx,repo_name);

	strcat(filename_data,".dat");
	strcat(filename_ndx,".ndx");

	//Opening the data file in 'wb' mode to create the file if it doesn't already exist.
	FILE *pds_data=fopen(filename_data,"wb");
	if(pds_data==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Opening the ndx file in 'wb' mode to create the file if it doesn't already exist.
	FILE *pds_ndx=fopen(filename_ndx,"wb");
	if(pds_ndx==NULL)
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

	//Creating a linked data file and a link data file.
	if(linked_repo_name!=NULL)
	{
		//Creating 2 strings to hold the filenames of the link file and the linked repo file.
		char link_file[60];
		char linked_file[30];

		strcpy(linked_file,linked_repo_name);
		strcat(linked_file,".dat");

		strcpy(link_file,repo_name);
		strcat(link_file,"_");
		strcat(link_file,linked_repo_name);
		strcat(link_file,".dat");

		//Opening the link file in 'wb' mode to create the file if it doesn't already exist.
		FILE *link_file_ptr=fopen(link_file,"wb");
		if(link_file_ptr==NULL)
		{
			return PDS_FILE_ERROR;
		}

		//Opening the linked file in 'wb' mode to create the file if it doesn't alraedy exist.
		FILE *linked_file_ptr=fopen(linked_file,"wb");
		if(linked_file_ptr==NULL)
		{
			return PDS_FILE_ERROR;
		}

		//Closing the link file.
		int link_close_status=fclose(link_file_ptr);
		if(link_close_status!=0)
		{
			return PDS_FILE_ERROR;
		}

		//Closing the linked file.
		int linked_close_status=fclose(linked_file_ptr);
		if(linked_close_status!=0)
		{
			return PDS_FILE_ERROR;
		}
	}

	return PDS_SUCCESS;
}

// pds_open
// Open the data file and index file in rb+ mode
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Load the index into the array and store in ndx_array by reading index entries from the index file
// Close only the index file
int pds_open( char *repo_name, char *linked_repo_name, int rec_size, int linked_rec_size )
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

	//Returning an error status if the repo is open.
	if(repo_handle.repo_status==PDS_REPO_OPEN)
	{
		return PDS_REPO_ALREADY_OPEN;
	}
	repo_handle.repo_status=PDS_REPO_OPEN;

	//Opening the data file in 'rb+' mode to read and write records.
	FILE *pds_data=fopen(filename_data,"rb+");
	if(pds_data==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Opening the ndx file in 'rb+' mode to read and write records.
	FILE *pds_ndx=fopen(filename_ndx,"rb+");
	if(pds_ndx==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Initializing the data,ndx file pointers of the global struct.
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

	//Closing the index file.
	int close_status_ndx=fclose(repo_handle.pds_ndx_fp);
	if(close_status_ndx!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Making the FILE pointer null as the file has been closed.
	repo_handle.pds_ndx_fp=NULL;

	//Opening the linked and link data files accordingly to whether the 'linked_repo_name' is NULL or not.
	if(linked_repo_name!=NULL)
	{
		//Creating 2 strings to hold the names of the link file and linked file.
		char link_file[60];
		char linked_file[30];

		strcpy(linked_file,linked_repo_name);
		strcat(linked_file,".dat");

		strcpy(link_file,repo_name);
		strcat(link_file,"_");
		strcat(link_file,linked_repo_name);
		strcat(link_file,".dat");


		//Opening the link file in 'rb+' mode to read and write records.
		FILE *link_file_ptr=fopen(link_file,"rb+");
		if(link_file_ptr==NULL)
		{
			return PDS_FILE_ERROR;
		}

		//Opening the linked file in 'rb+' mode to read and write records.
		FILE *linked_file_ptr=fopen(linked_file,"rb+");
		if(linked_file_ptr==NULL)
		{
			return PDS_FILE_ERROR;
		}

		//Initializing the linked repo related attributes.
		repo_handle.pds_link_fp=link_file_ptr;
		repo_handle.pds_linked_data_fp=linked_file_ptr;
		repo_handle.linked_rec_size=linked_rec_size;
	}
	else
	{
		repo_handle.pds_link_fp=NULL;
		repo_handle.pds_linked_data_fp=NULL;
		repo_handle.linked_rec_size=0;
	}

	return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
int pds_load_ndx()
{
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

	//Creating variables to hold the count of records in the ndx file and the root of the bst.
	int entries=repo_handle.rec_count;
	struct BST_Node *head=NULL;

	//Iterating through all the available records in the index file.
	for(int i=0;i<entries;i++)
	{
		//This indicates that the end of file has been reached. 
		long curr_loc=ftell(repo_handle.pds_ndx_fp);
		if(curr_loc==EOF_location)
		{
			break;
		}

		//Using a temporary struct to store a single entry.
		struct PDS_NdxInfo *tmp_ndx=(struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
		if(tmp_ndx==NULL)
		{
			return PDS_MALLOC_ERROR;
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
			return PDS_LOAD_NDX_FAILED;
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
	if(repo_handle.repo_status==PDS_REPO_CLOSED)	//Returning an error status if the file is closed.
	{
		return PDS_REPO_NOT_OPEN;
	}
	else if(repo_handle.rec_size>=MAX_NDX_SIZE)	//Returning an error status if the record count is already greater than or equal to maximum size.
	{
		return PDS_ADD_FAILED;
	}
	else if(repo_handle.pds_data_fp==NULL)		//Returning an error status if the data file pointer is NULL.
	{
		return PDS_REPO_NAME_IS_NULL;
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

	//Creating a temporary instace of the struct to store the key,offset and is_deleted flag to write into the data file.
	struct PDS_NdxInfo *node_data=(struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
	if(node_data==NULL)
	{
		return PDS_MALLOC_ERROR;
	}
	node_data->key=key;
	node_data->offset=(int)ftell(repo_handle.pds_data_fp);
	node_data->is_deleted=0;

	//Adding a node with the key,offset,is_deleted flag into the bst.
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

// put_linked_rec_by_key - NEW
// Seek to the end of the linked data file
// No need to create index entry
// Write the key at the current data file location
// Write the record after writing the key
int put_linked_rec_by_key( int key, void *rec )
{
	//Handling error cases.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_NOT_OPEN;
	}
	else if(repo_handle.rec_count>=MAX_NDX_SIZE)
	{
		return PDS_ADD_FAILED;
	}
	else if(repo_handle.pds_linked_data_fp==NULL)
	{
		return PDS_LINKED_DATA_FP_IS_NULL;
	}

	//Setting the location pointer to the end of the file.
	int set_lp_to_end=fseek(repo_handle.pds_linked_data_fp,0,SEEK_END);
	if(set_lp_to_end!=0)
	{
		return PDS_FILE_ERROR;
	}

	//First, writing the key into the data file.
	int key_written_status=fwrite(&key,sizeof(int),1,repo_handle.pds_linked_data_fp);
	if(key_written_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Writing the record into the data file.
	int rec_written_status=fwrite(rec,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
	if(rec_written_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
}

// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Read the record from the current location
int get_rec_by_ndx_key( int key, void *rec )
{
	//Handling the error cases.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_NOT_OPEN;
	}
	else if(repo_handle.pds_data_fp==NULL)
	{
		return PDS_REPO_NAME_IS_NULL;
	}

	//Searching for the given key in the bst.
	struct BST_Node *searched_node=bst_search(repo_handle.pds_bst,key);
	if(searched_node==NULL)
	{
		return PDS_REC_NOT_FOUND;
	}
	else if(((struct PDS_NdxInfo *)searched_node->data)->is_deleted==1)
	{
		return PDS_REC_NOT_FOUND;
	}

	//Setting the location pointer to the offset value.
	int offset=((struct PDS_NdxInfo*)searched_node->data)->offset;
	int set_lp=fseek(repo_handle.pds_data_fp,offset,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the key from the data file.
	int read_key;
	int key_read_status=fread(&read_key,sizeof(int),1,repo_handle.pds_data_fp);
	if(key_read_status!=1)
	{
		return PDS_FILE_ERROR;
	}

	//Reading the record from the file.
	int record_read_status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
	if(record_read_status!=1)
	{
		return PDS_FILE_ERROR;
	}

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
		return PDS_REPO_NOT_OPEN;
	}

	//Setting the variable to zero.
	*io_count=0;

	//Setting the location pointer to the start of the data file.
	int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_SET);
	if(set_lp!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Comparing the records by the name(non_ndx_key).
	int size=repo_handle.rec_count;
	for(int i=0;i<size;i++)
	{
		//Reading the key from the data file.
		int key_read;
		int key_read_status=fread(&key_read,sizeof(int),1,repo_handle.pds_data_fp);
		if(key_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Reading the record from the data file.
		int record_read_status=fread(rec,repo_handle.rec_size,1,repo_handle.pds_data_fp);
		if(record_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		(*io_count)++;

		//Comparing the records using the matcher function.
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

// get_linked_rec_by_key - NEW
// Do a linear search of the given key in the linked data file
int get_linked_rec_by_key( int key, void *rec )
{
	//Handling the error case.
	if(repo_handle.pds_linked_data_fp==NULL)
	{
		return PDS_LINKED_DATA_FP_IS_NULL;
	}

	//Getting the EOF location of the location pointer for the linked data file and then setting it to the start.
	int set_lp_to_end=fseek(repo_handle.pds_linked_data_fp,0,SEEK_END);
	if(set_lp_to_end!=0)
	{
		return PDS_FILE_ERROR;
	}
	long EOF_location=ftell(repo_handle.pds_linked_data_fp);
	int set_lp_to_start=fseek(repo_handle.pds_linked_data_fp,0,SEEK_SET);	
	if(set_lp_to_start!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Loop to linearly iterate through the contents of the linked data file.
	while(1)
	{
		//Breaking out of the loop if the EOF has been reached.
		long curr_loc=ftell(repo_handle.pds_linked_data_fp);
		if(curr_loc==EOF_location)
		{
			break;
		}

		int read_key;

		//Reading the key from the file.
		int read_key_status=fread(&read_key,sizeof(int),1,repo_handle.pds_linked_data_fp);
		if(read_key_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Reading the record from the file.
		int read_rec_status=fread(rec,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
		if(read_rec_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Returning a success status if the key read from the file is equal to the given key.
		if(read_key==key)
		{
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
	//Returning an error status if the repo_status is already set to 'PDS_REPO_CLOSED'.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_CLOSE_FAILED;
	}
	repo_handle.repo_status=PDS_REPO_CLOSED;

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
	repo_handle.pds_ndx_fp=wfptr;

	//Writing the count of the records. Returning an error status if there is an error writing into the file.
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

	//Freeing the bst.
	bst_destroy(repo_handle.pds_bst);

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

	//Closing the link data file.
	if(repo_handle.pds_link_fp!=NULL)
	{
		int close_link_fp=fclose(repo_handle.pds_link_fp);
		if(close_link_fp!=0)
		{
			return PDS_NDX_SAVE_FAILED;
		}
	}

	//Closing the linked data file.
	if(repo_handle.pds_linked_data_fp!=NULL)
	{
		int close_linked_data_fp=fclose(repo_handle.pds_linked_data_fp);
		if(close_linked_data_fp!=0)
		{
			return PDS_NDX_SAVE_FAILED;
		}
	}

	//Resetting the attributes of the global struct.
	strcpy(repo_handle.pds_name,"\0");
	repo_handle.pds_data_fp=NULL;
	repo_handle.pds_ndx_fp=NULL;
	repo_handle.rec_size=0;
	repo_handle.rec_count=0;
	repo_handle.pds_link_fp=NULL;
	repo_handle.pds_linked_data_fp=NULL;
	repo_handle.linked_rec_size=0;
	repo_handle.pds_bst=NULL;

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

	//Writing the struct only if it hasn't been deleted.
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

// delete by ndx_key - NO CHANGE
// NOTE: When we delete a key, we need to actually delete the linked records too.
// But we will ignore that part for now. 
// This function will now delete only from the main data
int delete_rec_by_ndx_key( int key )
{
	//Handling errors.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_REPO_NOT_OPEN;
	}

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

// pds_link_rec - NEW
// Create PDS_link_info instance based on key1 and key2
// Go to the end of the link file
// Store the PDS_link_info record
int pds_link_rec (int key1, int key2)
{
	//Handling error case.
	if(repo_handle.pds_link_fp==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Setting the location pointer to the end of the link file.
	int set_lp_to_end=fseek(repo_handle.pds_link_fp,0,SEEK_END);
	if(set_lp_to_end!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Creating an instance of the struct based off the parent and child key values.
	struct PDS_link_info *tmp=(struct PDS_link_info*)malloc(sizeof(struct PDS_link_info));
	if(tmp==NULL)
	{
		return PDS_MALLOC_ERROR;
	}

	tmp->parent_key=key1;
	tmp->child_key=key2;

	//Writing the struct into the link file.
	int link_rec_write_status=fwrite(tmp,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp);
	if(link_rec_write_status!=1)
	{
		return PDS_ADD_FAILED;
	}

	return PDS_SUCCESS;
}

// pds_get_linked_rec
// Go to the beginning of the link file
// Reset result_set_size to 0
// Do a linear search of all link_info records for matching the given parent_key
// Store the matching linked key in linked_keys_result array
// Keep updating the result_set_size
//int pds_get_linked_rec(int parent_key, int linked_keys_result[], &result_set_size);
int pds_get_linked_rec(int parent_key, int linked_keys_result[],int *result_set_size)
{
	//Handling error case.
	if(repo_handle.pds_link_fp==NULL)
	{
		return PDS_REPO_NOT_OPEN;
	}

	//Getting the EOF location of the location pointer for the link file and then setting it to the start.
	int set_lp_to_end=fseek(repo_handle.pds_link_fp,0,SEEK_END);
	if(set_lp_to_end!=0)
	{
		return PDS_FILE_ERROR;
	}
	long EOF_location=ftell(repo_handle.pds_link_fp);
	int set_lp_to_start=fseek(repo_handle.pds_link_fp,0,SEEK_SET);
	if(set_lp_to_start!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Setting the size to zero.
	*result_set_size=0;

	struct PDS_link_info tmp;

	//Loop to iterate through the contents of the link file linearly.
	while(1)
	{
		//Breaking out of the loop if the EOF has been reached.
		long curr_loc=ftell(repo_handle.pds_link_fp);
		if(curr_loc==EOF_location)
		{
			break;
		}

		//Reading the struct from the file.
		int structs_read_status=fread(&tmp,sizeof(struct PDS_link_info),1,repo_handle.pds_link_fp);
		if(structs_read_status!=1)
		{
			return PDS_FILE_ERROR;
		}

		//Adding the child key onto the given array if the parent key matches with that given. Size is incremented by one.
		if(tmp.parent_key==parent_key)
		{
			*(linked_keys_result+(*result_set_size))=tmp.child_key;
			*result_set_size = *result_set_size + 1;
		}
	}

	return PDS_SUCCESS;
}

//Utility function to simply read through all the contents of the data files of both the structs(Faculty and Course).
void read_all()
{
	//Reading through the contents of the parent data file.
	if(repo_handle.pds_data_fp!=NULL && repo_handle.repo_status==PDS_REPO_OPEN)
	{
		//Saving the current location of the pointer and then setting it to the start of the file.
		long orig_data=ftell(repo_handle.pds_data_fp);
		int set_lp=fseek(repo_handle.pds_data_fp,0,SEEK_SET);
		if(set_lp!=0)
		{
			printf("Reading Faculty.dat\n");
			for(int i=0;i<repo_handle.rec_count;i++)
			{
				int key;
				struct Faculty tmp;

				fread(&key,sizeof(int),1,repo_handle.pds_data_fp);
				fread(&tmp,repo_handle.rec_size,1,repo_handle.pds_data_fp);

				struct BST_Node *tmp_node=bst_search(repo_handle.pds_bst,key);

				if(tmp_node!=NULL && ((struct PDS_NdxInfo*)tmp_node->data)->is_deleted==0)
				{
					printf("key: %d and Faculty name: %s\n",key,tmp.name);
				}
			}
		}
		//Restoring the location of the pointer.
		fseek(repo_handle.pds_data_fp,0,orig_data);
	}

	//Reading through the contents of the child data file.
	if(repo_handle.pds_linked_data_fp!=NULL)
	{
		//Saving the current location of the pointer and then setting it to the start of the file.
		long orig_linked_data=ftell(repo_handle.pds_linked_data_fp);
		fseek(repo_handle.pds_linked_data_fp,0,SEEK_SET);
		printf("Reading Courses.dat\n");
		while(1)
		{
			int key;
			struct Course tmp;

			int obj_read_status=fread(&key,sizeof(int),1,repo_handle.pds_linked_data_fp);
			if(obj_read_status!=1)
			{
				printf("Done reading the file\n");
				break;
			}

			fread(&tmp,repo_handle.linked_rec_size,1,repo_handle.pds_linked_data_fp);
			printf("key: %d and Course name: %s\n",key,tmp.name);
		}

		//Restoring the location of the pointer.
		fseek(repo_handle.pds_linked_data_fp,0,orig_linked_data);
	}
}
