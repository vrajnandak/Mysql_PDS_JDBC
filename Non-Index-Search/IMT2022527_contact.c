#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "pds.h"
#include "contact.h"

// Load all the contacts from a CSV file
int store_contacts( char *contact_data_file )
{
	FILE *cfptr;
	char contact_line[500], token;
	struct Contact c, dummy;

	cfptr = (FILE *) fopen(contact_data_file, "r");
	while(fgets(contact_line, sizeof(contact_line)-1, cfptr)){
		//printf("line:%s",contact_line);
		sscanf(contact_line, "%d%s%s", &(c.contact_id),c.contact_name,c.phone);
		print_contact( &c );
		add_contact( &c );
	}
}

void print_contact( struct Contact *c )
{
	printf("%d,%s,%s\n", c->contact_id,c->contact_name,c->phone);
}

// Use get_rec_by_key function to retrieve contact
int search_contact( int contact_id, struct Contact *c )
{
	return get_rec_by_ndx_key( contact_id, c );
}

// Add the given contact into the repository by calling put_rec_by_key
int add_contact( struct Contact *c )
{
	int status;

	status = put_rec_by_key( c->contact_id, c );

	if( status != PDS_SUCCESS ){
		fprintf(stderr, "Unable to add contact with key %d. Error %d", c->contact_id, status );
		return CONTACT_FAILURE;
	}
	return status;
}

// Use get_rec_by_non_ndx_key function to retrieve contact
// Hint: get_rec_by_non_ndx_key( phone, c, &match_contact_phone, io_count );
// Return CONTACT_SUCCESS if contact is found, CONTACT_FAILURE if not found
int search_contact_by_phone( char *phone, struct Contact *c, int *io_count )
{
	//Checking whether the record with the given phone exists in the data file and returning the appropriate status code.
	int search_status=get_rec_by_non_ndx_key(phone,c,match_contact_phone,io_count);
	if(search_status==PDS_SUCCESS)
	{
		return CONTACT_SUCCESS;
	}

	//Returning a random error code when searching for contact by phone fails.
	return CONTACT_FAILURE;
}

/* Return 0 if phone of the contact matches with phone parameter */
/* Return 1 if phone of the contact does NOT match */
/* Return > 1 in case of any other error */
int match_contact_phone( void *rec, void *key )
{

	// Store the rec in a struct contact pointer
	// Store the key in a char pointer
	// Compare the phone values in key and record
	// Return 0,1,>1 based on above condition
	
	//Checking if either of the key or record pointers are pointing to NULL and returning an error status > 1.
	if(rec==NULL || key==NULL)
	{
		return 2;
	}

	//Type casting the record into struct Contact and returning an error status if there is an error while typecasting.
	struct Contact *tmp_rec=(struct Contact*)rec;
	if(sizeof(*tmp_rec)!=sizeof(struct Contact))
	{
		return 3;
	}

	//Type casting the key into a char pointer.
	char *key_string=(char*)key;

	//Returning 1 if the key and the phone of the record do not match and returning 0 if they do match.
	return (strcmp(tmp_rec->phone,key_string))?1:0;
}

