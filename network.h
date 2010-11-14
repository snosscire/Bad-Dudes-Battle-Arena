#ifndef __NETWORK_H__
#define __NETWORK_H__

void network_setup ();
void network_quit  ();

void network_event_manager_setup  ();
void network_event_manager_quit   ();
void network_event_manager_update ();

void network_event_manager_register_callback( void (*callback)( ENetEvent *event ) );

#define NETWORK_MESSAGE_JOIN     1
#define NETWORK_MESSAGE_LEAVE    2
#define NETWORK_MESSAGE_UPDATE   3
#define NETWORK_MESSAGE_CHAT     4
#define NETWORK_MESSAGE_KILL     5
#define NETWORK_MESSAGE_RESPAWN  6
#define NETWORK_MESSAGE_HIT      7
#define NETWORK_MESSAGE_END_GAME 8

NetworkMessage * network_message_new ( char *json );

void network_send_message          ( ENetHost *host, ENetPeer *peer, int size, char *message );
void network_send_join_message     ( ENetHost *host, ENetPeer *peer, GameObject *object, Map *map );
void network_send_update_message   ( ENetHost *host, ENetPeer *peer, GameObject *object );
void network_send_kill_message     ( ENetHost *host, ENetPeer *peer, GameObject *object );
void network_send_respawn_message  ( ENetHost *host, ENetPeer *peer, GameObject *object );
void network_send_hit_message      ( ENetHost *host, ENetPeer *peer, GameObject *source, GameObject *target );
void network_send_end_game_message ( ENetHost *host, ENetPeer *peer, GameObject *winner );

#endif /* __NETWORK_H__ */
