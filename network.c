#include "include.h"

void network_setup()
{
	if( enet_initialize() != 0 )
	{
		exit(EXIT_FAILURE);
	}
}

void network_quit()
{
	enet_deinitialize();
}

static List *callbacks = NULL;

void network_event_manager_setup()
{
	callbacks = list_new(free);
}

void network_event_manager_quit()
{
	list_free(callbacks);
	callbacks = NULL;
}

void network_event_manager_register_callback( void (*callback)( ENetEvent *event ) )
{
	NetworkEventCallback *event_callback = (NetworkEventCallback *) malloc(sizeof(NetworkEventCallback));
	
	if( event_callback )
	{
		event_callback->invoke = callback;
		list_add(callbacks, event_callback);
	}
}

void network_event_manager_update()
{
	ENetHost *host = client_get_host();
	
	if( host )
	{
		ENetEvent event;
	
		while( enet_host_service(client_get_host(), &event, 10) )
		{
			FOREACH(callbacks, NetworkEventCallback, callback)
				callback->invoke(&event);
			ENDFOREACH
		
			if( event.type == ENET_EVENT_TYPE_RECEIVE )
			{
				enet_packet_destroy(event.packet);
			}
		}
	}
}

int json_get_int_value( json_t *root, const char *name )
{
	int value = 0;
	
	json_t *item = json_find_first_label(root, name);
	
	if( item )
	{
		if( item->child && item->child->type == JSON_NUMBER )
		{
			value = atoi(item->child->text);
		}
		
		json_free_value(&item);
	}
	
	return value;
}

float json_get_float_value( json_t *root, const char *name )
{
	float value = 0.0;
	
	json_t *item = json_find_first_label(root, name);
	
	if( item )
	{
		if( item->child && item->child->type == JSON_NUMBER )
		{
			value = atof(item->child->text);
		}
		
		json_free_value(&item);
	}
	
	return value;
}

void json_get_string_value( json_t *root, const char *name, char *source )
{
	json_t *item = json_find_first_label(root, name);
	
	if( item )
	{
		if( item->child && item->child->type == JSON_STRING )
		{
			sprintf(source, "%s", item->child->text);
		}
		
		json_free_value(&item);
	}
}

NetworkMessage * network_message_new( char *json )
{
	NetworkMessage *message = NULL;
	
	json_t *root = NULL;
	
	if( json_parse_document(&root, json) == JSON_OK )
	{
		message = (NetworkMessage *) malloc(sizeof(NetworkMessage));
		memset(message, 0, sizeof(NetworkMessage));
		
		message->type = json_get_int_value(root, "type");
		
		switch( message->type )
		{
			case NETWORK_MESSAGE_JOIN:
				message->join.id = json_get_int_value(root, "id");
				message->join.x = json_get_float_value(root, "x");
				message->join.y = json_get_float_value(root, "y");
				message->join.angle = json_get_float_value(root, "angle");
				message->join.speed = json_get_float_value(root, "speed");
				json_get_string_value(root, "map", message->join.map);
				break;
			case NETWORK_MESSAGE_LEAVE:
				message->leave.id = json_get_int_value(root, "id");
				break;
			case NETWORK_MESSAGE_UPDATE:
				message->update.id = json_get_int_value(root, "id");
				message->update.x = json_get_float_value(root, "x");
				message->update.y = json_get_float_value(root, "y");
				message->update.angle = json_get_float_value(root, "angle");
				message->update.speed = json_get_float_value(root, "speed");
				message->update.health = json_get_float_value(root, "health");
				break;
			case NETWORK_MESSAGE_CHAT:
				break;
			case NETWORK_MESSAGE_KILL:
				message->kill.id = json_get_int_value(root, "id");
				break;
			case NETWORK_MESSAGE_RESPAWN:
				message->respawn.id = json_get_int_value(root, "id");
				break;
			case NETWORK_MESSAGE_HIT:
				message->hit.source = json_get_int_value(root, "source");
				message->hit.target = json_get_int_value(root, "target");
				break;
			case NETWORK_MESSAGE_END_GAME:
				message->end.winner = json_get_int_value(root, "winner");
				break;
			default:
				free(message);
				json_free_value(&root);
				return NULL;
		}
		
		json_free_value(&root);
		return message;
	}
	
	return NULL;
}


