#ifndef __STRUCTS_H__
#define __STRUCTS_H__

typedef struct __list                    List;
typedef struct __list_node               ListNode;
typedef struct __input_event_callback    InputEventCallback;
typedef struct __network_event_callback  NetworkEventCallback;
typedef struct __command_callback        CommandCallback;
typedef struct __map                     Map;
typedef struct __game_object             GameObject;
typedef struct __sprite                  Sprite;
typedef struct __sprite_animation        SpriteAnimation;
typedef struct __font                    Font;
typedef struct __string                  String;
typedef union  __network_message         NetworkMessage;
typedef struct __network_message_join    NetworkMessageJoin;
typedef struct __network_message_leave   NetworkMessageLeave;
typedef struct __network_message_update  NetworkMessageUpdate;
typedef struct __network_message_chat    NetworkMessageChat;
typedef struct __network_message_kill    NetworkMessageKill;
typedef struct __network_message_respawn NetworkMessageRespawn;
typedef struct __network_message_hit     NetworkMessageHit;
typedef struct __network_message_end_game NetworkMessageEndGame;
typedef struct __server_client           ServerClient;

struct __list
{
	ListNode     *first;
	ListNode     *last;
	unsigned int  size;
	void (*destroy) ( void *data );
};

struct __list_node
{
	ListNode *previous;
	ListNode *next;
	void     *data;
};

struct __input_event_callback
{
	void (*invoke) ( SDL_Event *event );
};

struct __network_event_callback
{
	void (*invoke) ( ENetEvent *event );
};

struct __map {
	SDL_Surface *surface;
	List        *boxes;
	char        *name;
};

struct __game_object
{
	int     id;
	float   x;
	float   y;
	int     width;
	int     height;
	float   angle;
	float   speed;
	int     health;
	Sprite *sprite;

	GameObject *origin;
	
	int slide_collision;
	
	void (*do_move)      ( GameObject *object, float radian_angle, float speed_percentage, float *new_x, float *new_y );
	int  (*do_collision) ( GameObject *source, GameObject *target );
	
	int move_forward;
	int move_backward;
	int strafe_left;
	int strafe_right;
};

struct __sprite
{
	SDL_Surface      *surface;
	int               width;
	int               height;
	int               rotatable;
	SpriteAnimation **animations;
	SpriteAnimation  *current_animation;
};

struct __sprite_animation
{
	int first_frame;
	int last_frame;
	int current_frame;
	int frame_play_time;
	int current_frame_time;
};

struct __font
{
	int          width;
	int          height;
	SDL_Surface *surface;
};

struct __command_callback
{
	void (*invoke) ( String *command, String **arguments, int num_arguments );
};

struct __string
{
	char         *data;
	unsigned int  size;
	unsigned int  length;
};

struct __network_message_join {
	int   type;
	int   id;
	float x;
	float y;
	float angle;
	float speed;
	char  map[32];
};

struct __network_message_leave {
	int type;
	int id;
};

struct __network_message_update {
	int   type;
	int   id;
	float x;
	float y;
	float angle;
	float speed;
	int   health;
	int   move_forward;
	int   move_backward;
	int   strafe_left;
	int   strafe_right;
};

struct __network_message_chat {
	int      type;
	int      id;
	char     message[32];
};

struct __network_message_kill {
	int type;
	int id;
};

struct __network_message_respawn {
	int type;
	int id;
};

struct __network_message_hit {
	int type;
	int source;
	int target;
};

struct __network_message_end_game {
	int type;
	int winner;
};

union __network_message {
	int                   type;
	NetworkMessageJoin    join;
	NetworkMessageLeave   leave;
	NetworkMessageUpdate  update;
	NetworkMessageChat    chat;
	NetworkMessageKill    kill;
	NetworkMessageRespawn respawn;
	NetworkMessageHit     hit;
	NetworkMessageEndGame end;
};

struct __server_client
{
	ENetPeer   *peer;
	GameObject *object;
	int         score;
};

#endif /* __STRUCTS_H */
