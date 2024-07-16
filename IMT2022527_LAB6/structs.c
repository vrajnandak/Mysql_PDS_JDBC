#include<stdio.h>

#include"structs.h"
#include"parent.h"
#include"child.h"
#include"pds.h"

//Adds a link between the faculty and the course and performs additional error handling.
int add_link(int parent_key,int child_key)
{
	//Checking if the parent struct exists.
	struct Faculty tmp_faculty;
	int check_faculty_status=search_faculty(parent_key,&tmp_faculty);
	if(check_faculty_status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Unable to add link %d to %d\n",parent_key,child_key);
		return STRUCTS_FAILURE;
	}

	//Checking if the child struct exists.
	struct Course tmp_course;
	int check_course_status=search_course(child_key,&tmp_course);
	if(check_course_status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Unable to add link %d to %d\n",parent_key,child_key);
		return STRUCTS_FAILURE;
	}

	//Checking to see if there already exists a link between the given keys. If it exists, then no link is added.
	int child_keys[MAX_CHILD_KEYS];
	int num_of_keys=0;
	int get_links_status=get_links_of_faculty(parent_key,child_keys,&num_of_keys);
	for(int i=0;i<num_of_keys;i++)
	{
		if(child_key==child_keys[i])
		{
			fprintf(stderr,"Link already exists. Faculty key %d and Course key %d\n",parent_key,child_key);
			return STRUCTS_FAILURE;
		}
	}

	//Adding the link between the given keys.
	int add_link_status=pds_link_rec(parent_key,child_key);
	if(add_link_status!=PDS_SUCCESS)
	{
		fprintf(stderr,"Error adding link. Faculty key %d and Course key %d\n",parent_key,child_key);
		return STRUCTS_FAILURE;
	}

	return STRUCTS_SUCCESS;
}

//Gets the links with the given Faculty key
int get_links(int parent_key,int child_keys[],int *actual_io)
{
	//Checking if the parent struct exists.
	struct Faculty tmp_faculty;
	int search_faculty_status=search_faculty(parent_key,&tmp_faculty);
	if(search_faculty_status!=PDS_SUCCESS)
	{
		return STRUCTS_FAILURE;
	}

	//Getting the links of the Faculty.
	int status=get_links_of_faculty(parent_key,child_keys,actual_io);
	if(status!=PDS_SUCCESS)
	{
		return STRUCTS_FAILURE;
	}

	//Checking if all the Courses that the parent is linked to exist.
	struct Course tmp_course;
	int size=*actual_io;
	for(int i=0;i<size;i++)
	{
		int check_course_status=search_course(child_keys[i],&tmp_course);
		if(check_course_status!=PDS_SUCCESS)
		{
			return STRUCTS_FAILURE;
		}
	}

	return status;
}
