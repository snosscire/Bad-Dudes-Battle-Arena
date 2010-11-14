#ifndef __SERVER_H__
#define __SERVER_H__

void server_setup  ();
void server_quit   ();
void server_update ();

ServerClient * server_client_new  ();
void           server_client_free ( void *data );

#endif /* __SERVER_H__ */
