#ifndef __INPUT_H__
#define __INPUT_H__

void input_event_manager_setup             ();
void input_event_manager_quit              ();
void input_event_manager_register_callback ( void (*callback)( SDL_Event *event ) );
void input_event_manager_update            ();

#endif /* __INPUT_H__ */
