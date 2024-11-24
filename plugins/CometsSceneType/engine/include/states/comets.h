#ifndef STATE_COMETS_H
#define STATE_COMETS_H

#include <gbdk/platform.h>

void comets_player_set_frame(void) BANKED;
void comets_player_input_dir(void) BANKED;
void comets_player_input_accel(void) BANKED;
void comets_init(void) BANKED;
void comets_update(void) BANKED;

#endif
