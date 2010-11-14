#include "include.h"

static void quit( SDL_Event *event )
{
	if( event->type == SDL_QUIT )
	{
		exit(0);
	}
}

static void toggle_console( SDL_Event *event ) 
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_F1 )
	{
		if( NOT console_is_frozen() )
			console_toggle_active();
	}
}

static void kill_player( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_k )
	{
		GameObject *player = client_get_player();
		
		if( player )
		{
			player_kill(player);
		}
	}
}

static void player_move_forward( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_w )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->move_forward = TRUE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
	else if( event->type == SDL_KEYUP AND event->key.keysym.sym == SDLK_w )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->move_forward = FALSE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
}

static void player_move_backward( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_s )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->move_backward = TRUE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
	else if( event->type == SDL_KEYUP AND event->key.keysym.sym == SDLK_s )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->move_backward = FALSE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
}

static void player_strafe_left( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_a )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->strafe_left = TRUE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
	else if( event->type == SDL_KEYUP AND event->key.keysym.sym == SDLK_a )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->strafe_left = FALSE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
}
static void player_strafe_right( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_d )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->strafe_right = TRUE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
	else if( event->type == SDL_KEYUP AND event->key.keysym.sym == SDLK_d )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			player->strafe_right = FALSE;
			/* network_send_update_message(client_get_host(), client_get_peer(), player); */
		}
	}
}

static void escape_quit( SDL_Event *event )
{
	if( event->type == SDL_KEYDOWN AND event->key.keysym.sym == SDLK_ESCAPE )
	{
		exit(0);
	}
}

static int pointer_x;
static int pointer_y;
static void player_rotate( SDL_Event *event )
{
	if( event->type == SDL_MOUSEMOTION )
	{
		GameObject *player = client_get_player();
		
		if( player AND player->health > 0 )
		{
			int screen_width = graphics_manager_get_screen_width();
			int screen_height = graphics_manager_get_screen_height();
			
			float radian_angle;
			
			/*
			pointer_x += event->motion.xrel;
			pointer_y += event->motion.yrel;
			*/
			pointer_x = event->motion.x;
			pointer_y = event->motion.y;
			
			radian_angle = atan2((pointer_y) - (screen_height / 2), (pointer_x) - (screen_width / 2));
			if( radian_angle < 0 )
				radian_angle += 2 * PI;
			
			player->angle = radian_angle * (180 / PI);
			
			network_send_update_message(client_get_host(), client_get_peer(), player);
		}
			
		client_set_cursor_positon(event->motion.x, event->motion.y);
	}
}

/*
static void bullet_collision( GameObject *bullet, GameObject *target )
{
	if( target AND target != bullet->origin )
	{
		printf("bulled hit: %d\n", target->id);
		network_send_hit_message(client_get_host(), client_get_peer(), client_get_player(), target);
	}
	game_object_manager_remove_object(bullet);
}


static void player_shoot( SDL_Event *event )
{
	GameObject *player = client_get_player();
	
	if( player AND player->health > 0 )
	{
		if( event->type == SDL_MOUSEBUTTONDOWN AND event->button.button == SDL_BUTTON_LEFT )
		{
			GameObject *bullet = game_object_new();
			bullet->origin = player;
			bullet->health = 1;
			bullet->width = 10;
			bullet->height = 10;
			bullet->x = player->x;
			bullet->y = player->y;
			bullet->angle = player->angle;
			bullet->speed = 5.0;
			bullet->slide_collision = FALSE;
			bullet->do_collision = bullet_collision;
			bullet->sprite = NULL;
			game_object_manager_register_object(bullet);
			
			Sprite *sprite = sprite_new();
			SpriteAnimation **animations = (SpriteAnimation**) malloc(sizeof(SpriteAnimation *) * 1);

			memset(animations, 0, sizeof(SpriteAnimation *) * 1);

			sprite->surface = graphics_manager_load_surface("gfx/gib1.png");
			sprite->width = 16;
			sprite->height = 16;
			sprite->rotatable = FALSE;

			animations[0] = sprite_animation_new(0, 0, 1000);

			sprite->animations = animations;
			sprite->current_animation = animations[0];

			bullet->sprite = sprite;
	}
}

static void player_respawn( SDL_Event *event )
{
	GameObject *player = client_get_player();
	
	if( player AND player->health == 0 )
	{
		if( event->type == SDL_MOUSEBUTTONDOWN AND event->button.button == SDL_BUTTON_LEFT )
		{
			network_send_respawn_message(client_get_host(), client_get_peer(), player);
		}
	}
}
*/

static void quit_command( String *command, String **arguments, int num_arguments )
{
	if( string_compare(command, "quit") )
	{
		exit(0);
	}
}

static void map_command( String *command, String **arguments, int num_arguments )
{
	if( string_compare(command, "map") )
	{
		if( num_arguments == 1 )
		{
			char message[64];
			snprintf(message, 64, "changing map to %s...", arguments[0]->data);
			
			client_log(message);
						
			if( client_set_map(arguments[0]->data) )
			{
				server_setup();
				client_connect("127.0.0.1", 8899);
				
				/* TODO: send map change to clients */
			}
			else
			{
				snprintf(message, 64, "error: could not find map %s.", arguments[0]->data);
				client_log(message);
			}
		}
	}
}

