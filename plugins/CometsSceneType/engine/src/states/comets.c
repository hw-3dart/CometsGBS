#pragma bank 255

#include "data/states_defines.h"
#include "states/comets.h"

#include <gbdk/platform.h>
#include "system.h"
#include "gbs_types.h"
#include "events.h"
#include "actor.h"
#include "camera.h"
#include "collision.h"
#include "data_manager.h"
#include "game_time.h"
#include "input.h"
#include "scroll.h"
#include "math.h"
#include "vm.h"

#include <rand.h>

#define ACTORS_SIZE 4

UBYTE i = 0;
int8_t r = 0;
actor_t actor_id[ACTORS_SIZE];
int8_t actor_xscroll[ACTORS_SIZE];
int8_t actor_yscroll[ACTORS_SIZE];
static actor_t *actor;
UBYTE player_angle = 0;
int8_t player_xscroll = 0;
int8_t player_yscroll = 0;
int8_t player_speedlimit = 8;
UBYTE dir_change_max = 5;
UBYTE dir_change_current = 0;
UBYTE dir_current = 2;
UBYTE dir_max = 7;
UBYTE retrigger_max = 10;
UBYTE retrigger_current = 0;

void comets_player_set_frame(void) BANKED
{
    //Init
    UBYTE _temp_offset = 0;
    if (INPUT_A)
    {
        _temp_offset = 8;
    }
    actor_set_frame_offset(&PLAYER, dir_current + _temp_offset);
}
void comets_player_input_dir(void) BANKED
{
    if (INPUT_LEFT)
    {
        dir_change_current ++;
        if (dir_change_current >= dir_change_max)
        {
            //Reset
            dir_change_current = 0;

            //Adjust
            dir_current ++;
            if (dir_current > dir_max)
            {
                dir_current = 0;
            }
            comets_player_set_frame();
        }
    }
    else if (INPUT_RIGHT)
    {
        dir_change_current ++;
        if (dir_change_current >= dir_change_max)
        {
            //Reset
            dir_change_current = 0;

            //Adjust
            if (dir_current == 0)
            {
                dir_current = dir_max;
            }
            else
            {
                dir_current --;
            }
            comets_player_set_frame();
        }
    }
    else
    {
        dir_change_current = 0;
    }
}
void comets_player_input_accel(void) BANKED
{
    if (INPUT_A)
    {
        comets_player_set_frame();
        retrigger_current = retrigger_max;
        if (IS_FRAME_EVEN)
        {

        switch(dir_current) //Use GameMaker directions ^^
        {
            case 0: //RIGHT
                player_xscroll ++;
            break;

            case 1:
                player_xscroll ++;
                player_yscroll --;
            break;

            case 2: //UP
                player_yscroll --;
            break;

            case 3:
                player_xscroll --;
                player_yscroll --;
            break;

            case 4: //LEFT
                player_xscroll --;
            break;

            case 5:
                player_xscroll --;
                player_yscroll ++;
            break;

            case 6: //DOWN
                player_yscroll ++;
            break;

            case 7:
                player_xscroll ++;
                player_yscroll ++;
            break;
        }
        player_xscroll = CLAMP(player_xscroll, -player_speedlimit, player_speedlimit);
        player_yscroll = CLAMP(player_yscroll, -player_speedlimit, player_speedlimit);

        }
    }
}
void comets_get_dir(SCRIPT_CTX * THIS) OLDCALL BANKED
{
    *(UBYTE*)VM_REF_TO_PTR(FN_ARG1) = dir_current;
}
void comets_randomise_xyscroll(SCRIPT_CTX * THIS) OLDCALL BANKED
{
    UBYTE _temp_index = 255;
    _temp_index = *(UBYTE*)VM_REF_TO_PTR(FN_ARG0);
    do {
    r = (rand() % 11) - 5;
    } while (r == 0);
    actor_xscroll[_temp_index] = r * 2;
    do {
    r = (rand() % 11) - 5;
    } while (r == 0);
    actor_yscroll[_temp_index] = r * 2;
}

