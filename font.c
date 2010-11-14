#include "include.h"

Font * font_load( const char *filename, int width, int height )
{
	Font *font = (Font*) malloc(sizeof(Font));
	if( NOT font ) {
		return NULL;
	}
	memset(font, 0, sizeof(Font));
	font->surface = graphics_manager_load_surface(filename);
	if( NOT font->surface ) {
		free(font);
		return NULL;
	}
	/* SDL_SetColorKey(font->surface, SDL_SRCCOLORKEY, SDL_MapRGB(font->surface->format, 0, 0, 0)); */
	font->width = width;
	font->height = height;
	return font;
}

void font_unload( Font *font )
{
	if( font ) {
		SDL_FreeSurface(font->surface);
		free(font);
	}
}

int font_get_text_width( Font *font, const char *text )
{
	return strlen(text) * font->width;
}

SDL_Surface * font_draw_text( Font *font, const char *text )
{
	SDL_Surface* text_surface = graphics_manager_create_surface(font_get_text_width(font, text), font->height);
	
	unsigned int length = strlen(text);
	unsigned int i;
	
	/* SDL_SetColorKey(text_surface, SDL_SRCCOLORKEY, SDL_MapRGB(text_surface->format, 0, 0, 0)); */
	
	for( i = 0; i < length; i++ )
	{
		SDL_Rect src_rect;
		SDL_Rect dest_rect;
		
		int c = (int)text[i];
		
		src_rect.x = (c % 16) * font->width;
		src_rect.y = (c / 16) * font->height;
		src_rect.w = font->width;
		src_rect.h = font->height;
		
		dest_rect.x = i * font->width;;
		dest_rect.y = 0;
		dest_rect.w = font->width;
		dest_rect.h = font->height;
		
		SDL_BlitSurface(font->surface, &src_rect, text_surface, &dest_rect);
	}

	return text_surface;
}

