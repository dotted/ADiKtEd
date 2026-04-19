/******************************************************************************/
/** @file obj_slabs.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from obj_slabs.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     07 Nov 2007
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_OBJSLABS_H
#define ADIKT_OBJSLABS_H

struct LEVEL;
struct IPOINT_2D;

#include "globals.h"

#define SLAB_TYPE_ROCK         0x00
#define SLAB_TYPE_GOLD         0x01
#define SLAB_TYPE_EARTH        0x02
#define SLAB_TYPE_TORCHDIRT    0x03
#define SLAB_TYPE_WALLDRAPE    0x04
#define SLAB_TYPE_WALLTORCH    0x05
#define SLAB_TYPE_WALLWTWINS   0x06
#define SLAB_TYPE_WALLWWOMAN   0x07
#define SLAB_TYPE_WALLPAIRSHR  0x08
#define SLAB_TYPE_PATH         0x0a
#define SLAB_TYPE_CLAIMED      0x0b
#define SLAB_TYPE_LAVA         0x0c
#define SLAB_TYPE_WATER        0x0d
#define SLAB_TYPE_PORTAL       0x0e
#define SLAB_TYPE_TREASURE     0x10
#define SLAB_TYPE_LIBRARY      0x12
#define SLAB_TYPE_PRISONCASE   0x14
#define SLAB_TYPE_TORTURE      0x16
#define SLAB_TYPE_TRAINING     0x18
#define SLAB_TYPE_DUNGHEART    0x1a
#define SLAB_TYPE_WORKSHOP     0x1c
#define SLAB_TYPE_SCAVENGER    0x1e
#define SLAB_TYPE_TEMPLE       0x20
#define SLAB_TYPE_GRAVEYARD    0x22
#define SLAB_TYPE_HATCHERY     0x24
#define SLAB_TYPE_LAIR         0x26
#define SLAB_TYPE_BARRACKS     0x28
#define SLAB_TYPE_DOORWOOD1    0x2a
#define SLAB_TYPE_DOORWOOD2    0x2b
#define SLAB_TYPE_DOORBRACE1   0x2c
#define SLAB_TYPE_DOORBRACE2   0x2d
#define SLAB_TYPE_DOORIRON1    0x2e
#define SLAB_TYPE_DOORIRON2    0x2f
#define SLAB_TYPE_DOORMAGIC1   0x30
#define SLAB_TYPE_DOORMAGIC2   0x31
#define SLAB_TYPE_BRIDGE       0x33
#define SLAB_TYPE_GEMS         0x34
#define SLAB_TYPE_GUARDPOST    0x35
#define SLAB_TYPE_PURPLE_PATH  0x36

#define PLAYERS_COUNT          6
#define PLAYER0                0x00
#define PLAYER1                0x01
#define PLAYER2                0x02
#define PLAYER3                0x03
#define PLAYER_GOOD            0x04
#define PLAYER_UNSET           0x05
#define PLAYER_ALL             0x06  /*used in scripts only */

extern char const SLB_UNKN_LTEXT[];
extern char const SLB_ROCK_LTEXT[];
extern char const SLB_GOLD_LTEXT[];
extern char const SLB_EARTH_LTEXT[];
extern char const SLB_TORCHDIRT_LTEXT[];
extern char const SLB_WALLDRAPE_LTEXT[];
extern char const SLB_WALLTORCH_LTEXT[];
extern char const SLB_WALLWTWINS_LTEXT[];
extern char const SLB_WALLWWOMAN_LTEXT[];
extern char const SLB_WALLPAIRSHR_LTEXT[];
extern char const SLB_PATH_LTEXT[];
extern char const SLB_CLAIMED_LTEXT[];
extern char const SLB_LAVA_LTEXT[];
extern char const SLB_WATER_LTEXT[];
extern char const SLB_PORTAL_LTEXT[];
extern char const SLB_TREASURE_LTEXT[];
extern char const SLB_LIBRARY_LTEXT[];
extern char const SLB_PRISON_LTEXT[];
extern char const SLB_TORTURE_LTEXT[];
extern char const SLB_TRAINING_LTEXT[];
extern char const SLB_DUNGHEART_LTEXT[];
extern char const SLB_WORKSHOP_LTEXT[];
extern char const SLB_SCAVENGER_LTEXT[];
extern char const SLB_TEMPLE_LTEXT[];
extern char const SLB_GRAVEYARD_LTEXT[];
extern char const SLB_HATCHERY_LTEXT[];
extern char const SLB_LAIR_LTEXT[];
extern char const SLB_BARRACKS_LTEXT[];
extern char const SLB_DOORWOOD_LTEXT[];
extern char const SLB_DOORBRACE_LTEXT[];
extern char const SLB_DOORIRON_LTEXT[];
extern char const SLB_DOORMAGIC_LTEXT[];
extern char const SLB_BRIDGE_LTEXT[];
extern char const SLB_GEMS_LTEXT[];
extern char const SLB_GUARDPOST_LTEXT[];
extern char const SLB_PURPLEPATH_LTEXT[];

DLLIMPORT short slab_is_central(struct LEVEL *lvl,int x,int y);
DLLIMPORT short slabs_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);

DLLIMPORT int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned short slab_type);
DLLIMPORT void slab_draw_smear(struct LEVEL *lvl,int startx,int starty,int startr,
                     int endx,int endy,int endr,int bend,unsigned short slab_type);
DLLIMPORT void slab_draw_circle(struct LEVEL *lvl,int x,int y,int rad,unsigned short slab_type);

DLLIMPORT short slab_is_short(unsigned short slab_type);
DLLIMPORT short slab_is_door(unsigned short slab_type);
DLLIMPORT short slab_is_room(unsigned short slab_type);
DLLIMPORT short slab_is_wall(unsigned short slab_type);
DLLIMPORT short slab_is_wealth(unsigned short slab_type);
DLLIMPORT short slab_is_space(unsigned short slab_type);
DLLIMPORT short slab_is_tall_unclmabl(unsigned short slab_type);
DLLIMPORT short slab_is_tall(unsigned short slab_type);
DLLIMPORT short slab_is_short_clmabl(unsigned short slab_type);
DLLIMPORT short slab_is_short_unclmabl(unsigned short slab_type);
DLLIMPORT short slab_is_clmabl(unsigned short slab_type);
DLLIMPORT short slab_is_claimedgnd(unsigned short slab_type);
DLLIMPORT short slab_is_liquid(unsigned short slab_type);
DLLIMPORT short slab_allows_torch(unsigned short slab_type);
DLLIMPORT short slab_needs_adjacent_torch(unsigned short slab_type);
DLLIMPORT short slab_verify_entry(unsigned short slab_type, char *err_msg);
DLLIMPORT char *get_slab_fullname(unsigned short slb_type);
DLLIMPORT unsigned short get_random_wall_slab(void);

DLLIMPORT short subtl_is_near_tall_slab(struct LEVEL *lvl,unsigned int sx,unsigned int sy);

DLLIMPORT short place_room_rndpos(struct LEVEL *lvl,const unsigned short rslab,
    const unsigned char rown,const struct IPOINT_2D *rsize);

DLLIMPORT char *get_owner_type_fullname(unsigned char own_idx);
DLLIMPORT char *get_owner_type_colorname(unsigned char own_idx);
DLLIMPORT unsigned char get_owner_next(unsigned char plyr_idx);
DLLIMPORT unsigned char get_owner_prev(unsigned char plyr_idx);
DLLIMPORT void update_slab_owners(struct LEVEL *lvl);

#endif /* ADIKT_OBJSLABS_H */
