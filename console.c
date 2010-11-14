#include "include.h"
#include <assert.h>

#define LINE_HEIGHT 12
#define MAX_INPUT_LENGTH 64
#define SCROLL_LINES 100
#define MAX_COMMAND_ARGUMENTS 8

static int          active;
static int          width;
static int          height;
static SDL_Surface *root_surface;
static Font        *font;
static List        *lines;
static int          input_position;
static char         input[MAX_INPUT_LENGTH];
static SDL_Surface *input_surface;
static unsigned int input_length;
static int frozen = FALSE;

static List *command_callbacks;
static void (*no_command_callback) ( const char *line );

static void destroy_line  ( void *line );
static void read_keyboard ( SDL_Event *event );

void console_freeze()
{
	frozen = TRUE;
}

int console_is_frozen()
{
	return frozen;
}

int console_setup( int w, int h )
{
	active = FALSE;
	width = w;
	height = h;
	
	font = font_load("gfx/bachler-font.bmp", 10, 10);
	if( NOT font ) {
		return FALSE;
	}
	
	root_surface = graphics_manager_create_surface(w, h - LINE_HEIGHT);
	if( NOT root_surface ) {
		font_unload(font);
		return FALSE;
	}
	
	lines = list_new(destroy_line);
	if( NOT lines ) {
		font_unload(font);
		SDL_FreeSurface(root_surface);
		return FALSE;
	}
	
	command_callbacks = list_new(free);
	
	input_surface = font_draw_text(font, "_");
	assert(input_surface);
	input_position = -1;
	
	input_event_manager_register_callback(read_keyboard);
	
	return TRUE;
}

void console_quit()
{
	font_unload(font);
	list_free(command_callbacks);
	list_free(lines);
	SDL_FreeSurface(root_surface);
	SDL_FreeSurface(input_surface);
}

int console_resize( int w, int h )
{
	return FALSE;
}

void console_activate()
{
	active = TRUE;
	SDL_EnableUNICODE(1);
}

void console_deactivate()
{
	active = FALSE;
	SDL_EnableUNICODE(0);
}

void console_toggle_active()
{
	if( active ) {
		console_deactivate();
	} else {
		console_activate();
	}
}

void console_set_no_command_callback( void (*callback)( const char *line ) )
{
	no_command_callback = callback;
}

void console_register_command_callback( void (*callback)( String *command, String **arguments, int num_arguments ) )
{
	CommandCallback *command_cb = (CommandCallback *) malloc(sizeof(CommandCallback));
	if( command_cb ) {
		command_cb->invoke = callback;
		list_add(command_callbacks, command_cb);
	}
}

void console_print( const char *text )
{
	SDL_Surface *text_surface = font_draw_text(font, text);
	if( text_surface ) {
		SDL_Rect line_dst_rect;
		int y = height - (LINE_HEIGHT * 2);
		if( lines->size == SCROLL_LINES ) {
			list_remove(lines, lines->last);
		}
		list_add_first(lines, text_surface);
		SDL_FillRect(root_surface, NULL, SDL_MapRGB(root_surface->format, 0, 0, 0));
		FOREACH(lines, SDL_Surface, line)
			if( y == 0 )
				break;
			line_dst_rect.x = 0;
			line_dst_rect.y = y;
			SDL_BlitSurface(line, NULL, root_surface, &line_dst_rect);
			y -= LINE_HEIGHT;
		ENDFOREACH
	}
}

void console_draw()
{
	if( active )
	{
		SDL_Rect input_dst_rect;
		input_dst_rect.x = 0;
		input_dst_rect.y = height - LINE_HEIGHT;
		SDL_BlitSurface(root_surface, NULL, graphics_manager_get_video_surface(), NULL);
		SDL_BlitSurface(input_surface, NULL, graphics_manager_get_video_surface(), &input_dst_rect);
	}
}

void destroy_line( void *line )
{
	SDL_FreeSurface((SDL_Surface *)line);
}

