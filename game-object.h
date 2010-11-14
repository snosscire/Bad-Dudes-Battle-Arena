#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

void game_object_manager_setup              ();
void game_object_manager_quit               ();
void game_object_manager_update             ();
void game_object_manager_draw               ();
void game_object_manager_register_object    ( GameObject *object );
void game_object_manager_reset              ();
void game_object_manager_remove_object      ( GameObject *object );

GameObject * game_object_manager_get_object_with_id ( int id );

GameObject * game_object_new         ();
void         game_object_free        ( void *data );
void         game_object_update      ( GameObject *object );
void         game_object_update_no_object_collision ( GameObject *object );
void         game_object_update_real ( GameObject *object, int do_object_collision, int slide_collision );
void         game_object_draw        ( GameObject *object, GameObject *reference );
void         game_object_set_x       ( GameObject *object, float x );
void         game_object_set_y       ( GameObject *object, float y );
void         game_object_set_width   ( GameObject *object, int width );
void         game_object_set_height  ( GameObject *object, int height );
/* void         game_object_set_surface ( GameObject *object, SDL_Surface *surface ); */


GameObject * game_object_simple_object_collision ( GameObject *object, List *objects_list );

GameObject * player_new  ( int id );
void         player_kill ( GameObject *player );
void         player_move ( GameObject *player, float radian_angle, float speed_percentage, float *new_x, float *new_y );

#endif /* __GAME_OBJECT_H__ */
