#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"pds.h"
#include"parent.h"

//Add the given record to the repository by calling the put rec function.
int add_faculty(struct Faculty *rec)
{
	int status=put_rec_by_key(rec->id,rec);
	if(status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Unable to add faculty with key %d. Error %d\n",rec->id,status);
		return STRUCTS_FAILURE;
	}
	return status;
}

//Printing the Faculty information.
void print_faculty(struct Faculty *rec)
{
	if(rec==NULL)
	{
		fprintf(stderr,"Unable to print Faculty details, it is NULL\n");
		return;
	}

	printf("\nFaculty name: %s\n",rec->name);
	printf("Faculty id: %d\n",rec->id);
	printf("Faculty age: %d\n",rec->age);
	printf("Faculty gender: %d\n",rec->gender);
}

//Searching for the faculty using the appropriate pds function.
int search_faculty(int faculty_id,struct Faculty *rec)
{
	int status=get_rec_by_ndx_key(faculty_id,rec);
	if(status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Error searching for faculty %d\n",faculty_id);
	}
	return status;
}

//Searching for the faculty using non_ndx_key using the appropriate pds function.
int search_faculty_by_name(char *faculty_name,struct Faculty *rec,int *io_count)
{
	int search_status=get_rec_by_non_ndx_key(faculty_name,rec,match_faculty_name,io_count);
	if(search_status==PDS_SUCCESS)
	{
		return STRUCTS_SUCCESS;
	}
	return STRUCTS_FAILURE;
}

//Returns 0 if there is a match, else returns 1. Returns 2,3 depending on error.
int match_faculty_name(void *rec,void *key)
{
	if(rec==NULL || key==NULL)
	{
		return IS_NULL;
	}

	struct Faculty *faculty=(struct Faculty*)rec;
	if((faculty==NULL) || sizeof(*faculty)!=sizeof(struct Faculty))
	{
		return CONVERSION_ERROR;
	}

	char *faculty_name=(char *)key;

	return (strcmp(key,faculty->name))?STRUCTS_FAILURE:STRUCTS_SUCCESS;
}

//Delete the faculty record from the repository.
int delete_faculty(int faculty_id)
{
	int delete_status=delete_rec_by_ndx_key(faculty_id);
	if(delete_status==PDS_SUCCESS)
	{
		return STRUCTS_SUCCESS;
	}
	return STRUCTS_FAILURE;
}

//Getting the links of the given Faculty id.
int get_links_of_faculty(int faculty_id,int *child_keys,int *result_set_size)
{
	int status=pds_get_linked_rec(faculty_id,child_keys,result_set_size);
	if(status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Error retrieving the course keys of Faculty %d\n",faculty_id);
		return STRUCTS_FAILURE;
	}
	return status;
}
