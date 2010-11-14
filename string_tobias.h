#ifndef __STRING_TOBIAS_H__
#define __STRING_TOBIAS_H__

String * string_new              ();
void     string_free             ( String *string );
void     string_clear            ( String *string );
void     string_append           ( String *string, const char *str );
void     string_append_character ( String *string, char c );
char     string_character_at     ( String *string, unsigned int position );
int      string_compare          ( String *string, const char *str );
void     string_split            ( String *string, String* first, String **secondary, int *num_secondary,
                                   char separator, int num_ignore, int max_secondary ); 

#endif /* __STRING_TOBIAS_H__ */
