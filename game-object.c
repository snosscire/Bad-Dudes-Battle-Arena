#include "include.h"

static List *objects = NULL;

void game_object_manager_setup()
{
	objects = list_new(game_object_free);
}

void game_object_manager_quit()
{
	list_free(objects);
	objects = NULL;
}

void game_object_manager_update()
{
	FOREACH(objects, GameObject, object)
		game_object_update(object);
	ENDFOREACH
}

void game_object_manager_draw()
{
	FOREACH(objects, GameObject, object)
		game_object_draw(object, client_get_player());
	ENDFOREACH
}

void game_object_manager_register_object( GameObject *object )
{
	list_add(objects, object);
}

void game_object_manager_reset()
{
	list_free(objects);
	objects = NULL;
	objects = list_new(game_object_free);
}

void game_object_manager_remove_object( GameObject *object )
{
	ListNode *node = list_find(objects, object);
	list_remove(objects, node);
}

GameObject * game_object_manager_get_object_with_id ( int id )
{
	GameObject *object = NULL;
	FOREACH(objects, GameObject, o)
		if( o->id == id )
		{
			object = o;
			break;
		}
	ENDFOREACH
	return object;
}

GameObject * game_object_new()
{
	GameObject *object = (GameObject *) malloc(sizeof(GameObject));
	memset(object, 0, sizeof(GameObject));
	return object;
}

void game_object_free( void *data )
{
	GameObject *object = (GameObject *)data;
}

/*
void game_object_update( GameObject *object )
{
	float radian_angle = object->angle * (PI / 180);
	float speed_percentage = (object->speed * (time_manager_get_last_frame_time() / 10.0));
	float new_x = object->x + (speed_percentage * cos(radian_angle));
	float new_y = object->y + (speed_percentage * sin(radian_angle));
	object->x = new_x;
	object->y = new_y;
}
*/

GameObject * game_object_simple_object_collision ( GameObject *object, List *objects_list )
{
	SDL_Rect object_rect;
	object_rect.x = (int) (object->x) - (object->width / 2);
	object_rect.y = (int) (object->y) - (object->height / 2);
	object_rect.w = object->width;
	object_rect.h = object->height;
	
	FOREACH(objects_list, GameObject, target)
		if( object != target AND target->health > 0 )
		{
			SDL_Rect target_rect;
			target_rect.x = (int) (target->x) - (target->width / 2);
			target_rect.y = (int) (target->y) - (target->height / 2);
			target_rect.w = target->width;
			target_rect.h = target->height;
	
			if( check_collision(&object_rect, &target_rect) )
			{
				return target;
			}
		}
	ENDFOREACH
	
	return NULL;
}

void game_object_update_real ( GameObject *object, int do_object_collision, int do_slide_collision )
{
	if( object->health > 0 )
	{
		float radian_angle = object->angle * (PI / 180);
		float speed_percentage = (object->speed * (time_manager_get_last_frame_time() / 10.0));
		
		float new_x = object->x;
		float new_y = object->y;
		
		if( object->do_move )
		{
			object->do_move(object, radian_angle, speed_percentage, &new_x, &new_y);
		}
		else
		{
			new_x = object->x + (speed_percentage * cos(radian_angle));
			new_y = object->y + (speed_percentage * sin(radian_angle));
		}
		
		if( new_x != object->x OR new_y != object->y )
		{
			int check_map_collision = TRUE;
		
			SDL_Rect object_rect;
			object_rect.x = (int) new_x - (object->width / 2);
			object_rect.y = (int) new_y - (object->height / 2);
			object_rect.w = object->width;
			object_rect.h = object->height;
		
			if( do_object_collision )
			{
				FOREACH(objects, GameObject, target)
					if( object != target AND target->health > 0 )
					{
						SDL_Rect target_rect;
						target_rect.x = (int) target->x - (target->width / 2);
						target_rect.y = (int) target->y - (target->height / 2);
						target_rect.w = target->width;
						target_rect.h = target->height;
			
						if( check_collision(&object_rect, &target_rect) )
						{
							check_map_collision = FALSE;
							if( object->do_collision )
								check_map_collision = object->do_collision(object, target);
							break;
						}
					}
				ENDFOREACH
			}
		
			if( check_map_collision )
			{
				if( do_slide_collision )
				{
					if( NOT map_check_collision(client_get_map(), &object_rect) )
					{
						object->x = new_x;
						object->y = new_y;
					}
					else
					{
						object_rect.x = (int) new_x - (object->width / 2);
						object_rect.y = (int) object->y - (object->height / 2);
			
						if( NOT map_check_collision(client_get_map(), &object_rect) )
						{
							object->x = new_x;
						}
						else
						{
							object_rect.x = (int) object->x - (object->width / 2);
							object_rect.y = (int) new_y - (object->height / 2);
				
							if( NOT map_check_collision(client_get_map(), &object_rect) )
							{
								object->y = new_y;
							}
						}
			
					}
				}
				else
				{
					if( NOT map_check_collision(client_get_map(), &object_rect) )
					{
						object->x = new_x;
						object->y = new_y;
					}
					else
					{
						if( object->do_collision )
							object->do_collision(object, NULL);
					}
				}
				
			}
		}
		
	}
	
	/*
	{
		graphics_manager_put_pixel(client_get_map()->surface, object->x, object->y, SDL_MapRGB(graphics_manager_get_video_surface()->format, 0, 255, 255));
	}
	*/
}

