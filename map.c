#include "include.h"

Map * map_load( char *name ) {
	char *surface_filename = NULL;
	char *desc_surface_filename = NULL;
	SDL_Surface *desc_surface = NULL;
	Map *map = NULL;
	
	surface_filename = (char *) malloc(sizeof(char) * (strlen(name) + strlen(".bmp") + 1));
	desc_surface_filename = (char *) malloc(sizeof(char) * (strlen(name) + strlen("-description.bmp") + 1));
	
	map = (Map *) malloc(sizeof(Map));
	
	if( NOT map )
	{
		free(surface_filename);
		free(desc_surface_filename);
		return NULL;
	}
	
	memset(map, 0, sizeof(Map));
	
	/* sprintf(map->name, "%s", name); */
	map->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
	sprintf(map->name, "%s", name);
	printf("name: %s\n", map->name);
	
	sprintf(surface_filename, "%s.bmp", name);
	sprintf(desc_surface_filename, "%s-description.bmp", name);
	/* printf("%s-description.bmp\n", name); */
	
	map->surface = graphics_manager_load_surface(surface_filename);
	
	if( NOT map->surface )
	{
		free(surface_filename);
		free(desc_surface_filename);
		free(map);
		return NULL;
	}
	
	map->boxes = list_new(free);
	
	desc_surface = graphics_manager_load_surface(desc_surface_filename);
	
	if( desc_surface )
	{
		Uint32 black = SDL_MapRGB(desc_surface->format, 0, 0, 0);
		Uint32 white = SDL_MapRGB(desc_surface->format, 255, 255, 255);
		
		Uint32 pixel;
		
		SDL_Rect *rect = NULL;
		
		int x;
		int y;
		
		/* If new white set start to position and end to position */
		/* While white increase end position */
		/* When black check if box above x starts at start x position */
		/* If false create new box */
		/* If true check if fetched box starts and ends on start and end position */
		/* If true increase height of box */
		
		for( y = 0; y < desc_surface->h; y++ )
		{
			for( x = 0; x < desc_surface->w; x++ )
			{
				pixel = graphics_manager_get_pixel(desc_surface, x, y);
				
				if( pixel == white )
				{
					if( NOT rect )
					{
						rect = (SDL_Rect *) malloc(sizeof(SDL_Rect));
						rect->x = x;
						rect->y = y;
						rect->w = 1;
						rect->h = 1;
					}
					else
					{
						rect->w++;
					}
				}
				
				if( (pixel == black OR x == desc_surface->w - 1) AND rect )
				{
					int create_new_box = FALSE;
					
					if( y > 0 )
					{
						SDL_Rect *above_rect = map_get_box_at(map, rect->x, (y - 1));
						
						if( above_rect && rect->w == above_rect->w )
						{
							above_rect->h++;
							free(rect);
							rect = NULL;
						}
						else
						{
							create_new_box = TRUE;
						}
					}
					else
					{
						create_new_box = TRUE;
					}
					
					if( create_new_box )
					{
						list_add(map->boxes, rect);
						rect = NULL;
					}
				}
			}
		}
		
		SDL_FreeSurface(desc_surface);
	}
	
	free(surface_filename);
	free(desc_surface_filename);
	
	return map;
}

void map_unload( Map* map )
{
	if( map )
	{
		SDL_FreeSurface(map->surface);
		
		if( map->boxes )
		{
			list_free(map->boxes);
		}
		
		free(map);
	}
}

void map_draw( Map *map, GameObject *reference ) {
	SDL_Rect src_rect;
	src_rect.x = 0;
	src_rect.y = 0;
	
	if( reference )
	{
		src_rect.x = (reference->x) - (graphics_manager_get_screen_width() / 2);
		src_rect.y = (reference->y) - (graphics_manager_get_screen_height() / 2);
	}

	src_rect.w = graphics_manager_get_screen_width();
	src_rect.h = graphics_manager_get_screen_height();
		
	SDL_BlitSurface(map->surface, &src_rect, graphics_manager_get_video_surface(), NULL);
}

SDL_Rect * map_get_box_at( Map *map, int x, int y ) {
	FOREACH(map->boxes, SDL_Rect, box)
		if( box->x == x AND box->y == y ) {
			return box;
		}
	ENDFOREACH
	return NULL;
}

SDL_Rect * map_check_collision( Map *map, SDL_Rect *a )
{
	FOREACH( map->boxes, SDL_Rect, b )
		if( check_collision(a, b) ) {
			return b;
		}
	ENDFOREACH
	return NULL;
}


void map_add_gib( Map* map, GameObject *gib )
{
	SDL_Rect dst_rect;
	dst_rect.x = (int)gib->x - (gib->sprite->surface->w / 2);
	dst_rect.y = (int)gib->y - (gib->sprite->surface->h / 2);
	/*
	printf("gib x: %f\n", gib->x);
	printf("gib y: %f\n", gib->y);
	printf("dst rect x: %d\n", dst_rect.x);
	printf("dst rect y: %d\n", dst_rect.y);
	*/
	SDL_BlitSurface(gib->sprite->surface, NULL, map->surface, &dst_rect);
}

void map_add_blood( Map *map, int x, int y )
{
	Uint32 red = SDL_MapRGB(map->surface->format, 200, 0, 0);
	int splashes = random_number(4, 8);
	int pixels;
	int pixel_origin_x;
	int pixel_origin_y;
	int i;
	int j;
	for( i = 0; i < splashes; i++ ) {
		pixels = random_number(50, 65);
		pixel_origin_x = random_number(x - 25, x + 25);
		pixel_origin_y = random_number(y - 25, y + 25);
		for( j = 0; j < pixels; j++ ) {
			graphics_manager_put_pixel(map->surface,
				random_number(pixel_origin_x - 5, pixel_origin_x + 5),
				random_number(pixel_origin_y - 5, pixel_origin_y + 5),
				red);
		}
	}
}

