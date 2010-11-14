#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

#include "enet/enet.h"

#include "json.h"

#include "structs.h"
#include "client.h"
#include "server.h"
#include "list.h"
#include "client.h"
#include "time.h"
#include "graphics.h"
#include "input.h"
#include "network.h"
#include "map.h"
#include "game-object.h"
#include "sprite.h"
#include "client-callbacks.h"
#include "string_tobias.h"
#include "font.h"
#include "console.h"

#define AND &&
#define OR ||
#define NOT !
#define FALSE 0
#define TRUE ! FALSE

#define PI 3.14159265

#define PLAYER_SPRITE_WIDTH 60
#define PLAYER_SPRITE_HEIGHT 60
#define PLAYER_ANIMATIONS 2
#define PLAYER_ANIMATION_IDLE 0
#define PLAYER_ANIMATION_WALK 1

#endif /* __INCLUDE_H__ */