void network_send_message( ENetHost *host, ENetPeer *peer, int size, char *message ) {
	ENetPacket *packet = enet_packet_create(message, size + 1, ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
	/* ENetPacket *packet = enet_packet_create(string, 512, 0); */
	enet_peer_send(peer, 1, packet);
	enet_host_flush(host);
	/* enet_packet_destroy(packet); */
}

void network_send_join_message( ENetHost *host, ENetPeer *peer, GameObject *object, Map *map )
{
	char *message = NULL;
	
	char value_type  [64];
	char value_id    [64];
	char value_x     [64];
	char value_y     [64];
	char value_angle [64];
	char value_speed [64];
	char value_map   [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type,  64, "%d", NETWORK_MESSAGE_JOIN);
	snprintf(value_id,    64, "%d", object->id);
	snprintf(value_x,     64, "%f", object->x);
	snprintf(value_y,     64, "%f", object->y);
	snprintf(value_angle, 64, "%f", object->angle);
	snprintf(value_speed, 64, "%f", object->speed);
	snprintf(value_map,   64, "%s", map->name);
	
	json_insert_pair_into_object(root, "type",  json_new_number(value_type));
	json_insert_pair_into_object(root, "id",    json_new_number(value_id));
	json_insert_pair_into_object(root, "x",     json_new_number(value_x));
	json_insert_pair_into_object(root, "y",     json_new_number(value_y));
	json_insert_pair_into_object(root, "angle", json_new_number(value_angle));
	json_insert_pair_into_object(root, "speed", json_new_number(value_speed));
	json_insert_pair_into_object(root, "map",   json_new_string(value_map));
	
	json_tree_to_string(root, &message);
	printf("json: %s", message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}

void network_send_update_message( ENetHost *host, ENetPeer *peer, GameObject *object )
{
	char *message = NULL;
	
	char value_type   [64];
	char value_id     [64];
	char value_x      [64];
	char value_y      [64];
	char value_angle  [64];
	char value_speed  [64];
	char value_health [64];
	char value_move_forward [64];
	char value_move_backward [64];
	char value_strafe_left [64];
	char value_strafe_right [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type,   64, "%d", NETWORK_MESSAGE_UPDATE);
	snprintf(value_id,     64, "%d", object->id);
	snprintf(value_x,      64, "%f", object->x);
	snprintf(value_y,      64, "%f", object->y);
	snprintf(value_angle,  64, "%f", object->angle);
	snprintf(value_speed,  64, "%f", object->speed);
	snprintf(value_health, 64, "%d", object->health);
	snprintf(value_move_forward, 64, "%d", object->move_forward);
	snprintf(value_move_backward, 64, "%d", object->move_backward);
	snprintf(value_strafe_left, 64, "%d", object->strafe_left);
	snprintf(value_strafe_right, 64, "%d", object->strafe_right);
	
	
	json_insert_pair_into_object(root, "type",   json_new_number(value_type));
	json_insert_pair_into_object(root, "id",     json_new_number(value_id));
	json_insert_pair_into_object(root, "x",      json_new_number(value_x));
	json_insert_pair_into_object(root, "y",      json_new_number(value_y));
	json_insert_pair_into_object(root, "angle",  json_new_number(value_angle));
	json_insert_pair_into_object(root, "speed",  json_new_number(value_speed));
	json_insert_pair_into_object(root, "health", json_new_number(value_health));
	json_insert_pair_into_object(root, "move_forward", json_new_number(value_move_forward));
	json_insert_pair_into_object(root, "move_backward", json_new_number(value_move_backward));
	json_insert_pair_into_object(root, "strafe_left", json_new_number(value_strafe_left));
	json_insert_pair_into_object(root, "strafe_right", json_new_number(value_strafe_right));
	
	json_tree_to_string(root, &message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}

void network_send_kill_message( ENetHost *host, ENetPeer *peer, GameObject *object )
{
	char *message = NULL;
	
	char value_type [64];
	char value_id   [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type, 64, "%d", NETWORK_MESSAGE_KILL);
	snprintf(value_id,   64, "%d", object->id);
	
	json_insert_pair_into_object(root, "type",  json_new_number(value_type));
	json_insert_pair_into_object(root, "id",    json_new_number(value_id));
	
	json_tree_to_string(root, &message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}

void network_send_respawn_message( ENetHost *host, ENetPeer *peer, GameObject *object )
{
	char *message = NULL;
	
	char value_type [64];
	char value_id   [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type, 64, "%d", NETWORK_MESSAGE_RESPAWN);
	snprintf(value_id,   64, "%d", object->id);
	
	json_insert_pair_into_object(root, "type",  json_new_number(value_type));
	json_insert_pair_into_object(root, "id",    json_new_number(value_id));
	
	json_tree_to_string(root, &message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}

void network_send_hit_message( ENetHost *host, ENetPeer *peer, GameObject *source, GameObject *target )
{
	char *message = NULL;
	
	char value_type   [64];
	char value_source [64];
	char value_target [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type,   64, "%d", NETWORK_MESSAGE_HIT);
	snprintf(value_source, 64, "%d", source->id);
	snprintf(value_target, 64, "%d", target->id);
	
	json_insert_pair_into_object(root, "type",   json_new_number(value_type));
	json_insert_pair_into_object(root, "source", json_new_number(value_source));
	json_insert_pair_into_object(root, "target", json_new_number(value_target));
	
	json_tree_to_string(root, &message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}

void network_send_end_game_message( ENetHost *host, ENetPeer *peer, GameObject *winner )
{
	char *message = NULL;
	
	char value_type   [64];
	char value_winner [64];
	
	json_t* root = json_new_object();
	
	snprintf(value_type,   64, "%d", NETWORK_MESSAGE_END_GAME);
	snprintf(value_winner, 64, "%d", winner->id);
	
	json_insert_pair_into_object(root, "type",   json_new_number(value_type));
	json_insert_pair_into_object(root, "winner", json_new_number(value_winner));
	
	json_tree_to_string(root, &message);
	network_send_message(host, peer, strlen(message), message);
	json_free_value(&root);
}