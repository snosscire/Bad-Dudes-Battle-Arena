#include "include.h"

Sprite * sprite_new()
{
	Sprite *sprite = (Sprite *) malloc(sizeof(Sprite));
	memset(sprite, 0, sizeof(Sprite));
	return sprite;
}

void sprite_free( Sprite *sprite )
{
	/* TODO: Free animations */
	free(sprite);
}

void sprite_update( Sprite *sprite )
{
	sprite_animation_update(sprite->current_animation);
}

void sprite_draw( Sprite *sprite, float x, float y, float angle )
{
	sprite_animation_draw(sprite->current_animation, sprite, x, y, angle);
}

SpriteAnimation * sprite_animation_new( int first, int last, int time )
{
	SpriteAnimation *animation = (SpriteAnimation *) malloc(sizeof(SpriteAnimation));
	
	if( animation )
	{
		memset(animation, 0, sizeof(SpriteAnimation));
		
		animation->first_frame = first;
		animation->last_frame = last;
		animation->current_frame = first;
		animation->frame_play_time = time;
		animation->current_frame_time = 0;
	}
	
	return animation;
}

void sprite_animation_free( SpriteAnimation *animation )
{
	if( animation )
	{
		free(animation);
	}
}

void sprite_animation_update( SpriteAnimation *animation )
{
	animation->current_frame_time += time_manager_get_last_frame_time();
	
	if( animation->current_frame_time >= animation->frame_play_time )
	{
		animation->current_frame++;
		animation->current_frame_time = 0;
		
		if( animation->current_frame > animation->last_frame )
		{
			animation->current_frame = animation->first_frame;
		}
	}
}

#define ROTATION_STEP_SIZE 5

void sprite_animation_draw( SpriteAnimation *animation, Sprite *sprite, float x, float y, float angle ) {
	SDL_Rect src_rect;
	SDL_Rect dst_rect;
	
	src_rect.x = (sprite->rotatable ? sprite->width * ((int)angle / ROTATION_STEP_SIZE) : 0);
	src_rect.y = animation->current_frame * sprite->height;
	src_rect.w = sprite->width;
	src_rect.h = sprite->height;
	
	dst_rect.x = (int)x - (sprite->width / 2);
	dst_rect.y = (int)y - (sprite->height / 2);
	
	SDL_BlitSurface(sprite->surface, &src_rect, graphics_manager_get_video_surface(), &dst_rect);
}

