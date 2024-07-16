#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "structs.h"
#include "parent.h"
#include "child.h"

#define TREPORT(a1,a2) printf("Status: %s - %s\n\n",a1,a2); fflush(stdout);

void process_line( char *test_case );

int main(int argc, char *argv[])
{
	FILE *cfptr;
	char test_case[50];

	if( argc != 2 ){
		fprintf(stderr, "Usage: %s testcasefile\n", argv[0]);
		exit(1);
	}

	cfptr = (FILE *) fopen(argv[1], "r");
	while(fgets(test_case, sizeof(test_case)-1, cfptr)){
		// printf("line:%s",test_case);
		if( !strcmp(test_case,"\n") || !strcmp(test_case,"") )
			continue;
		process_line( test_case );
	}
}

void process_line( char *test_case )
{
	//Variables for parsing the line and to store information.
	char repo_name[30],linked_repo_name[30];
	char command[30], param1[30], param2[30], param3[30], param4[30], info[1024];
	int status,rec_size,linked_rec_size,expected_status;
	int parent_key,child_key;
	int faculty_id;

	//Variables of parent and child structs.
	Faculty testFaculty;
	Course testCourse;

	//Storing the sizes of the parent and child structs.
	rec_size = sizeof(struct Faculty);
	linked_rec_size=sizeof(struct Course);

	//Parsing the line.
	sscanf(test_case, "%s%s%s%s%s", command, param1, param2,param3,param4);
	printf("Test case: %s", test_case); fflush(stdout);

	if(!strcmp(command,"CREATE"))
	{
		strcpy(repo_name,param1);

		//If the second parameter is either 0 or 1, then it is the expected return value.
		if(!strcmp(param2,"0") || !strcmp(param2,"1"))
		{
			expected_status=(!strcmp(param2,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
			status=pds_create(repo_name,NULL);
		}
		else		//The second parameter is the name of the linked repository.
		{
			strcpy(linked_repo_name,param2);
			expected_status=(!strcmp(param3,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
			status=pds_create(repo_name,linked_repo_name);
		}

		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS", "");
		}
		else
		{
			sprintf(info,"pds_create returned status %d",status);
			TREPORT("FAIL", info);
		}
	}
	else if(!strcmp(command,"OPEN"))
	{
		strcpy(repo_name,param1);

		//If the second parameter is 0 or 1, then it is the expected return value.
		if(!strcmp(param2,"0") || !strcmp(param2,"1"))
		{
			expected_status=(!strcmp(param2,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
			status=pds_open(repo_name,NULL,rec_size,linked_rec_size);
		}
		else		//The second parameter is the name of the linked repository.
		{
			strcpy(linked_repo_name,param2);
			expected_status=(!strcmp(param3,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
			status=pds_open(repo_name,linked_repo_name,rec_size,linked_rec_size);
		}

		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS", "");
		}
		else
		{
			sprintf(info,"pds_open returned status %d",status);
			TREPORT("FAIL",info);
		}
	}
	else if(!strcmp(command,"STORE_PARENT"))
	{
		//Storing the parent struct information into the parent struct variable.
		sscanf(param1,"%d",&testFaculty.id);
		sscanf(param2,"%d",&testFaculty.age);
		sscanf(param3,"%d",&testFaculty.gender);
		sprintf(testFaculty.name,"Name-of-Faculty-%d",testFaculty.id);

		expected_status=(!strcmp(param4,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=add_faculty(&testFaculty);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS","");
		}
		else
		{
			sprintf(info,"add parent returned status %d",status);
			TREPORT("FAIL",info);
		}
	}
	else if(!strcmp(command,"STORE_CHILD"))
	{
		//Storing the child struct information into the child struct variable.
		sscanf(param1,"%d",&testCourse.id);
		sscanf(param2,"%d",&testCourse.credits);
		sprintf(testCourse.name,"Name-of-Course-%d",testCourse.id);

		expected_status=(!strcmp(param3,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=add_course(&testCourse);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS","");
		}
		else
		{
			sprintf(info,"add child returned status %d",status);
			TREPORT("FAIL",info);
		}
	}
	else if(!strcmp(command,"STORE_LINK"))
	{
		//Storing the link information into the respective variables.
		sscanf(param1,"%d",&parent_key);
		sscanf(param2,"%d",&child_key);

		expected_status=(!strcmp(param3,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=add_link(parent_key,child_key);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS","");
		}
		else
		{
			sprintf(info,"add link returned status %d",status);
			TREPORT("FAIL",info);
		}
	}
	else if(!strcmp(command,"NDX_SEARCH"))
	{
		sscanf(param1,"%d",&faculty_id);
		testFaculty.id=-1;

		expected_status=(!strcmp(param2,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=search_faculty(faculty_id,&testFaculty);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		
		if(status!=expected_status)
		{
			sprintf(info,"search key: %d; Got status %d",faculty_id,status);
			TREPORT("FAIL",info);
		}
		else if(expected_status==0)		//The return value matches and it's a success.
		{
			char expected_name[30];
			sprintf(expected_name,"Name-of-Faculty-%d",faculty_id);
			if(testFaculty.id==faculty_id && strcmp(testFaculty.name,expected_name)==0)		//Comparing only the name and id of the faculty. The testcase passes if it's a match.
			{
				TREPORT("PASS","");
			}
			else
			{
				sprintf(info,"Faculty data not matching... Expected:{%d,%s} Got:{%d,%s}\n",faculty_id,expected_name,testFaculty.id,testFaculty.name);
				TREPORT("FAIL",info);
			}
		}
		else		//The return value matches and it's a failure.
		{
			TREPORT("PASS","");
		}
	}
	else if(!strcmp(command,"NON_NDX_SEARCH"))
	{
		int expected_io,actual_io;
		char faculty_name[30];

		sscanf(param1,"%s",faculty_name);
		sscanf(faculty_name,"Name-of-Faculty-%d",&faculty_id);
		sscanf(param2, "%d", &expected_io);
		testFaculty.id=-1;

		actual_io=0;

		expected_status=(!strcmp(param2,"-1"))?STRUCTS_FAILURE:STRUCTS_SUCCESS;
		status=search_faculty_by_name(faculty_name,&testFaculty,&actual_io);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status!=expected_status)
		{
			sprintf(info,"search key: %s; Got status %d",faculty_name,status);
			TREPORT("FAIL",info);
		}
		else if(expected_status==0)		//The return value matches and it's a success.
		{
			if(testFaculty.id==faculty_id && !strcmp(testFaculty.name,faculty_name))	//Comparing only the id and the name of the faculty.
			{
				if(expected_io==actual_io)
				{
					TREPORT("PASS","");
				}
				else
				{
					sprintf(info,"Num I/O not matching for faculty %d... Expected:%d Got:%d\n",faculty_id,expected_io,actual_io);
					TREPORT("FAIL",info);
				}
			}
			else
			{
				sprintf(info,"Faculty data not matching... Expected:{%d,%s} Got:{%d,%s}\n",faculty_id,faculty_name,testFaculty.id,testFaculty.name);
				TREPORT("FAIL",info);
			}
		}
		else		//The return value matches and it's a failure.
		{
			TREPORT("PASS","");
		}
	}
	else if(!strcmp(command,"SEARCH_CHILD"))
	{
		char expected_child_name[30];

		sscanf(param1,"%d",&child_key);
		sprintf(expected_child_name,"Name-of-Course-%d",child_key);
		testCourse.id=-1;

		expected_status=(!strcmp(param2,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=search_course(child_key,&testCourse);
		status=(status!=PDS_SUCCESS)?STRUCTS_FAILURE:STRUCTS_SUCCESS;

		if(status!=expected_status)
		{
			sprintf(info,"search key: %d; Got status %d",child_key,status);
			TREPORT("FAIL",info);
		}
		else if(expected_status==0)		//The return value matches and it's a success.
		{
			if(!strcmp(testCourse.name,expected_child_name) && testCourse.id==child_key)		//Comparing only the name and the id of the Course.
			{
				TREPORT("PASS","");
			}
			else
			{
				sprintf(info,"Child data not matching... Expected:{%d,%s} Got:{%d,%s}\n",child_key,expected_child_name,testCourse.id,testCourse.name);
				TREPORT("FAIL",info);
			}
		}
		else		//The return value matches and it's a failure.
		{
			TREPORT("PASS","");
		}
	}
	else if(!strcmp(command,"NDX_DELETE"))
	{
		sscanf(param1,"%d",&faculty_id);
		testFaculty.id=-1;

		expected_status=(!strcmp(param2,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=delete_faculty(faculty_id);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status!=expected_status)
		{
			sprintf(info,"delete key: %d; Got status %d",faculty_id,status);
			TREPORT("FAIL",info);
		}
		else
		{
			TREPORT("PASS","");
		}
	}
	else if(!strcmp(command,"GET_LINKS"))
	{

		sscanf(param1,"%d",&parent_key);
		char *ptr_child_keys=(char*)malloc((sizeof(char)*strlen(param2))+1);
		int expected_child_keys[10];
		int expected_links=0;
		int num;

		//Extracting the expected child keys.
		strcpy(ptr_child_keys,param2);
		while(sscanf(ptr_child_keys,"%d_",&num)==1)
		{
			expected_child_keys[expected_links]=num;
			expected_links++;
			ptr_child_keys=strchr(ptr_child_keys,'_');
			if(ptr_child_keys==NULL)
			{
				break;
			}
			ptr_child_keys++;
			if(ptr_child_keys==NULL)
			{
				break;
			}
		}

		int result_child_keys[10];
		int result_links=0;

		expected_status=(!strcmp(param3,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=get_links(parent_key,result_child_keys,&result_links);
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status!=expected_status)
		{
			sprintf(info," parent key: %d Got status: %d",parent_key,status);
			TREPORT("FAIL",info);
		}
		else if(expected_status==0)		//The return value matches and it's a success.
		{
			if(result_links==expected_links)		//The number of links matches, so every link is checked to see if it matches with the expected links.
			{
				int are_equal=1;
				for(int i=0;i<expected_links;i++)
				{
					if(result_child_keys[i]!=expected_child_keys[i])
					{
						sprintf(info,"%dth Child key: %d, %dth Expected Child key: %d\n",i,result_child_keys[i],i,expected_child_keys[i]);
						TREPORT("FAIL",info);
						are_equal=0;
						break;
					}
				}
				if(are_equal)
				{
					TREPORT("PASS","");
				}
			}
			else
			{
				sprintf(info,"Child keys not matching. Expected number of child keys:%d , Got: %d\n",expected_links,result_links);
				TREPORT("FAIL",info);
			}
		}
		else		//The return value matches and it's a failure.
		{
			TREPORT("PASS","");
		}
	}
	else if(!strcmp(command,"CLOSE"))
	{
		expected_status=(!strcmp(param1,"0"))?STRUCTS_SUCCESS:STRUCTS_FAILURE;
		status=pds_close();
		status=(status==PDS_SUCCESS)?STRUCTS_SUCCESS:STRUCTS_FAILURE;

		if(status==expected_status)
		{
			TREPORT("PASS","");
		}
		else
		{
			sprintf(info,"pds_close returned status %d",status);
			TREPORT("FAIL",info);
		}
	}
	else if(!strcmp(command,"READ_ALL"))		//A utility command to print all the contents of the faculty and course data files.
	{
		read_all();
	}
}
