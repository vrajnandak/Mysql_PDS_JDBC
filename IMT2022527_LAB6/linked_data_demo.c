#include<stdio.h>
#include<string.h>

// Include the header file that contains the struct declarations for parent and child records
#include"structs.h"
#include"parent.h"
#include"child.h"

#include"pds.h"

// Declare global variables for 10 parent and 10 child records
Faculty faculty_records[10];
Course course_records[10];

void show_menu();
void setup_data();
void process_option( int option );
void process_option1();
void process_option2();

int main()
{
    int option;
    setup_data();
    do{
        show_menu();
        scanf("%d",&option);
        process_option(option);
    } while (option != 0);

    pds_close();
    return 0;
}

void show_menu()
{
    printf("\nLINKED DATA DEMO\n\n");
    printf("0. Exit\n");
    printf("1. Add linked data\n");
    printf("2. Get linked data\n");
    printf("\nEnter option: ");  
}

void setup_data()
{
	//Creating 2 strings to hold the names of the repository and the linked repository.
	char repo_name[30];
	char linked_repo_name[30];

	strcpy(repo_name,"Faculty");
	strcpy(linked_repo_name,"Courses");

	//Getting the record size of both the structs.
	int rec_size=sizeof(struct Faculty);
	int linked_rec_size=sizeof(struct Course);

	//Creating the Records.
	for(int i=0;i<10;i++)
	{
		//Filling the ith parent record.
		sprintf(faculty_records[i].name,"Parent-%d",i+1);
		faculty_records[i].id=i+1;
		faculty_records[i].age=28+i;
		faculty_records[i].gender=(i%2);

		//Filling the ith child record.
		sprintf(course_records[i].name,"Child-%d",i+1);
		course_records[i].credits=(i%3==0)?3:4;
		course_records[i].id=i+1;
	}

	//Calling the appropriate PDS functions to create the data files.
	int create_files_status=pds_create("Faculty","Courses");
	if(create_files_status!=PDS_SUCCESS)
	{
		printf("Error creating the files\n");
	}
	
	//Calling the appropriate PDS functions to open the data file.
	int open_files_status=pds_open("Faculty","Courses",sizeof(struct Faculty),sizeof(struct Course));
	if(open_files_status!=PDS_SUCCESS)
	{
		printf("Error opening the files\n");
	}

	//Adding the Faculty and Course records.
	for(int i=1;i<11;i++)
	{
	    add_faculty(&faculty_records[i-1]);		//Adding the parent record.
	    add_course(&course_records[i-1]);		//Adding the child record.
	}
}

void process_option( int option )
{
    switch(option){
        case 0: 
            // do nothing
            break;
        case 1:
            process_option1();
            break;
        case 2:
            process_option2();
            break;
        default:
            printf("Invalid option\n");
            break;
    }
}

void process_option1()
{
    int parent_key, child_key;
    printf("Enter parent key and child key for linking: ");
    scanf("%d%d", &parent_key, &child_key);
    // TODO: Call appropriate PDS functions here to link the two records
    
    add_link(parent_key,child_key);	//Adding the link between the parent and the child records.

}

void process_option2()
{
    int parent_key;
    printf("Retrieve the links of parent key: ");
    scanf("%d", &parent_key);
    // TODO: Call appropriate PDS functions here to fetch arrach of child keys
    // Call your PDS functions to display the list of child keys retrieved for the given parent key
    // Display all field values of each child record in a nicely formatted manner

    int child_keys[10];
    int child_keys_retrieved=0;
    get_links(parent_key,child_keys,&child_keys_retrieved);		//Retrieving the links of the given parent key.

    Course child_course;
    for(int i=0;i<child_keys_retrieved;i++)
    {
	    int get_child_record_status=search_course(child_keys[i],&child_course);
	    if(get_child_record_status==PDS_SUCCESS)
	    {
		    print_course(&child_course);
	    }
    }
}
