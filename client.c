#include "include.h"

static int running = TRUE;
static Map *map = NULL;
static GameObject *player = NULL;
static ENetHost *host = NULL;
static ENetPeer *peer = NULL;
static List *gibs = NULL;
static GameObject *bullet = NULL;

static SDL_Surface *cursor_surface;
static SDL_Surface *cursor_surface_regular;
static SDL_Surface *cursor_surface_shooting;
static int cursor_x;
static int cursor_y;

static int bullet_collision( GameObject *bullet, GameObject *target )
{
	/* console_print("hit"); */
	if( NOT target )
	{
		/* console_print("hit wall"); */
		bullet->health = 0;
		return FALSE;
	}
	if( target AND target == bullet->origin )
	{
		/* console_print("hit self"); */
		return TRUE;
	}
	if( target AND target != bullet->origin )
	{
		/* console_print("bullet hit enemy"); */
		network_send_hit_message(client_get_host(), client_get_peer(), client_get_player(), target);
		map_add_blood(map, (int)target->x, (int)target->y);
		bullet->health = 0;
		return FALSE;
	}
	/* game_object_manager_remove_object(bullet); */
	bullet->health = 0;
	return FALSE;
}

static void player_shoot()
{
	GameObject *player = client_get_player();
	
	if( player AND player->health > 0 AND bullet->health == 0 )
	{
		bullet->origin = player;
		bullet->health = 1;
		bullet->x = player->x;/* + player->width + 5;*/
		bullet->y = player->y;/* + player->height + 5;*/
		bullet->angle = player->angle;
		bullet->speed = 5.0;
		
		/* console_print("shot"); */
	}
}

static void player_respawn()
{
	GameObject *player = client_get_player();
	
	if( player AND player->health == 0 )
	{
		network_send_respawn_message(client_get_host(), client_get_peer(), player);
	}
}

void client_quit()
{
	running = FALSE;
	player = NULL;
	
	console_quit();
	
	game_object_manager_quit();
	network_event_manager_quit();
	input_event_manager_quit();
	graphics_manager_quit();
	
	SDL_Quit();
	
	if( gibs )
	{
		list_free(gibs);
	}
}

void client_setup()
{
	srand(time(NULL));
	
	SDL_Init(SDL_INIT_VIDEO);
	
	/* atexit(client_quit); */
	
	graphics_manager_setup();
	input_event_manager_setup();
	network_event_manager_setup();
	game_object_manager_setup();
	
	console_setup(graphics_manager_get_screen_width(), graphics_manager_get_screen_height());
	console_print("--------------------------------------------------------");
	console_print("welcome to the bad dudes arena game!");
	console_print("play deathmatch against your friends in a top down view");
	console_print("type /help followed by enter to get help");
	console_print("press f1 to toggle console");
	console_print("--------------------------------------------------------");
	console_activate();
	
	register_client_callbacks();
	
	gibs = list_new(game_object_free);
	
	/*
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	*/
	
	bullet = game_object_new();
	bullet->health = 0;
	bullet->width = 2;
	bullet->height = 2;
	bullet->slide_collision = FALSE;
	bullet->do_collision = bullet_collision;
	
	bullet->sprite = NULL;
	
	/*
	bullet->sprite = sprite_new();
	bullet->sprite->surface = graphics_manager_load_surface("gfx/bullets.png");
	bullet->sprite->width = 10;
	bullet->sprite->height = 10;
	bullet->sprite->rotatable = FALSE;

	bullet->sprite->animations = (SpriteAnimation**) malloc(sizeof(SpriteAnimation *) * 1);
	bullet->sprite->animations[0] = sprite_animation_new(0, 0, 1000);
	bullet->sprite->current_animation = bullet->sprite->animations[0];
	*/
	
	SDL_ShowCursor(SDL_DISABLE);
	cursor_surface_regular = graphics_manager_load_surface("gfx/cursor.png");
	cursor_surface_shooting = graphics_manager_load_surface("gfx/cursor_shooting.png");
	cursor_surface = cursor_surface_regular;
}

void client_change_cursor()
{
	if( cursor_surface == cursor_surface_regular )
	{
		cursor_surface = cursor_surface_shooting;
	}
	else
	{
		cursor_surface = cursor_surface_regular;
	}
}

void client_set_cursor_positon( int x, int y )
{
	cursor_x = x;
	cursor_y = y;
}

void client_draw_cursor()
{
	graphics_manager_draw_surface_simple(cursor_surface, (cursor_x - 7), (cursor_y - 7));
}

