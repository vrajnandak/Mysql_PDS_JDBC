#ifndef PARENT_H
#define PARENT_H

#define STRUCTS_SUCCESS 0
#define STRUCTS_FAILURE 1
#define IS_NULL 2
#define CONVERSION_ERROR 3

typedef struct Faculty
{
	char name[30];
	int id;
	int age;
	int gender;	//0-Male, 1-Female
}Faculty;

//Add the given Faculty record to the repository by calling the put rec function.
int add_faculty(struct Faculty *rec);

//Printing the Faculty information.
void print_faculty(struct Faculty *rec);

//Searching for the faculty using the appropriate pds function.
int search_faculty(int faculty_id,struct Faculty *rec);

//Searching for the faculty using non_ndx_key using the appropriate pds function.
int search_faculty_by_name(char *faculty_name,struct Faculty *rec,int *io_count);

//Returns 0 if there is a match, else returns 1. Returns 2,3 depending on error.
int match_faculty_name(void *rec,void *key);

//Delete the faculty record from the repository.
int delete_faculty(int faculty_id);

//Getting the links of the given Faculty id.
int get_links_of_faculty(int faculty_id,int *child_keys,int *result_set_size);

#endif
