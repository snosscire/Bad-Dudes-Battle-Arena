#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "json.h"

int main()
{
	char *string = NULL;
	
	json_t* root = json_new_object();
	
	json_insert_pair_into_object(root, "first", json_new_number("1"));
	
	json_tree_to_string(root, &string);
	
	printf("%s\n", string);
}

