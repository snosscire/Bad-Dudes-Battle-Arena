#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

void graphics_manager_setup  ();
void graphics_manager_quit   ();
void graphics_manager_update ();

void graphics_manager_clear_screen ();

SDL_Surface * graphics_manager_get_video_surface ();

Uint32 graphics_manager_get_pixel ( SDL_Surface *surface, int x, int y );
void   graphics_manager_put_pixel ( SDL_Surface *surface, int x, int y, Uint32 pixel );

int graphics_manager_get_screen_width  ();
int graphics_manager_get_screen_height ();

SDL_Surface * graphics_manager_load_surface   ( const char *filename );
SDL_Surface * graphics_manager_create_surface ( int width, int height ); 

void graphics_manager_draw_surface_simple ( SDL_Surface *surface, int x, int y );

#endif /* __GRAPHICS_H__ */
