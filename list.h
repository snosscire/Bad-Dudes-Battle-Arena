#ifndef __LIST_H__
#define __LIST_H__

List *     list_new       ( void (*destroy)( void *data ) );
void       list_free      ( List *list);
void       list_add       ( List *list, void *data );
void       list_add_first ( List *list, void *data );
void       list_remove    ( List *list, ListNode *node );
ListNode * list_find      ( List *list, void *data );

#define FOREACH( LIST, TYPE, NAME ) \
	if( LIST->first ) { \
		ListNode *current_node = NULL; \
		ListNode *next_node = LIST->first; \
		TYPE *NAME = NULL; \
		while( next_node ) { \
			current_node = next_node; \
			next_node = current_node->next; \
			NAME = (TYPE *)current_node->data;

#define ENDFOREACH \
		} \
	}

#define CONTINUE \
	node = node->next; \
	continue;

#endif /* __LIST_H__ */