void read_keyboard( SDL_Event *event )
{
	if( active AND event->type == SDL_KEYDOWN ) {
		/* Limiting what characters can be entered because a complete */
		/* font has not yet been created */
		switch( event->key.keysym.sym ) {
			case SDLK_a:
			case SDLK_b:
			case SDLK_c:
			case SDLK_d:
			case SDLK_e:
			case SDLK_f:
			case SDLK_g:
			case SDLK_h:
			case SDLK_i:
			case SDLK_j:
			case SDLK_k:
			case SDLK_l:
			case SDLK_m:
			case SDLK_n:
			case SDLK_o:
			case SDLK_p:
			case SDLK_q:
			case SDLK_r:
			case SDLK_s:
			case SDLK_t:
			case SDLK_u:
			case SDLK_v:
			case SDLK_w:
			case SDLK_x:
			case SDLK_y:
			case SDLK_z:
			case SDLK_0:
			case SDLK_1:
			case SDLK_2:
			case SDLK_3:
			case SDLK_4:
			case SDLK_5:
			case SDLK_6:
			case SDLK_7:
			case SDLK_8:
			case SDLK_9:
			case SDLK_COMMA:
			case SDLK_MINUS:
			case SDLK_PERIOD:
			case SDLK_SLASH:
			case SDLK_SPACE:
				if( input_position + 2 < MAX_INPUT_LENGTH ) {
					input_position++;
					input[input_position] = event->key.keysym.unicode & 0x7F;
					input[input_position + 1] = '_';
					if( input_surface ) {
						SDL_FreeSurface(input_surface);
						input_surface = NULL;
					}
					input_surface = font_draw_text(font, input);
					assert(input_surface);
					input_length++;
				}
				break;
			case SDLK_BACKSPACE:
				if( input_position >= 0 ) {
					input[input_position] = '_';
					input[input_position + 1] = 0;
					input_position--; 
					if( input_surface ) {
						SDL_FreeSurface(input_surface);
						input_surface = NULL;
					}
					input_surface = font_draw_text(font, input);
					assert(input_surface);
					input_length--;
				}
				break;
			case SDLK_RETURN:
				if( input_length > 0 )
				{
					int i;
					
					int line_length = 0;
					char *line = (char *) malloc(sizeof(char) * (input_length + 1));
					memset(line, 0 , sizeof(char) * (input_length + 1));
					strncpy(line, input, input_length);
					line[input_length] = '\0';
					line_length = strlen(line);
					
					if( line_length > 1 AND line[0] == '/' )
					{
						String *command = NULL;
						String **arguments = (String**) malloc(sizeof(String *) * MAX_COMMAND_ARGUMENTS);
						unsigned int num_arguments = 0;
						
						String *current = string_new();
						
						printf("Line: %s\n", line);
						
						for( i = 1; i < line_length; i++ ) {
							if( line[i] != ' ' ) {
								string_append_character(current, line[i]);
							}
							if( (line[i] == ' ' OR i == (line_length - 1)) AND current->length ) {
								if( NOT command ) {
									command = string_new();
									string_append(command, current->data);
									string_clear(current);
								} else {
									String *argument = string_new();
									string_append(argument, current->data);
									arguments[num_arguments] = argument;
									num_arguments++;
									string_clear(current);
									if( num_arguments == MAX_COMMAND_ARGUMENTS )
										break;
								}
							}
						}
						
						string_free(current);
						
						if( command )
						{
							printf("Command: %s\n", command->data);
						}
						for( i = 0; i < num_arguments; i++ )
						{
							printf("Argument: %s\n", arguments[i]->data);
						}
						
						if( command )
						{
							FOREACH(command_callbacks, CommandCallback, command_cb)
								command_cb->invoke(command, arguments, num_arguments);
							ENDFOREACH
							string_free(command);
							for( i = 0; i < num_arguments; i++ )
								string_free(arguments[i]);
						}
					}
					else if( no_command_callback )
					{
						no_command_callback(line);
					}
					
					free(line);
					
					for( i = 0; i < MAX_INPUT_LENGTH; i++ )
						input[i] = 0;
					input[0] = '_';
					input_position = -1;
					input_length = 0;
					
					SDL_FreeSurface(input_surface);
					input_surface = NULL;
					input_surface = font_draw_text(font, input);
					assert(input_surface);
				}
				
				break;
			default:
				break;
		}
	}
}