static void help_command( String *command, String **arguments, int num_arguments )
{
	if( string_compare(command, "help") )
	{
		console_print("type /help to show this help");
		console_print("type /map name of map to start server,");
		console_print("     choose between gfx/one or gfx/snow");
		console_print("type /connect ip number to connect to a server");
		console_print("type /quit to quit");
	}
}

static void connect_command( String *command, String **arguments, int num_arguments )
{
	if( string_compare(command, "connect") )
	{
		if( num_arguments == 1 )
		{
			char message[64];
			snprintf(message, 64, "trying to connect to %s:8899...", arguments[0]->data);
			
			client_log(message);
			
			server_quit();
			
			if( NOT client_connect(arguments[0]->data, 8899) )
			{
				client_log("error: could not connect.");
			}
		}
		else
		{
			client_log("usage /connect host");
		}
	}
}

static void connect_to_server( ENetEvent *event )
{
	if( event->type == ENET_EVENT_TYPE_CONNECT )
	{
		client_log("connected, waiting for information...");
	}
}

static void disconnect_from_server( ENetEvent *event )
{
	if( event->type == ENET_EVENT_TYPE_DISCONNECT )
	{
		client_log("disconnected from server.");
		client_disconnect();
	}
}

static void handle_network_message( ENetEvent *event )
{
	if( event->type == ENET_EVENT_TYPE_RECEIVE )
	{
		NetworkMessage *message = network_message_new((char *)event->packet->data);
		
		if( message )
		{
			switch( message->type )
			{
				case NETWORK_MESSAGE_JOIN:
				{
					GameObject *player = client_get_player();
					
					if( NOT player )
					{
						printf("Creating player: %d\n", message->join.id);
						
						player = player_new(message->join.id);
					
						game_object_set_x(player, message->join.x);
						game_object_set_y(player, message->join.y);
					
						/*
						game_object_set_width(player, 45);
						game_object_set_height(player, 45);
						*/
					
						game_object_manager_reset();
						game_object_manager_register_object(client_get_bullet());
						game_object_manager_register_object(player);
					
						client_set_player(player);
						
						{
							/* TODO: Send this in message */
							Map *map = client_get_map();
							if( NOT map )
							{
								char text[32];
								sprintf(text, "loading map %s", message->join.map);
								console_print(text);
								client_set_map(message->join.map);
								console_deactivate();
							}
						}
					}
					else
					{
						printf("Creating other player: %d\n", message->join.id);
						
						player = player_new(message->join.id);
						
						game_object_set_x(player, message->join.x);
						game_object_set_y(player, message->join.y);
						
						game_object_manager_register_object(player);
					}
					
					break;
				}
				case NETWORK_MESSAGE_LEAVE:
					break;
				case NETWORK_MESSAGE_UPDATE:
				{
					GameObject *object = game_object_manager_get_object_with_id(message->update.id);
					if( object )
					{
						object->x = message->update.x;
						object->y = message->update.y;
						object->angle = message->update.angle;
						object->speed = message->update.speed;
						object->health = message->update.health;
						object->move_forward = message->update.move_forward;
						object->move_backward = message->update.move_backward;
						object->strafe_left = message->update.strafe_left;
						object->strafe_right = message->update.strafe_right;
						
						/*
						if( entity->speed > 0.01 )
						{
							entity->sprite->current_animation = entity->sprite->animations[PLAYER_ANIMATION_WALK];
						}
						else
						{
							entity->sprite->current_animation = entity->sprite->animations[PLAYER_ANIMATION_IDLE];
						}
						*/
					}
					break;
				}
				case NETWORK_MESSAGE_CHAT:
					break;
				case NETWORK_MESSAGE_KILL:
				{
					GameObject *object = game_object_manager_get_object_with_id(message->kill.id);
					if( object )
					{
						player_kill(object);
					}
					break;
				}
				case NETWORK_MESSAGE_END_GAME:
				{
					char text[64];
					snprintf(text, 64, "player %d won the game", message->end.winner);
					console_print("----------------------");
					console_print("game over");
					console_print(text);
					console_print("----------------------");
					console_freeze();
					console_activate();
					break;
				}
			}
		}
		
		free(message);
	}
}

static void change_cursor( SDL_Event *event )
{
	if( event->type == SDL_MOUSEBUTTONDOWN AND event->button.button == SDL_BUTTON_LEFT )
	{
		client_change_cursor();
	}
	else if( event->type == SDL_MOUSEBUTTONUP AND event->button.button == SDL_BUTTON_LEFT )
	{
		client_change_cursor();
	}
}

void register_client_callbacks()
{
	input_event_manager_register_callback(quit);
	input_event_manager_register_callback(toggle_console);
	input_event_manager_register_callback(player_move_forward);
	input_event_manager_register_callback(player_move_backward);
	input_event_manager_register_callback(player_strafe_left);
	input_event_manager_register_callback(player_strafe_right);
	input_event_manager_register_callback(player_rotate);
	input_event_manager_register_callback(change_cursor);
	input_event_manager_register_callback(escape_quit);
	/* input_event_manager_register_callback(kill_player); */
	
	/*
	input_event_manager_register_callback(player_respawn);
	input_event_manager_register_callback(player_shoot);
	*/
	
	network_event_manager_register_callback(connect_to_server);
	network_event_manager_register_callback(disconnect_from_server);
	network_event_manager_register_callback(handle_network_message);
	
	console_register_command_callback(quit_command);
	console_register_command_callback(map_command);
	console_register_command_callback(connect_command);
	console_register_command_callback(help_command);
}

