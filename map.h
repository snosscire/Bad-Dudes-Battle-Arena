#ifndef __MAP_H__
#define __MAP_H__

Map * map_load   ( char *name );
void  map_unload ( Map* map );

void  map_draw ( Map *map, GameObject *reference );

SDL_Rect * map_get_box_at      ( Map *map, int x, int y );
SDL_Rect * map_check_collision ( Map *map, SDL_Rect *a );

void map_add_gib   ( Map* map, GameObject *gib );
void map_add_blood ( Map *map, int x, int y );

#endif /* __MAP_H__ */
