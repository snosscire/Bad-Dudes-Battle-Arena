#include "include.h"

void game_quit()
{
	client_quit();
	server_quit();
	network_quit();
}

int main( int argc, char *argv[] )
{
	atexit(game_quit);
	network_setup();
	/* server_setup(); */
	client_setup();
	client_run();
	return 0;
}

