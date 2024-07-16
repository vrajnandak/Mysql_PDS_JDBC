#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

#include"pds.h"

// Define the global variable
struct PDS_RepoInfo repo_handle;

//Creating an empty file with name '<repo_name>.dat' by opening the file in mode 'wb+'. This creates the file if it doesn't exist and overwrites the contents of the file if it already exists, all in binary format.
int pds_create(char *repo_name)
{
	//Appending the '.dat' extension to the filename.
	char filename[30];
	strcpy(filename,repo_name);
	strcat(filename,".dat");

	//Returning PDS_FILE_ERROR if there is an error opening the file.
	FILE *fp=fopen(filename,"wb+");
	if(fp==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the file and returning the code for 'success'.
	int close_val=fclose(fp);
	if(close_val!=0)
	{
		return PDS_FILE_ERROR;
	}
	return PDS_SUCCESS;
}

//The following method opens the '<repo_name>.dat' file and initializes the members of the global struct 'repo_handle'.
int pds_open( char *repo_name, int rec_size  )
{
	//Opening the file with name '<repo_name>.dat' in 'rb+' to enable reading and writing, from and to the file in binary format.
	char filename[30];
	strcpy(filename,repo_name);
	strcat(filename,".dat");
	strcpy(repo_handle.pds_name,filename);

	//Handling and setting the repo status.
	if(repo_handle.repo_status==PDS_REPO_ALREADY_OPEN)
	{
		return PDS_FILE_ERROR;
	}
	
	FILE *fptr=fopen(repo_handle.pds_name,"rb+");
	repo_handle.pds_data_fp=fptr;
	repo_handle.rec_size=rec_size;

	//Handling for when the pointer is not a valid pointer and changing the repo_status accordingly.
	repo_handle.repo_status=PDS_REPO_OPEN;
	if(fptr==NULL)
	{
		repo_handle.repo_status=PDS_REPO_CLOSED;
		return PDS_FILE_ERROR;
	}

	return PDS_SUCCESS;
}

//This method writes the given key,record into the file (in binary format) at the end of the file and returns an error code if either the file is closed or there is an error when writing to the file.
int put_rec_by_key( int key, void *rec )
{
	//Returning an error code if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_FILE_ERROR;
	}

	//Returning an error code if the FILE pointer is not a valid pointer.
	FILE *fp=repo_handle.pds_data_fp;
	if(fp==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Moving the location pointer to the end of the file
	int move_location_ptr=fseek(fp,0,SEEK_END);
	if(move_location_ptr!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Writing the key first into the file. Returns an error code if there is an error writing the key.
	int writing_key=fwrite(&key,sizeof(int),1,fp);
	if(writing_key!=1)
	{
		return PDS_ADD_FAILED;
	}

	//Writing the record into the file. Returns an error code if there is an error writing the record.
	int writing_rec=fwrite(rec,repo_handle.rec_size,1,fp);
	if(writing_rec!=1)
	{
		return PDS_ADD_FAILED;
	}

	//Returning the success code if there are no errors writing into the file.
	return PDS_SUCCESS;
}

//This method searches through the entire file, searching for key matching the given parameter. If there is a match, the corresponding record is stored into the pointer 'rec'.
int get_rec_by_key( int key, void *rec )
{
	//Returning an error code if the file is closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_FILE_ERROR;
	}

	//Returning an error code if the FILE pointer is not a valid pointer.
	FILE *fp=repo_handle.pds_data_fp;
	if(fp==NULL)
	{
		return PDS_FILE_ERROR;
	}

	//Moving the location pointer to the beggining of the file to read through the entire file.
	int move_location_ptr=fseek(fp,0,SEEK_SET);
	if(move_location_ptr!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Variables to store the key read from the file, outputs of the fread() function for reading the key and then the record from the file respectively.
	int key_read=-1;
	char read_result;
	char read_rec;

	//Reading through till the given key is found. The loop stops execution either when there is an error reading from the file or when the file reaches the EOF.
	do
	{
		read_result=fread(&key_read,sizeof(int),1,fp);
		read_rec=fread(rec,repo_handle.rec_size,1,fp);

		if((read_result==EOF || read_result!=1) || (read_rec==EOF || read_rec!=1))
		{
			return PDS_REC_NOT_FOUND;
		}

		//Returning success code if the key is found in the file.
		if(key_read==key)
		{
			return PDS_SUCCESS;
		}
	}while(1);

	//Returning an error code if the key is not found in the entire file.
	return PDS_REC_NOT_FOUND;
}

//This method closes the file and updates the members of the global struct 'repo_handle'.
int pds_close()
{
	//Returning an error code if the file is already closed.
	if(repo_handle.repo_status==PDS_REPO_CLOSED)
	{
		return PDS_FILE_ERROR;
	}

	//Closing the file and checking if the file was able to close without errors.
	int close_val=fclose(repo_handle.pds_data_fp);
	if(close_val!=0)
	{
		return PDS_FILE_ERROR;
	}

	//Updating the members of the global struct.
	strcpy(repo_handle.pds_name,"\0");
	repo_handle.repo_status=PDS_REPO_CLOSED;
	repo_handle.rec_size=0;
	repo_handle.pds_data_fp=NULL;

	//Returning a success code.
	return PDS_SUCCESS;
}
