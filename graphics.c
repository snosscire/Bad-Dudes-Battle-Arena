#include "include.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

static SDL_Surface *video_surface = NULL;

void graphics_manager_setup()
{
	video_surface = SDL_SetVideoMode(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, 0, SDL_SWSURFACE);
}

void graphics_manager_quit()
{
}

void graphics_manager_update()
{
	SDL_Flip(video_surface);
}

void graphics_manager_clear_screen()
{
	Uint32 black = SDL_MapRGB(video_surface->format, 0, 0, 0);
	SDL_FillRect(video_surface, NULL, black);
}

SDL_Surface * graphics_manager_get_video_surface()
{
	return video_surface;
}

int graphics_manager_get_screen_width()
{
	return video_surface->w;
}

int graphics_manager_get_screen_height()
{
	return video_surface->h;
}

void graphics_manager_put_pixel( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = NULL;
	
	SDL_LockSurface(surface);
	
	p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	
	switch( bpp ) {
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			if( SDL_BYTEORDER == SDL_BIG_ENDIAN )
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
	
	SDL_UnlockSurface(surface);
}

Uint32 graphics_manager_get_pixel( SDL_Surface *surface, int x, int y ) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;
	switch( bpp )
	{
		case 1:
			return *p;
		case 2:
			return *(Uint16 *)p;
	    case 3:
			if( SDL_BYTEORDER == SDL_BIG_ENDIAN )
			{
				return p[0] << 16 | p[1] << 8 | p[2];
			}
			else
			{
				return p[0] | p[1] << 8 | p[2] << 16;
			}
		case 4:
			return *(Uint32 *)p;
		default:
			return 0; /* shouldn't happen, but avoids warnings */
	}
}

SDL_Surface * graphics_manager_create_surface( int width, int height ) {
	/*
	int bpp = screen->format->BytesPerPixel;
	Uint32 rmask = screen->format->Rmask;
	Uint32 gmask = screen->format->Gmask;
	Uint32 bmask = screen->format->Bmask;
	Uint32 amask = screen->format->Amask;
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp, rmask, gmask, bmask, amask);
	*/
	SDL_Surface *original_surface = NULL;
	SDL_Surface *converted_surface = NULL;
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	original_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
	if( original_surface ) {
		converted_surface = SDL_ConvertSurface(original_surface, graphics_manager_get_video_surface()->format, SDL_SWSURFACE);
		SDL_FreeSurface(original_surface);
	}
	return converted_surface;
}

SDL_Surface * graphics_manager_load_surface( const char *filename )
{
	return IMG_Load(filename);
}

void graphics_manager_draw_surface_simple( SDL_Surface *surface, int x, int y )
{
	SDL_Rect destination;
	destination.x = x;
	destination.y = y;
	SDL_BlitSurface(surface, NULL, video_surface, &destination);
}

