#ifndef __SPRITE_H__
#define __SPRITE_H__

Sprite * sprite_new     ();
void     sprite_free    ( Sprite *sprite );
void     sprite_update  ( Sprite *sprite );
void     sprite_draw    ( Sprite *sprite, float x, float y, float angle );

SpriteAnimation * sprite_animation_new ( int first, int last, int time );
void sprite_animation_free             ( SpriteAnimation *animation );
void sprite_animation_update           ( SpriteAnimation *animation );
void sprite_animation_draw             ( SpriteAnimation *animation, Sprite *sprite, float x, float y, float angle );

#endif /* __SPRITE_H__ */
