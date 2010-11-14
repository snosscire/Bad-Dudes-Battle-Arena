#ifndef __CONSOLE_H__
#define __CONSOLE_H__

int  console_setup                     ( int width, int height );
void console_quit                      ();
int  console_resize                    ( int width, int height );
void console_activate                  ();
void console_deactivate                ();
void console_toggle_active             ();
void console_set_no_command_callback   ( void (*callback)( const char *line ) );
void console_register_command_callback ( void (*callback)( String *command, String **arguments, int num_arguments ) );
void console_print                     ( const char *text );
void console_draw                      ();
void console_freeze                    ();
int  console_is_frozen                 ();

#endif /* __CONSOLE_H__ */
