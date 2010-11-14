#include "include.h"

static ENetHost *host = NULL;
static List *objects = NULL;
static List *clients = NULL;
static int next_game_object_id = 0;

static void server_handle_connect( ENetEvent *event )
{
	GameObject *telefragged_player = NULL;
	
	ServerClient *new_client = NULL;
	
	next_game_object_id++;
	
	new_client = server_client_new();
	new_client->object = game_object_new();
	
	new_client->peer = event->peer;
	
	new_client->object->id = next_game_object_id;
	new_client->object->x = 100.0;
	new_client->object->y = 100.0;
	new_client->object->angle = 0.0;
	new_client->object->speed = 0.0;
	new_client->object->width = 25;
	new_client->object->height = 25;
	new_client->object->health = 100;

	event->peer->data = new_client;
	
	network_send_join_message(host, new_client->peer, new_client->object, client_get_map());
	
	FOREACH(clients, ServerClient, client)
		network_send_join_message(host, new_client->peer, client->object, client_get_map());
		if( client->object->id != new_client->object->id )
			network_send_join_message(host, client->peer, new_client->object, client_get_map());
	ENDFOREACH
	
	telefragged_player = game_object_simple_object_collision(new_client->object, objects);
	
	list_add(clients, new_client);
	list_add(objects, new_client->object);
	
	if( telefragged_player )
	{
		telefragged_player->health = 0;
		FOREACH(clients, ServerClient, client)
			if( client->object == telefragged_player )
				client->score--;
			network_send_kill_message(host, client->peer, telefragged_player);
		ENDFOREACH
	}
}

static void server_handle_disconnect( ENetEvent *event )
{
	printf("Server: disconnect.\n");
}

static void server_handle_receive( ENetEvent *event )
{
	ServerClient *sender_client = (ServerClient *) event->peer->data;
	
	NetworkMessage *message = network_message_new((char *)event->packet->data);
	
	/* printf("json: %s\n", (char *)event->packet->data); */
	
	if( message )
	{
		switch( message->type )
		{
			case NETWORK_MESSAGE_UPDATE:
			{
				sender_client->object->x = message->update.x;
				sender_client->object->y = message->update.y;
				sender_client->object->angle = message->update.angle;
				sender_client->object->speed = message->update.speed;
				
				FOREACH(clients, ServerClient, client)
					if( client->object->id != sender_client->object->id ) {
						network_send_update_message(host, client->peer, sender_client->object);
					}
				ENDFOREACH
				
				break;
			}
			case NETWORK_MESSAGE_RESPAWN:
			{
				GameObject *telefragged_player = NULL;
				
				sender_client->object->x = 100.0;
				sender_client->object->y = 100.0;
				sender_client->object->speed = 0.0;
				sender_client->object->health = 100;

				telefragged_player = game_object_simple_object_collision(sender_client->object, objects);
				
				FOREACH(clients, ServerClient, client)
					network_send_update_message(host, client->peer, sender_client->object);
				ENDFOREACH
				
				if( telefragged_player )
				{
					telefragged_player->health = 0;
					FOREACH(clients, ServerClient, client)
						network_send_kill_message(host, client->peer, telefragged_player);
					ENDFOREACH
				}
				
				break;
			}
			case NETWORK_MESSAGE_HIT:
			{
				FOREACH(clients, ServerClient, target)
					if( target->object->id == message->hit.target )
					{
						target->object->health -= 35;
						
						if( target->object->health <= 0 )
						{
							char text[64];
							
							FOREACH(clients, ServerClient, client)
								network_send_kill_message(host, client->peer, target->object);
							ENDFOREACH
							
							sender_client->score++;
							
							snprintf(text, 64, "player %d score is %d", sender_client->object->id, sender_client->score);
							console_print(text);
							
							if( sender_client->score >= 20 )
							{
								FOREACH(clients, ServerClient, client)
									network_send_end_game_message(host, client->peer, sender_client->object);
								ENDFOREACH
							}
						}
						
						break;
					}
				ENDFOREACH
				
			}
		}
	}
	
	enet_packet_destroy(event->packet);
}

void server_quit()
{
	if( clients )
	{
		list_free(clients);
	}
	
	if( objects )
	{
		list_free(objects);
	}
	
	if( host )
	{
		enet_host_destroy(host);
		host = NULL;
	}
}

void server_setup()
{
	if( NOT clients )
	{
		clients = list_new(server_client_free);
	}
	
	if( NOT objects )
	{
		objects = list_new(NULL);
	}
	
	if( NOT host )
	{
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = 8899;
	
		host = enet_host_create(&address, 32, 0, 0);
	}
}

void server_update()
{
	if( host )
	{
		ENetEvent event;
	
		while( enet_host_service(host, &event, 1) > 0 )
		{
			switch( event.type ) {
				case ENET_EVENT_TYPE_NONE:
					break;
				case ENET_EVENT_TYPE_CONNECT:
					server_handle_connect(&event);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					server_handle_disconnect(&event);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					server_handle_receive(&event);
					break;
			}
		}
	}
}

ServerClient * server_client_new()
{
	ServerClient *client = (ServerClient *) malloc(sizeof(ServerClient));
	memset(client, 0, sizeof(ServerClient));
	client->score = 0;
	return client;
}

void server_client_free( void *data )
{
	ServerClient *client = (ServerClient *) data;
	game_object_free(client->object);
}

