#include "include.h"

String * string_new() {
	String *string = malloc(sizeof(String));
	if( string )
	{
		memset(string, 0, sizeof(String));
		string->length = 0;
		string->size = 32;
		string->data = (char *) malloc(sizeof(char) * 32);
		if( NOT string->data )
		{
			free(string);
			return NULL;
		}
		memset(string->data, 0, sizeof(char) * 32);
		string->data[0] = '\0';
	}
	return string;
}

void string_free( String *string ) {
	if( string )
	{
		free(string->data);
		free(string);
	}
}

void string_clear( String *string ) {
	string->length = 0;
	string->size = 32;
	free(string->data);
	string->data = (char *) malloc(sizeof(char) * 32);
	memset(string->data, 0, sizeof(char) * 32);
}

void string_append( String *string, const char *str ) {
	int i;
	int j = string->length;
	int length = strlen(str);
	string->length += length;
	for( i = 0; i < length; i++ )
	{
		if( str[i] != '\0' )
		{
			string->data[j] = str[i];
			j++;
		}
	}
	string->data[string->length] = '\0';
}

void string_append_character( String *string, char c ) {
	string->data[string->length] = c;
	string->data[string->length + 1] = '\0';
	string->length++;
}

char string_character_at( String *string, unsigned int position ) {
	if( position < string->size ) {
		return string->data[position];
	}
	return 0;
}

int string_compare( String *string, const char *str ) {
	if( strcasecmp(string->data, str) == 0 )
		return TRUE;
	return FALSE;
}

void string_split ( String *string, String *first, String **secondary, int *num_secondary,
	char separator, int num_ignore, int max_secondary )
{
	if ( first AND secondary AND num_secondary )
	{
		int i;
		String *current = string_new();
		*num_secondary = 0;
		
		for ( i = num_ignore; i < string->length; i++ )
		{
			if ( string->data[i] != separator )
			{
				string_append_character(current, string->data[i]);
			}
			
			if ( (string->data[i] == separator OR i == (string->length - 1)) AND current->length > 0 )
			{
				if ( first->length == 0 )
				{
					string_append(first, current->data);
					string_clear(current);
				}
				else
				{
					String *second = string_new();
					string_append(second, current->data);
					
					secondary[*num_secondary] = second;
					
					*num_secondary = *num_secondary + 1;
					
					string_clear(current);
					
					if ( *num_secondary == max_secondary )
						break;
				}
			}
		}
		
		string_free(current);
	}
}

