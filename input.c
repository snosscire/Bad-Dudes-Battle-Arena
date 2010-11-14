#include "include.h"

static List *callbacks = NULL;

void input_event_manager_setup()
{
	callbacks = list_new(free);
}

void input_event_manager_quit()
{
	list_free(callbacks);
	callbacks = NULL;
}

void input_event_manager_register_callback( void (*callback)( SDL_Event *event ) )
{
	InputEventCallback *event_callback = (InputEventCallback *) malloc(sizeof(InputEventCallback));
	
	if( event_callback )
	{
		event_callback->invoke = callback;
		list_add(callbacks, event_callback);
	}
}

void input_event_manager_update()
{
	SDL_Event event;
	while( SDL_PollEvent(&event) )
	{
		FOREACH(callbacks, InputEventCallback, callback)
			callback->invoke(&event);
		ENDFOREACH
	}
}