void game_object_update_no_object_collision( GameObject *object )
{
	game_object_update_real(object, FALSE, FALSE);
}

void game_object_update( GameObject *object )
{
	/* printf("id: %d, health: %d, x: %f, y: %f\n", object->id, object->health, object->x, object->y); */
	game_object_update_real(object, TRUE, object->slide_collision);
}

void game_object_draw( GameObject *object, GameObject *reference )
{
	if( object->health > 0 AND object->sprite )
	{
		/* Calculate x and y of camera by substracting reference's x and y with half of screen's w and h */
		/* Add camera x and y to entity's x and y */
		/* If x and y is in camera's view draw entity */
		float camera_x = reference->x - (graphics_manager_get_screen_width() / 2);
		float camera_y = reference->y - (graphics_manager_get_screen_height() / 2);
		float object_x = object->x - camera_x;
		float object_y = object->y - camera_y;
		if( object == reference ) {
			object_x = (graphics_manager_get_screen_width() / 2);
			object_y = (graphics_manager_get_screen_height() / 2);
		}
		/*
		if( object->surface )
		{
		*/
			/* graphics_manager_draw_surface_simple(object->surface, (int)object_x, (int)object_y); */
		/*}*/
		/*
		if( object != reference )
		{
			printf("camera x: %f\n", camera_x);
			printf("camera y: %f\n", camera_y);
			printf("object x: %f\n", object_x);
			printf("object y: %f\n", object_y);
		}
		*/
		sprite_draw(object->sprite, object_x, object_y, object->angle);
		/*
		{
			Uint32 red = SDL_MapRGB(graphics_manager_get_video_surface()->format, 255, 255, 0);
			SDL_Rect rect;
			rect.x = object->x - (object->width / 2) - camera_x;
			rect.y = object->y - (object->height / 2) - camera_y;
			rect.w = object->width;
			rect.h = object->height;
			SDL_FillRect(graphics_manager_get_video_surface(), &rect, red);
		}
		*/
	}
}

void game_object_set_x( GameObject *object, float x )
{
	object->x = x;
}

void game_object_set_y( GameObject *object, float y )
{
	object->y = y;
}

void game_object_set_width( GameObject *object, int width )
{
	object->width = width;
}

void game_object_set_height( GameObject *object, int height )
{
	object->height = height;
}

/*
void game_object_set_surface( GameObject *object, SDL_Surface *surface )
{
	object->surface = surface;
}
*/

GameObject * player_new( int id )
{
	GameObject *player = game_object_new();
	Sprite *sprite = sprite_new();
	SpriteAnimation **animations = (SpriteAnimation**) malloc(sizeof(SpriteAnimation *) * 1);
	
	memset(animations, 0, sizeof(SpriteAnimation *) * 1);
	
	player->id = id;
	player->x = 0.0;
	player->y = 0.0;
	player->angle = 0.0;
	player->width = 25;
	player->height = 25;
	player->health = 100;
	player->slide_collision = TRUE;
	player->do_move = player_move;
	
	sprite->surface = graphics_manager_load_surface("gfx/player.png");
	sprite->width = PLAYER_SPRITE_WIDTH;
	sprite->height = PLAYER_SPRITE_HEIGHT;
	sprite->rotatable = TRUE;
	
	animations[PLAYER_ANIMATION_IDLE] = sprite_animation_new(0, 0, 1000);
	/* animations[PLAYER_ANIMATION_WALK] = sprite_animation_new(1, 13, 200); */
	
	sprite->animations = animations;
	sprite->current_animation = animations[PLAYER_ANIMATION_IDLE];
	
	player->sprite = sprite;
	
	return player;
}

void player_kill( GameObject *player )
{
	int gibs = random_number(1, 3);
	int i;
	
	for( i = 0; i < gibs; i++ )
	{
		add_gib(player->x, player->y);
	}
	
	add_blod(player->x, player->y);
	
	player->speed = 0.0;
	player->health = 0;
}

void player_move ( GameObject *player, float radian_angle, float speed_percentage, float *new_x, float *new_y )
{
	player->speed = 0.0;
	/* player->sprite->current_animation = player->sprite->animations[PLAYER_ANIMATION_IDLE]; */
	
	if( player->move_forward OR player->move_backward OR player->strafe_left OR player->strafe_right )
	{
		player->speed = 1.8;
		/* player->sprite->current_animation = player->sprite->animations[PLAYER_ANIMATION_WALK]; */
	}
	
	if( player->move_forward )
	{
		*new_y = player->y - speed_percentage;
	}
	else if( player->move_backward  )
	{
		*new_y = player->y + speed_percentage;
	}
	
	if( player->strafe_left )
	{
		*new_x = player->x - speed_percentage;
	}
	else if( player->strafe_right )
	{
		*new_x = player->x + speed_percentage;
	}
	
	if( client_get_player() == player )
	{
		network_send_update_message(client_get_host(), client_get_peer(), player);
	}
}

