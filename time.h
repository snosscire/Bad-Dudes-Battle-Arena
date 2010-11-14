#ifndef __TIME_H__
#define __TIME_H__

void time_manager_setup  ();
void time_manager_quit   ();
void time_manager_update ();

unsigned int time_manager_get_current_time    ();
unsigned int time_manager_get_last_frame_time ();

#endif /* __TIME_H__ */
