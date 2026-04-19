/******************************************************************************/
/** @file obj_column.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Header file. Defines exported routines from obj_column.c
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     12 Dec 2007
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_OBJCOLMN_H
#define ADIKT_OBJCOLMN_H

struct LEVOPTIONS;
struct COLUMN_REC;

#include "globals.h"

#define CUST_CLM_GEN_MAX_INDEX  41

struct DK_CUSTOM_CLM {
    unsigned short wib_val;
    struct COLUMN_REC *rec;
  };

typedef void (*cr_clm_func)(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

DLLIMPORT unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw);
DLLIMPORT short column_wib_animate(unsigned int clm);

DLLIMPORT char *get_custom_column_fullname(unsigned short idx);
DLLIMPORT short fill_custom_column_data(unsigned short idx,struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

DLLIMPORT void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],struct LEVOPTIONS *optns,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
DLLIMPORT unsigned char *surr_tng_find(unsigned char **surr_tng,unsigned char type_idx);

void create_columns_slb_unaffected_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_fulldirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_walltorch(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallwwoman(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wallpairshr(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_unaffected_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_purple_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng);

void modify_frail_columns(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
short modify_liquid_surrounding(struct COLUMN_REC *clm_recs[9],unsigned char *surr_slb,
        short liq_level,unsigned short water_cube,unsigned short lava_cube);
short modify_liquid_surrounding_advncd(struct COLUMN_REC *clm_recs[9],
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const short liq_level,const unsigned short *water_cube,const unsigned short *lava_cube);
unsigned short *get_room_corner_direction_indices(unsigned char *surr_slb,unsigned char *surr_own);
unsigned short *get_room_edge_direction_indices(unsigned char *surr_slb,unsigned char *surr_own);

void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void fill_columns_slb_roomrelief(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
short fill_side_columns_room_relief(struct COLUMN_REC *clm_reca,struct COLUMN_REC *clm_recb,
    struct COLUMN_REC *clm_recc,unsigned short room_slab,unsigned char owner, short corner, short edge);

void create_columns_slb_skulls_on_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_skulls_on_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_skulls_on_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_wall_force_relief_splatbody(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_slb_room(cr_clm_func cr_floor,cr_clm_func cr_edge,
        cr_clm_func cr_corner,cr_clm_func cr_inside,cr_clm_func cr_nearinsd,
        struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_slb_portal_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_portal_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_treasure_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_library_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_prison_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_torture_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_training_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_dungheart_nearinsd(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_workshop_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_scavenger_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_temple_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_graveyard_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_hatchery_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_lair_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_barracks_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

void create_columns_slb_door_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

/*User columns - used only in "manual columns" function, not as standard ones */
void create_columns_slb_thingems_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_rock_gndlev(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_rockcaped_pathcave(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);
void create_columns_slb_rockcaped_claimcave(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng);

short surrnd_not_enemy(unsigned char *surr_own, short direction);

#endif /* ADIKT_OBJCOLMN_H */
