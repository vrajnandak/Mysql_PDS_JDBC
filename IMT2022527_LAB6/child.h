#ifndef CHILD_H
#define CHILD_H

#define STRUCTS_SUCCESS 0
#define STRUCTS_FAILURE 1

typedef struct Course
{
	char name[30];
	int credits;
	int id;
}Course;

//Add the given Child record to the repository by calling the appropriate put rec function.
int add_course(struct Course *rec);

//Printing the information of the course record.
void print_course(struct Course *rec);

//Searching for the course record from the file using appropriate pds function.
int search_course(int course_id,struct Course *rec);

#endif
