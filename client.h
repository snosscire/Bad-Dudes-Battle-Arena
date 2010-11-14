#ifndef __CLIENT_H__
#define __CLIENT_H__

void client_quit  ();
void client_setup ();
void client_run   ();

void client_log ( char *message );

Map *        client_get_map    ();
GameObject * client_get_player ();
GameObject * client_get_bullet ();

void client_set_player( GameObject *object );

int client_set_map ( char *filename );

int  client_connect      ( char *hostname, int port );
void client_disconnect   ();
int  client_is_connected ();

ENetHost * client_get_host ();
ENetPeer * client_get_peer ();

int   check_collision ( SDL_Rect *a, SDL_Rect *b );
int   random_number   ( int min, int max );
float add_to_angle    ( float angle, float value );
void  add_gib         ( float origin_x, float origin_y );
void  add_blod        ( float origin_x, float origin_y );

void client_set_cursor_position ( int x, int y );
void client_draw_cursor         ();
void client_change_cursor       ();

#endif /* __CLIENT_H__ */
