#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"child.h"
#include"pds.h"

//Add the given Child record to the repository by calling the appropriate put rec function.
int add_course(struct Course *rec)
{
	int status=put_linked_rec_by_key(rec->id,rec);
	if(status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Unable to add Course with key %d. Error %d\n",rec->id,status);
	}
	return status;
}

//Printing the information of the course record.
void print_course(struct Course *rec)
{
	if(rec==NULL)
	{
		fprintf(stderr,"Unable to print Course details, it is NULL\n");
		return;
	}

	printf("\nCourse name: %s\n",rec->name);
	printf("Course credits: %d\n",rec->credits);
	printf("Course id: %d\n",rec->id);
}

//Searching for the course record from the file using appropriate pds function.
int search_course(int course_id,struct Course *rec)
{
	int search_status=get_linked_rec_by_key(course_id,rec);
	if(search_status==PDS_SUCCESS)
	{
		return STRUCTS_SUCCESS;
	}

	fprintf(stderr,"Course %d does not exist\n",course_id);
	return STRUCTS_FAILURE;
}
