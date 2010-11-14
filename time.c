#include "include.h"

static unsigned int current_time;
static unsigned int last_time;
static unsigned int last_frame_time;

void time_manager_setup()
{
	current_time = SDL_GetTicks();
	last_time = current_time;
	last_frame_time = 0;
}

void time_manager_quit()
{
}

void time_manager_update()
{
	current_time = SDL_GetTicks();
	last_frame_time = current_time - last_time;
	last_time = current_time;
}

unsigned int time_manager_get_current_time()
{
	return current_time;
}

unsigned int time_manager_get_last_frame_time()
{
	return last_frame_time;
}