void comets_init(void) BANKED
{
    //Player Setup
    dir_current = 2;
    player_xscroll = 0;
    player_yscroll = 0;
    comets_player_set_frame();

    //Grab Actors
    actor = actors_active_tail;
    i = 0;
    while (actor)
    {
        if (actor != &PLAYER)
        {
            actor_id[i] = *actor;
            i ++;
        }
        actor = actor->prev;
    }

    //Random Scrolls
    i = 0;
    for(i = 0; i < ACTORS_SIZE; i++)
    {
        do {
        r = (rand() % 11) - 5;
        } while (r == 0);
        actor_xscroll[i] = r * 2;
    }
    i = 0;
    for(i = 0; i < ACTORS_SIZE; i++)
    {
        do {
        r = (rand() % 11) - 5;
        } while (r == 0);
        actor_yscroll[i] = r * 2;
    }

}

void comets_update(void) BANKED
{
    //Dir
    comets_player_input_dir();
    comets_player_input_accel();
    if (retrigger_current > 0)
    {
        retrigger_current --;
        if (retrigger_current == 0)
        {
            comets_player_set_frame();
        }
    }

    //Cache
    int8_t _temp_xscroll = player_xscroll;
    UBYTE _temp_xpos = (PLAYER.pos.x >> 4) + 8;
    int8_t _temp_yscroll = player_yscroll;
    UBYTE _temp_ypos = (PLAYER.pos.y >> 4);

    //Movement
    PLAYER.pos.x += _temp_xscroll;
    PLAYER.pos.y += _temp_yscroll;

    if (IS_FRAME_ODD)
    {
        //Wrap
        if (_temp_xscroll > 0 && _temp_xpos >= SCREEN_WIDTH)
        {
            PLAYER.pos.x -= (SCREEN_WIDTH << 4);
        }
        if (_temp_xscroll < 0 && _temp_xpos <= 0)
        {
            PLAYER.pos.x += (SCREEN_WIDTH << 4);
        }
        if (_temp_yscroll > 0 && _temp_ypos >= SCREEN_HEIGHT)
        {
            PLAYER.pos.y -= (SCREEN_HEIGHT << 4);
        }
        if (_temp_yscroll < 0 && _temp_ypos <= 0)
        {
            PLAYER.pos.y += (SCREEN_HEIGHT << 4);
        }
        
        //Collisions
        actor_t *hit_actor;
        hit_actor = actor_overlapping_player(FALSE);
        if (hit_actor != NULL && hit_actor->collision_group)
        {
            player_register_collision_with(hit_actor);
        }
    }

    //--

    //NPAs
    actor = actors_active_tail;
    i = 0;
    while (actor)
    {
        if (actor != &PLAYER)
        {
            //Move
            actor->pos.x += actor_xscroll[i];
            actor->pos.y += actor_yscroll[i];

            //Use Cache
            _temp_xscroll = actor_xscroll[i];
            _temp_xpos = (actor->pos.x >> 4) + 8;
            _temp_yscroll = actor_yscroll[i];
            _temp_ypos = (actor->pos.y >> 4);

            //OOB - Horizontal
            if (_temp_xscroll > 0 && _temp_xpos >= SCREEN_WIDTH)
            {
                actor->pos.x -= (SCREEN_WIDTH << 4);
            }
            if (_temp_xscroll < 0 && _temp_xpos <= 0)
            {
                actor->pos.x += (SCREEN_WIDTH << 4);
            }

            //OOB - Vertical
            if (_temp_yscroll > 0 && _temp_ypos >= SCREEN_HEIGHT)
            {
                actor->pos.y -= (SCREEN_HEIGHT << 4);
            }
            if (_temp_yscroll < 0 && _temp_ypos <= 0)
            {
                actor->pos.y += (SCREEN_HEIGHT << 4);
            }

            //Iterate
            i ++;
        }
        actor = actor->prev;
    }
}