void client_run()
{
	while( running )
	{
		time_manager_update();
		
		server_update();
		
		network_event_manager_update();
		input_event_manager_update();
		
		if( SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1) )
		{
			player_respawn();
			player_shoot();
		}

		graphics_manager_clear_screen();
		
		if( map )
		{
			FOREACH(gibs, GameObject, gib)
				game_object_update_no_object_collision(gib);
				
				gib->speed -= gib->speed * (time_manager_get_last_frame_time() / 500.0);
				
				if( gib->speed <= 0.01 )
				{
					map_add_gib(map, gib);
					list_remove(gibs, current_node);
				}
			ENDFOREACH
			
			game_object_manager_update();
			
			map_draw(map, player);
			
			FOREACH(gibs, GameObject, gib)
				game_object_draw(gib, player);
			ENDFOREACH
			
			game_object_manager_draw();
		}
		
		console_draw();
		
		client_draw_cursor();
		
		graphics_manager_update();
	}
}

void client_log( char *message )
{
	printf("%s\n", message);
}

int client_is_connected()
{
	if( host )
		return TRUE;
	return FALSE;
}

Map * client_get_map()
{
	return map;
}

GameObject * client_get_player()
{
	return player;
}

GameObject * client_get_bullet()
{
	return bullet;
}

void client_set_player( GameObject *object )
{
	player = object;
}

void client_disconnect()
{
	if( host )
	{
		if( host )
		{
			enet_host_destroy(host);
		}
	
		host = NULL;
		/* I wonder what happens with this. Memory leak FTW!!! */
		peer = NULL;
		
		if( map )
		{
			map_unload(map);
			map = NULL;
		}
		
		game_object_manager_reset();
		player = NULL;
	}
}

int client_connect( char *hostname, int port )
{
	ENetAddress address;
	
	enet_address_set_host(&address, hostname);
	address.port = port;
	
	client_disconnect();
	
	host = enet_host_create(NULL, 16, 0, 0);
	
	if( host )
	{
		peer = enet_host_connect(host, &address, 2);
		
		if( peer )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

int client_set_map( char *filename )
{
	Map *new_map = map_load(filename);
	
	if( new_map )
	{
		if( map )
		{
			map_unload(map);
			map = NULL;
		}
	
		map = new_map;
	
		return TRUE;
	}
	
	return FALSE;
}

ENetHost * client_get_host()
{
	return host;
}

ENetPeer * client_get_peer()
{
	return peer;
}

int check_collision( SDL_Rect *a, SDL_Rect *b ) {
	int a_left = a->x;
	int a_right = a->x + a->w;
	int a_top = a->y;
	int a_bottom = a->y + a->h;
	int b_left = b->x;
	int b_right = b->x + b->w;
	int b_top = b->y;
	int b_bottom = b->y + b->h;
	if( a_bottom <= b_top ) {
		return FALSE;
	}
	if( a_top >= b_bottom ) {
		return FALSE;
	}
	if( a_right <= b_left ) {
		return FALSE;
	}
	if( a_left >= b_right ) {
		return FALSE;
	}
	return TRUE;
}

int random_number( int min, int max ) {
	return (rand() % (max - min + 1) + min);
}

float add_to_angle( float angle, float value )
{
	angle += value;
	
	if( angle > 360.0 )
	{
		angle -= 360.0;
	}
	else if( angle < 0.0 )
	{
		angle += 360.0;
	}
	
	return angle;
}

void add_gib( float origin_x, float origin_y ) {
	/* First the gib is added to the gibs list as an Entity. */
	/* In the main loop the gib is updated until the speed is 0. */
	/* When the speed is 0 it is drawn to the map's surface. */
	GameObject *gib = game_object_new();
	
	printf("origin_x: %f\n", origin_x);
	printf("origin_y: %f\n", origin_y);
	
	if( gib )
	{
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
		
		gib->sprite = sprite;
		
		gib->x = random_number((int)origin_x - 2, (int)origin_x + 2);
		gib->y = random_number((int)origin_y - 2, (int)origin_y + 2);
		gib->speed = random_number(8, 14) / 10.0;
		gib->angle = random_number(0, 359);
		gib->width = 16;
		gib->height = 16;
		gib->health = 1;
		gib->slide_collision = TRUE;
		
		list_add(gibs, gib);
	}
}

void add_blod( float origin_x, float origin_y ) {
	map_add_blood(map, (int)origin_x, (int)origin_y);
}

