#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_CHILD_KEYS 30

//Adds a link between the faculty and the course.
int add_link(int parent_key,int child_key);

//Gets the links with the given Faculty key
int get_links(int parent_key,int child_keys[],int *actual_io);

#endif
