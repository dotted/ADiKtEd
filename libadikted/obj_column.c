/******************************************************************************/
/** @file obj_column.c
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Functions for maintaining single CLM entries.
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

#include "obj_column.h"

#include "lev_data.h"
#include "globals.h"
#include "obj_cube.h"
#include "obj_column_def.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "graffiti.h"
#include "bulcommn.h"

static void (*custom_columns_gen [])(struct COLUMN_REC *clm_recs[9],
        unsigned char *,unsigned char *, unsigned char **)={
     create_columns_slb_rock,create_columns_slb_gold,              /*00 */
     create_columns_slb_fulldirt,create_columns_slb_earth,
     create_columns_slb_torchdirt,
     create_columns_slb_walldrape,create_columns_slb_walltorch,
     create_columns_slb_wallwtwins,create_columns_slb_wallwwoman,
     create_columns_slb_wallpairshr,
     create_columns_slb_path,create_columns_slb_claimed,           /*0a */
     create_columns_slb_lava,create_columns_slb_water,
     create_columns_slb_portal,create_columns_slb_treasure,
     create_columns_slb_library,create_columns_slb_prison,
     create_columns_slb_torture,create_columns_slb_training,
     create_columns_slb_dungheart,create_columns_slb_workshop,     /*14 */
     create_columns_slb_scavenger,create_columns_slb_temple,
     create_columns_slb_graveyard,create_columns_slb_hatchery,
     create_columns_slb_lair,create_columns_slb_barracks,          /*1a */
     create_columns_slb_doorwood,create_columns_slb_doorbrace,
     create_columns_slb_dooriron,create_columns_slb_doormagic,
     create_columns_slb_bridge,create_columns_slb_gems,            /*20 */
     create_columns_slb_guardpost,
     create_columns_slb_thingems_path,create_columns_slb_rock_gndlev,
     create_columns_slb_rockcaped_pathcave,create_columns_slb_rockcaped_claimcave,
     create_columns_slb_skulls_on_lava,create_columns_slb_skulls_on_path,
     create_columns_slb_skulls_on_claimed,create_columns_slb_wall_force_relief_splatbody,
     };

const char * custom_columns_fullnames[]={
     "Standard Rock","Standard Gold",              /*00 */
     "Unaffected Earth","Standard Earth",
     SLB_TORCHDIRT_LTEXT,
     SLB_WALLDRAPE_LTEXT,SLB_WALLTORCH_LTEXT,
     SLB_WALLWTWINS_LTEXT,SLB_WALLWWOMAN_LTEXT,
     SLB_WALLPAIRSHR_LTEXT,
     "Standard Path",SLB_CLAIMED_LTEXT,           /*0A */
     "Standard Lava","Standard Water",
     SLB_PORTAL_LTEXT,SLB_TREASURE_LTEXT,
     SLB_LIBRARY_LTEXT,SLB_PRISON_LTEXT,
     SLB_TORTURE_LTEXT,SLB_TRAINING_LTEXT,
     SLB_DUNGHEART_LTEXT,SLB_WORKSHOP_LTEXT,     /*14 */
     SLB_SCAVENGER_LTEXT,SLB_TEMPLE_LTEXT,
     SLB_GRAVEYARD_LTEXT,SLB_HATCHERY_LTEXT,
     SLB_LAIR_LTEXT,SLB_BARRACKS_LTEXT,          /*1A */
     SLB_DOORWOOD_LTEXT,SLB_DOORBRACE_LTEXT,
     SLB_DOORIRON_LTEXT,SLB_DOORMAGIC_LTEXT,
     SLB_BRIDGE_LTEXT,"Standard Gems",            /*20 */
     SLB_GUARDPOST_LTEXT,
     "Thin gems on path","Ground level rock",
     "Rock with cave","Rock w/claimed cave",
     "Skulls on Lava","Skulls on Path",
     "Skulls on Claimed","Wall w/body splat",
     };

const unsigned short dir_rot_000[]={
      IDIR_NW,IDIR_NORTH,IDIR_NE,IDIR_WEST,IDIR_CENTR,IDIR_EAST,
      IDIR_SW,IDIR_SOUTH,IDIR_SE, };

const unsigned short dir_rot_090[]={
      IDIR_NE,IDIR_EAST,IDIR_SE,IDIR_NORTH,IDIR_CENTR,IDIR_SOUTH,
      IDIR_NW,IDIR_WEST,IDIR_SW, };

const unsigned short dir_rot_180[]={
      IDIR_SE,IDIR_SOUTH,IDIR_SW,IDIR_EAST,IDIR_CENTR,IDIR_WEST,
      IDIR_NE,IDIR_NORTH,IDIR_NW, };

const unsigned short dir_rot_270[]={
      IDIR_SW,IDIR_WEST,IDIR_NW,IDIR_SOUTH,IDIR_CENTR,IDIR_NORTH,
      IDIR_SE,IDIR_EAST,IDIR_NE, };

short fill_reinforced_corner=true;
short frail_columns_near_short=true;
short frail_columns_near_tall=true;

/*
 * Returns custom column type name as text
 */
char *get_custom_column_fullname(unsigned short idx)
{
    int types_count=sizeof(custom_columns_fullnames)/sizeof(char *);
    if (idx<types_count)
      return (char *)custom_columns_fullnames[idx];
    else
      return "unknown(?!)";
}

/*
 * Executes custom column filling function with given index
 */
short fill_custom_column_data(unsigned short idx,struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    custom_columns_gen[idx](clm_recs,surr_slb,surr_own,surr_tng);
    return true;
}

/*
 * Fills up 9 CLM entries needed for given slab with specified surroundings.
 */
void create_columns_for_slab(struct COLUMN_REC *clm_recs[9],struct LEVOPTIONS *optns,
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned short slab=surr_slb[IDIR_CENTR];
  frail_columns_near_short=((optns->frail_columns&1)==1);
  frail_columns_near_tall=((optns->frail_columns&2)==2);
  switch (slab)
  {
    case SLAB_TYPE_ROCK:
      if (optns->unaffected_rock)
        create_columns_slb_unaffected_rock(clm_recs,surr_slb,surr_own,surr_tng);
      else
        create_columns_slb_rock(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GOLD:
      create_columns_slb_gold(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_EARTH:
      create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORCHDIRT:
      create_columns_slb_torchdirt(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLDRAPE:
      fill_reinforced_corner=optns->fill_reinforced_corner;
      create_columns_slb_walldrape(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLTORCH:
      fill_reinforced_corner=optns->fill_reinforced_corner;
      create_columns_slb_walltorch(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWTWINS:
      fill_reinforced_corner=optns->fill_reinforced_corner;
      create_columns_slb_wallwtwins(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLWWOMAN:
      fill_reinforced_corner=optns->fill_reinforced_corner;
      create_columns_slb_wallwwoman(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WALLPAIRSHR:
      fill_reinforced_corner=optns->fill_reinforced_corner;
      create_columns_slb_wallpairshr(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PATH:
      create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_CLAIMED:
      create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAVA:
      create_columns_slb_lava(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WATER:
      create_columns_slb_water(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PORTAL:
      create_columns_slb_portal(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TREASURE:
      create_columns_slb_treasure(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LIBRARY:
      create_columns_slb_library(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_columns_slb_prison(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TORTURE:
      create_columns_slb_torture(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TRAINING:
      create_columns_slb_training(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_columns_slb_dungheart(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_columns_slb_workshop(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_columns_slb_scavenger(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_TEMPLE:
      create_columns_slb_temple(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_columns_slb_graveyard(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_HATCHERY:
      create_columns_slb_hatchery(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_LAIR:
      create_columns_slb_lair(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BARRACKS:
      create_columns_slb_barracks(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      create_columns_slb_doorwood(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      create_columns_slb_doorbrace(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      create_columns_slb_dooriron(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      create_columns_slb_doormagic(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_BRIDGE:
      create_columns_slb_bridge(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GEMS:
      if (optns->unaffected_gems)
        create_columns_slb_unaffected_gems(clm_recs,surr_slb,surr_own,surr_tng);
      else
        create_columns_slb_gems(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_columns_slb_guardpost(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    case SLAB_TYPE_PURPLE_PATH:
      create_columns_slb_purple_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    default: /*on error, make purple_path */
      create_columns_slb_purple_path(clm_recs,surr_slb,surr_own,surr_tng);
      break;
    }
}

/*
 * A helper function for using surr_own array
 */
short surrnd_not_enemy(unsigned char *surr_own, short direction)
{
    if ((surr_own[IDIR_CENTR]==surr_own[direction%9])
      ||((surr_own[direction%9])>=PLAYER_UNSET))
        return true;
    return false;
}

/*
 * A helper function for using surr_tng array
 */
unsigned char *surr_tng_find(unsigned char **surr_tng,unsigned char type_idx)
{
  unsigned char *thing=surr_tng[IDIR_CENTR];
  if (thing!=NULL)
  {
    if (get_thing_type(thing)==type_idx)
      return thing;
  }
  int i;
  for (i=0;i<9;i++)
  {
    thing=surr_tng[i];
    if (thing!=NULL)
    {
      if (get_thing_type(thing)==type_idx)
        return thing;
    }
  }
  return NULL;
}

void create_columns_slb_unaffected_rock(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb, unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_rock(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_unaffected_rock(clm_recs,surr_slb,surr_own,surr_tng);
  /*Switch (remove) corner columns near lava,water,... */
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_gold(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  /*Central column */
  fill_column_gold(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  /*Note: can't use modify_liquid_surrounding() because more than one cube changes near water */
  /*corner columns */
  int i;
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER) || (surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA) || (surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
  }
  /* Remaining edge columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
      fill_column_gold_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
      fill_column_gold_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
    else
      fill_column_gold(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
  /*Switch (remove) corner columns near lava,water,... */
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

/*
 * Creates earth which are all filled with dirt, always.
 * May be used for making earth and torchdirt columns.
 */
void create_columns_slb_fulldirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  /*Center is always earth */
  fill_column_earth(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);

  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  /*Note: can't use modify_liquid_surrounding() - two cubes changes */
  /*Now corner slabs */
  for (i=0;i<4;i++)
  {
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      {
         fill_column_earth_nearwater(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      {
         fill_column_earth_nearlava(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      } else
      {
         fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      }
  }
  /* And the middle ones */
  for (i=0;i<4;i++)
  {
      if (surr_slb[dir_a[i]]==SLAB_TYPE_WATER)
         fill_column_earth_nearwater(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
      if (surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)
         fill_column_earth_nearlava(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
      else
         fill_column_earth(clm_recs[dir_a[i]],surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_earth(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*Make the standard, 9x9 columns filled with dirt */
  create_columns_slb_fulldirt(clm_recs,surr_slb,surr_own,surr_tng);
  /*Finally - switch corner columns near lava,water,... */
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torchdirt(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  /*This is identical to standard dirt */
  create_columns_slb_earth(clm_recs,surr_slb,surr_own,surr_tng);
  /*But one of the c[3] entries is replaced with torch-one */
  int i;
  short has_torches=false;
  for (i=0;i<4;i++)
  {
/* old way - without things (pending delete, but first we must have auto-adding things) */
/*      if (slab_is_room(surr_slb[dir_a[i]])||slab_is_space(surr_slb[dir_a[i]])) */
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        /*Add the torch plate at same height where the torch thing is */
        unsigned short height=get_thing_subtile_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x018;
        has_torches=true;
    }
  }
  if (!has_torches)
    modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_skulls_on_lava(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_lava(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_skulls_on_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_path(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_skulls_on_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_wall_lair_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

/*
 * Creates wall with red brick inside. Returns where are whole brick walls
 */
void create_columns_slb_wallbrick(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
/*TODO: add shadow to central cobblestones near water and lava */
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  int i;
  /*Filling matrix */
  for (i=0;i<9;i++)
    allow_relief[i]=true;
  /*We will use the NW, NE, SW, SE values of the matrix to help making brick */
  /* with shadow */
  /*Central slab identifies the player */
  fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  /*Corner slabs - white brick, small red brick or clear earth */
  for (i=0;i<4;i++)
  {
    /* Place the floor */
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER) || (surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
      fill_column_wallground_nearwater(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA) || (surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
      fill_column_wallground_nearlava(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_earthground(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    /* If we're surrounded by our wall, and there is something short also - red brick */
    if (((slab_is_wall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
       &&((slab_is_short(surr_slb[dir_b[i]]))||(!surrnd_not_enemy(surr_own,dir_b[i]))))
       ||((slab_is_wall(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
       &&((slab_is_short(surr_slb[dir_a[i]]))||(!surrnd_not_enemy(surr_own,dir_a[i])))))
    {
      /*Note: can't use modify_liquid_surrounding() because more than one cube changes near water */
      /*corner columns */
        if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER) || (surr_slb[dir_b[i]]==SLAB_TYPE_WATER) ||
            (surr_slb[dir_a[i]]==SLAB_TYPE_LAVA) || (surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
          place_column_wall_redsmbrick(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
        else
          place_column_wall_redsmbrick_dkbtm(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    } else
    /* If we're surrounded by our wall, and there are doors in front - red brick, but without relief */
    if (((slab_is_wall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
       &&((slab_is_door(surr_slb[dir_b[i]]))||(!surrnd_not_enemy(surr_own,dir_b[i]))))
       ||((slab_is_wall(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
       &&((slab_is_door(surr_slb[dir_a[i]]))||(!surrnd_not_enemy(surr_own,dir_a[i])))))
    {
        place_column_wall_redsmbrick_dkbtm(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
        if (slab_is_door(surr_slb[dir_a[i]]))
          allow_relief[dir_a[i]]=false;
        if (slab_is_door(surr_slb[dir_b[i]]))
          allow_relief[dir_b[i]]=false;
    } else
    /* if we're surrouned with unowned tall thing on _both_ sides, fill with dirt */
    if ((slab_is_tall_unclmabl(surr_slb[dir_a[i]])&&slab_is_tall_unclmabl(surr_slb[dir_b[i]]))
      ||(((slab_is_tall(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
      &&(slab_is_tall(surr_slb[dir_b[i]])&& surrnd_not_enemy(surr_own,dir_b[i])))))
    {
      if ((fill_reinforced_corner)&&(slab_is_short(surr_slb[dir_c[i]]))
          &&(!(surr_slb[dir_c[i]]==SLAB_TYPE_PATH)))
      {
        if (fill_reinforced_corner==1)
          place_column_wall_cobblestones(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
        else
          place_column_wall_cobblestones_mk(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      } else
      {
        fill_column_earth(clm_recs[dir_c[i]],surr_own[IDIR_CENTR]);
      }
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
    } else
    /* Finally - if there's something short, like ground - use the white brick */
    {
      place_column_wall_cobblestones(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      allow_relief[dir_a[i]]=false;
      allow_relief[dir_b[i]]=false;
      allow_relief[dir_c[i]]=false;
    }
  }
  /* And the remaining, middle slabs */
  for (i=0;i<4;i++)
  {
    /* Place the floor */
    if (surr_slb[dir_a[i]]==SLAB_TYPE_WATER)
      fill_column_wallground_nearwater(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
    if (surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)
      fill_column_wallground_nearlava(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_earthground(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  /*These cannot be taken in simple 'for' loop, because different directions uses */
  /* different fill_column_wall_redsmbrick_* functions. */
  if ((slab_is_wall(surr_slb[IDIR_NORTH])&&surrnd_not_enemy(surr_own,IDIR_NORTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH]))
  {
    fill_column_earth(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
  } else
  if ((surr_slb[IDIR_NORTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_NORTH]==SLAB_TYPE_LAVA))
  {
    if (allow_relief[IDIR_NORTH])
      place_column_wall_cobblestones(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    else
      place_column_wall_redsmbrick(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_NORTH]=false;
  } else
  if (allow_relief[IDIR_NE])
  { /* just standard red brick */
    place_column_wall_redsmbrick_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
  } else
  { /* the "b" brick has dark edge from this view */
    place_column_wall_redsmbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_EAST])&&surrnd_not_enemy(surr_own,IDIR_EAST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_EAST]))
  {
    fill_column_earth(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_EAST]=false;
  } else
  if ((surr_slb[IDIR_EAST]==SLAB_TYPE_WATER)||(surr_slb[IDIR_EAST]==SLAB_TYPE_LAVA))
  {
    if (allow_relief[IDIR_EAST])
      place_column_wall_cobblestones(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    else
      place_column_wall_redsmbrick(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_EAST]=false;
  } else
  if (allow_relief[IDIR_SE])
  {   /* just standard red brick */
      place_column_wall_redsmbrick_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
  } else
  {   /* the "b" brick has dark edge from this view */
      place_column_wall_redsmbrick_b(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_SOUTH])&&surrnd_not_enemy(surr_own,IDIR_SOUTH))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
  {
    fill_column_earth(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
  } else
  if ((surr_slb[IDIR_SOUTH]==SLAB_TYPE_WATER)||(surr_slb[IDIR_SOUTH]==SLAB_TYPE_LAVA))
  {
    if (allow_relief[IDIR_SOUTH])
      place_column_wall_cobblestones(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    else
      place_column_wall_redsmbrick(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_SOUTH]=false;
  } else
  if (allow_relief[IDIR_SW])
  {   /* just standard red brick */
      place_column_wall_redsmbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
  } else
  {   /* the "c" brick has dark edge from this view */
      place_column_wall_redsmbrick_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
  }

  if ((slab_is_wall(surr_slb[IDIR_WEST])&&surrnd_not_enemy(surr_own,IDIR_WEST))
    ||slab_is_tall_unclmabl(surr_slb[IDIR_WEST]))
  {
    fill_column_earth(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
    allow_relief[IDIR_WEST]=false;
  } else
  if ((surr_slb[IDIR_WEST]==SLAB_TYPE_WATER)||(surr_slb[IDIR_WEST]==SLAB_TYPE_LAVA))
  {
    if (allow_relief[IDIR_WEST])
      place_column_wall_cobblestones(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    else
      place_column_wall_redsmbrick(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    allow_relief[IDIR_WEST]=false;
  } else
  if (allow_relief[IDIR_NW])
  {   /* just standard red brick */
      place_column_wall_redsmbrick_b(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  } else
  {   /* the "c" brick has dark edge from this view */
      place_column_wall_redsmbrick_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  }
  /*No frail columns switching on reinforced wall, so we're done. */
}

/*
 * If some of the walls are next to room, fills the columns with room relief.
 * Modifies allow_relief setting of the modified columns to false.
 */
void fill_columns_slb_roomrelief(struct COLUMN_REC *clm_recs[9], short *allow_relief,
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_NW,   IDIR_NW,   IDIR_NE,   IDIR_SW};
  const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_c[]={IDIR_SW,   IDIR_NE,   IDIR_SE,   IDIR_SE};
  const unsigned short dir_d[]={IDIR_NORTH,IDIR_WEST, IDIR_NORTH,IDIR_WEST,};
  const unsigned short dir_e[]={IDIR_SOUTH,IDIR_EAST, IDIR_SOUTH,IDIR_EAST,};
  int i;
  for (i=0;i<4;i++)
  {
    unsigned short slab=surr_slb[dir_b[i]];
    if ((allow_relief[dir_b[i]])&&(slab_is_room(slab)))
    {
      /*TODO: check edge and corner if they're set properly */
      short corner=false;
      short edge=false;
      /* 'edge' means that the wall starts/ends/changes direction after current slab */
      if (((!slab_is_short(surr_slb[dir_d[i]]))&&(surrnd_not_enemy(surr_own,dir_d[i])))
        ||((!slab_is_short(surr_slb[dir_e[i]]))&&(surrnd_not_enemy(surr_own,dir_e[i]))))
        edge=true;
      /* 'corner' means that the room ends after current slab */
      if ((!slab_is_room(surr_slb[dir_a[i]]))||(!slab_is_room(surr_slb[dir_c[i]])))
        corner=true;
      if (fill_side_columns_room_relief(clm_recs[dir_a[i]],clm_recs[dir_b[i]],
          clm_recs[dir_c[i]],slab,surr_own[IDIR_CENTR],corner,edge))
        allow_relief[dir_b[i]]=false;
    }
  }
}

/*
 * Fills columns with relief specific for given room. Returns false if given room
 * has no its own wall, or given slab is not room. 'corner' indicates
 * that a room corner columns should be used. 'edge' means that the wall
 * ends after or before these columns.
 */
short fill_side_columns_room_relief(struct COLUMN_REC *clm_reca,struct COLUMN_REC *clm_recb,
    struct COLUMN_REC *clm_recc,unsigned short room_slab,unsigned char owner, short corner, __attribute__((unused)) short edge)
{
  /*Note: we're using fill_column_wall_drapebrick to save CLM space. */
  /*TODO: make corner and edge support */
  switch (room_slab)
  {
  case SLAB_TYPE_PORTAL:
    place_column_wall_portal_a(clm_reca,owner);
    place_column_wall_portal_b(clm_recb,owner);
    place_column_wall_portal_a(clm_recc,owner);
    return true;
  case SLAB_TYPE_TREASURE:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    if (!corner)
    {
      place_column_wall_treasure_a(clm_reca,owner);
      place_column_wall_treasure_b(clm_recb,owner);
      place_column_wall_treasure_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_LIBRARY:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    if (corner)
    {
      /*TODO */
    } else
    {
      place_column_wall_library_a(clm_reca,owner);
      place_column_wall_library_b(clm_recb,owner);
      place_column_wall_library_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_PRISONCASE:
    if (corner)
    {
      fill_column_wall_drapebrick_a(clm_reca,owner);
      fill_column_wall_drapebrick_b(clm_recb,owner);
      fill_column_wall_drapebrick_c(clm_recc,owner);
      /* TODO */
    } else
    {
      place_column_wall_prison_a(clm_reca,owner);
      place_column_wall_prison_b(clm_recb,owner);
      place_column_wall_prison_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_TORTURE:
    place_column_wall_torture_a(clm_reca,owner);
    place_column_wall_torture_b(clm_recb,owner);
    place_column_wall_torture_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_TRAINING:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    if (corner)
    {
      /*TODO */
    } else
    {
      place_column_wall_training_a(clm_reca,owner);
      place_column_wall_training_b(clm_recb,owner);
      place_column_wall_training_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_WORKSHOP:
    if (corner)
    {
      fill_column_wall_drapebrick_a(clm_reca,owner);
      fill_column_wall_drapebrick_b(clm_recb,owner);
      fill_column_wall_drapebrick_c(clm_recc,owner);
      /* TODO */
    } else
    {
      place_column_wall_workshop_a(clm_reca,owner);
      place_column_wall_workshop_b(clm_recb,owner);
      place_column_wall_workshop_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_SCAVENGER:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    if (corner)
    {
      /*TODO */
    } else
    {
      place_column_wall_scavenger_a(clm_reca,owner);
      place_column_wall_scavenger_b(clm_recb,owner);
      place_column_wall_scavenger_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_TEMPLE:
    if (corner)
    {
      fill_column_wall_drapebrick_a(clm_reca,owner);
      fill_column_wall_drapebrick_b(clm_recb,owner);
      fill_column_wall_drapebrick_c(clm_recc,owner);
      /* TODO */
    } else
    {
      place_column_wall_temple_a(clm_reca,owner);
      place_column_wall_temple_b(clm_recb,owner);
      place_column_wall_temple_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_GRAVEYARD:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    place_column_wall_graveyard_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    place_column_wall_graveyard_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    place_column_wall_graveyard_c(clm_recc,owner);
    return true;
  case SLAB_TYPE_HATCHERY:
    fill_column_wall_drapebrick_a(clm_reca,owner);
    fill_column_wall_drapebrick_b(clm_recb,owner);
    fill_column_wall_drapebrick_c(clm_recc,owner);
    if (corner)
    {
      /*TODO */
    } else
    {
      place_column_wall_hatchery_a(clm_reca,owner);
      place_column_wall_hatchery_b(clm_recb,owner);
      place_column_wall_hatchery_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_LAIR:
    if (corner)
    {
      place_column_wall_laircrnr_a(clm_reca,owner);
      place_column_wall_laircrnr_b(clm_recb,owner);
      place_column_wall_laircrnr_c(clm_recc,owner);
    } else
    {
      place_column_wall_lair_a(clm_reca,owner);
      place_column_wall_lair_b(clm_recb,owner);
      place_column_wall_lair_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_BARRACKS:
    if (corner)
    {
      fill_column_wall_drapebrick_a(clm_reca,owner);
      fill_column_wall_drapebrick_b(clm_recb,owner);
      fill_column_wall_drapebrick_c(clm_recc,owner);
      /* TODO */
    } else
    {
      place_column_wall_barracks_a(clm_reca,owner);
      place_column_wall_barracks_b(clm_recb,owner);
      place_column_wall_barracks_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_DUNGHEART:
    if (corner)
    {
      fill_column_wall_drapebrick_a(clm_reca,owner);
      fill_column_wall_drapebrick_b(clm_recb,owner);
      fill_column_wall_drapebrick_c(clm_recc,owner);
    }
    return true;
  case SLAB_TYPE_BRIDGE:
  case SLAB_TYPE_GUARDPOST:
  default:
    return false;
  }
}

/*
 * Creates wall with splatted dead body (torture chamber specific)
 */
void create_columns_slb_wall_force_relief_splatbody(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    const unsigned short dir_a[]={IDIR_NW,   IDIR_NE,   IDIR_SE,   IDIR_SW};
    const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
    /* Filling the wall with brick & cobblestones on edges */
    int i;
    for (i=0;i<4;i++)
    {
      fill_column_wall_cobblestones(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_redsmbrick_b(clm_recs[dir_b[i]], surr_own[IDIR_CENTR]);
    }
    fill_column_wall_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
    /* Finding best orientation for the relief */
    const unsigned short *dir;
    short draw_oposite;
    if (slab_is_short(surr_slb[IDIR_WEST]) && slab_is_short(surr_slb[IDIR_NORTH]))
    {
      dir=dir_rot_000;
      draw_oposite=false;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) && slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_180;
      draw_oposite=false;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) && slab_is_short(surr_slb[IDIR_WEST]))
    {
      dir=dir_rot_090;
      draw_oposite=true;
    } else
    if (slab_is_short(surr_slb[IDIR_NORTH]) && slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_000;
      draw_oposite=true;
    } else
    if (slab_is_short(surr_slb[IDIR_EAST]) || slab_is_short(surr_slb[IDIR_SOUTH]))
    {
      dir=dir_rot_180;
      draw_oposite=false;
    } else
    /*if (slab_is_short(surr_slb[IDIR_WEST]) || slab_is_short(surr_slb[IDIR_NORTH])) */
    {
      dir=dir_rot_000;
      draw_oposite=false;
    }
    /* Drawing the reliefs */
    if (draw_oposite)
    {
        place_column_wall_torture_a(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[7]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[8]],surr_own[IDIR_CENTR]);
    } else
    {
      if (dir==dir_rot_180)
      {
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[3]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
      } else
      {
        place_column_wall_torture_b(clm_recs[dir[1]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_b(clm_recs[dir[3]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[2]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_a(clm_recs[dir[0]],surr_own[IDIR_CENTR]);
        place_column_wall_torture_c(clm_recs[dir[6]],surr_own[IDIR_CENTR]);
      }
    }
}

/*
 * Creates wall with small drape at top
 */
void create_columns_slb_walldrape(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*This variable will help in placing the bas-relief or drape; */
  /*only four of its values will be used */
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  /*If there's enought place for drape - draw it */
  const unsigned short dir_a[]={IDIR_NW,   IDIR_NW,   IDIR_NE,   IDIR_SW};
  const unsigned short dir_b[]={IDIR_WEST, IDIR_NORTH,IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_c[]={IDIR_SW,   IDIR_NE,   IDIR_SE,   IDIR_SE};
  int i;
  for (i=0;i<4;i++)
  {
    if (allow_relief[dir_b[i]])
    {
      fill_column_wall_drapebrick_a(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_drapebrick_b(clm_recs[dir_b[i]], surr_own[IDIR_CENTR]);
      fill_column_wall_drapebrick_c(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    }
  }
}

/*
 * Creates wall with torch plate
 */
void create_columns_slb_walltorch(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  /*This variable will help in placing the bas-relief or drape; */
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
    /* Torch plate */
  int i;
  for (i=0;i<4;i++)
  {
    unsigned char *thing=surr_tng[dir_a[i]];
    if ((thing!=NULL)&&(is_torch(thing)))
    {
        /*Add the torch plate at same height where the torch thing is */
        unsigned short height=get_thing_subtile_h(thing);
        if (get_thing_subtpos_h(thing)>127) height++;
        clm_recs[dir_a[i]]->c[height%8]=0x077;
        allow_relief[dir_a[i]]=false;
    }
  }
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_wallwtwins(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*This variable will help in placing the bas-relief or drape; */
  /*only four of its values will be used */
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  /*If there's enought place for drape - draw it */
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_twinsbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_twinsbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}
void create_columns_slb_wallwwoman(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*This variable will help in placing the bas-relief or drape; */
  /*only four of its values will be used */
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  /*If there's enought place for drape - draw it */
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_womanbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_womanbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_wallpairshr(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*This variable will help in placing the bas-relief or drape; */
  /*only four of its values will be used */
  short allow_relief[9];
  create_columns_slb_wallbrick(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  fill_columns_slb_roomrelief(clm_recs,allow_relief,surr_slb,surr_own,surr_tng);
  /*If there's enought place for drape - draw it */
  if (allow_relief[IDIR_NORTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_SOUTH])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_EAST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NE],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_EAST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SE],    surr_own[IDIR_CENTR]);
  }
  if (allow_relief[IDIR_WEST])
  {
    fill_column_wall_pairshrbrick_a(clm_recs[IDIR_NW],    surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_b(clm_recs[IDIR_WEST],  surr_own[IDIR_CENTR]);
    fill_column_wall_pairshrbrick_c(clm_recs[IDIR_SW],    surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_purple_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_purple_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_claimed(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};

  /*Center */
  fill_column_claimedgnd_centr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);

  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_rec_sim(clm_recs[dir_c[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i])&&
        slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { /*Surrounded by our area */
       clm_recs[dir_c[i]]->c[0]=0x07e +rnd(3);
    } else
    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    { /*our on side A only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*West only, not north */
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_NE:/*North only, not east */
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        case IDIR_SW:/*South only, not west */
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SE:/*East only, not south */
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        }
    } else
    if (slab_is_claimedgnd(surr_slb[dir_b[i]])&&surrnd_not_enemy(surr_own,dir_b[i]))
    { /*our on side B only */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x084;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x087;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x082;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x085;
           break;
        }
    } else
    { /*Surrounded by not our area */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x081;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x083;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x086;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x088;
          break;
        }
    }
  }

  /*And the middle columns */
  for (i=0;i<4;i++)
  {
    fill_column_rec_sim(clm_recs[dir_a[i]], 0, 0x0ce,
         0x07f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);

    if (slab_is_claimedgnd(surr_slb[dir_a[i]])&&surrnd_not_enemy(surr_own,dir_a[i]))
    {
       clm_recs[dir_a[i]]->c[0]=0x07e +rnd(3);
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x087;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x084;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x082;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x085;
           break;
        }
    }
  }
  /* Water and lava can beat all previous conditions */
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x094, 0x093);
}

void create_columns_slb_lava(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_lava(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_water(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_water(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_unaffected_gems(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  /*These are, in official editor, same for every surrounding, even near water */
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_gem(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_gems(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_unaffected_gems(clm_recs,surr_slb,surr_own,surr_tng);
  /*Switch corner columns near lava,water,... */
  modify_frail_columns(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_thingems_path(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  fill_column_gem(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_portal(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_portal_floor,create_columns_slb_portal_edge,
        create_columns_slb_portal_corner,create_columns_slb_portal_inside,create_columns_slb_portal_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_treasure_floor,create_columns_slb_treasure_edge,
       create_columns_slb_treasure_corner,create_columns_slb_treasure_inside,create_columns_slb_treasure_floor,
       clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_library_floor,create_columns_slb_library_edge,
        create_columns_slb_library_corner,create_columns_slb_library_inside,create_columns_slb_library_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_prison_floor,create_columns_slb_prison_edge,
        create_columns_slb_prison_corner,create_columns_slb_prison_inside,create_columns_slb_prison_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_torture_floor,create_columns_slb_torture_edge,
        create_columns_slb_torture_corner,create_columns_slb_torture_inside,create_columns_slb_torture_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_training_floor,create_columns_slb_training_edge,
        create_columns_slb_training_corner,create_columns_slb_training_inside,create_columns_slb_training_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_dungheart(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    /*Note: the 'floor' function is different in DK, and floor can change if user */
    /* force it to update in the game. */
    create_columns_slb_room(create_columns_slb_dungheart_nearinsd,create_columns_slb_dungheart_edge,
        create_columns_slb_dungheart_corner,create_columns_slb_dungheart_inside,create_columns_slb_dungheart_nearinsd,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_workshop_floor,create_columns_slb_workshop_edge,
        create_columns_slb_workshop_corner,create_columns_slb_workshop_inside,create_columns_slb_workshop_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_scavenger_floor,create_columns_slb_scavenger_edge,
        create_columns_slb_scavenger_corner,create_columns_slb_scavenger_inside,create_columns_slb_scavenger_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_temple(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_temple_floor,create_columns_slb_temple_edge,
        create_columns_slb_temple_corner,create_columns_slb_temple_inside,create_columns_slb_temple_edge,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_graveyard_floor,create_columns_slb_graveyard_edge,
        create_columns_slb_graveyard_corner,create_columns_slb_graveyard_inside,create_columns_slb_graveyard_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_hatchery_floor,create_columns_slb_hatchery_edge,
        create_columns_slb_hatchery_corner,create_columns_slb_hatchery_inside,create_columns_slb_hatchery_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_lair_floor,create_columns_slb_lair_edge,
        create_columns_slb_lair_corner,create_columns_slb_lair_inside,create_columns_slb_lair_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_room(create_columns_slb_barracks_floor,create_columns_slb_barracks_edge,
        create_columns_slb_barracks_corner,create_columns_slb_barracks_inside,create_columns_slb_barracks_floor,
        clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_door_floor(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,__attribute__((unused)) unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    fill_column_rec_sim(clm_recs[IDIR_NW],    0, CUBE_PATH_SMOOTH3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NORTH], 0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_NE],    0, 0x0b5,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_WEST],  0, 0x0b3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_CENTR], 0, 0x0b1,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_EAST],  0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SW],    0, 0x0b8,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SOUTH], 0, 0x0ba,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
    fill_column_rec_sim(clm_recs[IDIR_SE],    0, CUBE_PATH_SMOOTH3,
        0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_doorwood(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorwood_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorwood_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorwood_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorwood_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_doorbrace(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doorbrace_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doorbrace_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doorbrace_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doorbrace_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_dooriron(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_dooriron_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_dooriron_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_dooriron_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_dooriron_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_doormagic(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_door_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned char *thing=surr_tng_find(surr_tng,THING_TYPE_DOOR);
    short orient=DOOR_ORIENT_NSPASS;
    if (thing!=NULL) orient=get_door_orientation(thing);
    if (orient==DOOR_ORIENT_NSPASS)
    {
        place_column_doormagic_a(clm_recs[IDIR_WEST],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_EAST],surr_own[IDIR_CENTR]);
    } else
    {
        place_column_doormagic_a(clm_recs[IDIR_NORTH],surr_own[IDIR_CENTR]);
        place_column_doormagic_b(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
        place_column_doormagic_c(clm_recs[IDIR_SOUTH],surr_own[IDIR_CENTR]);
    }
}

/*
 * Function modifies a column made of earth,gold or similar material
 * for gaps when near unclaimed, short terrain. The corner
 * column are sometimes changed to the surrounding material.
 */
void modify_frail_columns(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
    const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
    const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
    /*This gives smaller probability of column change if there are no adjacent tall slabs */
    int base_prob=33;
    if (slab_is_tall_unclmabl(surr_slb[IDIR_WEST])||slab_is_tall_unclmabl(surr_slb[IDIR_NORTH])||
       slab_is_tall_unclmabl(surr_slb[IDIR_EAST])||slab_is_tall_unclmabl(surr_slb[IDIR_SOUTH]))
       base_prob=66;
    int i;
    /*Let's take all corners at once, in a loop */
    for (i=0;i<4;i++)
    {
      /* All the changes near short unclaimable slabs */
      if (slab_is_short_unclmabl(surr_slb[dir_a[i]]) &&
          slab_is_short_unclmabl(surr_slb[dir_b[i]]) && (frail_columns_near_short))
      {
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_PATH)&&(surr_slb[dir_b[i]]==SLAB_TYPE_PATH)&&
              slab_is_tall(surr_slb[dir_c[i]]))
          {
            /* Path on both sides, and tall diagonal slabs - in this case we shouldn't */
            /* modify the columns, as this may cause imps to stuck */
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_PATH)||(surr_slb[dir_b[i]]==SLAB_TYPE_PATH))
          {
            if (rnd(100)<base_prob)
              fill_column_path(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)&&(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
          {
            if (rnd(100)<base_prob+33)
              fill_column_water(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          } else
          if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)&&(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
          {
            if (rnd(100)<base_prob+33)
              fill_column_lava(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          }
      } else
      /* All the changes near tall unclaimable slabs */
      /* These are mods by Tomasz Lis - originally tall slabs do not affect others */
      if (slab_is_tall_unclmabl(surr_slb[dir_a[i]]) &&
          slab_is_tall_unclmabl(surr_slb[dir_b[i]]) && (frail_columns_near_tall))
      {
          /* Replacing part of gold/gems with dirt if it is near */
          if ((surr_slb[IDIR_CENTR]!=SLAB_TYPE_EARTH)&&(surr_slb[IDIR_CENTR]!=SLAB_TYPE_TORCHDIRT)&&
             ((surr_slb[dir_a[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_a[i]]==SLAB_TYPE_TORCHDIRT))&&
             ((surr_slb[dir_b[i]]==SLAB_TYPE_EARTH)||(surr_slb[dir_b[i]]==SLAB_TYPE_TORCHDIRT)))
          {
            if (rnd(100)<base_prob)
              fill_column_earth(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
          }
      }
    }
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Returns true if something was modified
 */
short modify_liquid_surrounding(struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,
        short liq_level,unsigned short water_cube,unsigned short lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  /*initial tests */
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  /*corner columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_c[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_c[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  /*And the edge columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube;
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube;
       result=true;
    }
  }
  return result;
}

/*
 * Function modifies non-central columns if there is water or lava near.
 * Advanced version - the change is different for every direction.
 * (water_cube and lava_cube should point to 9-element matrix)
 * Returns true if something was modified
 */
short modify_liquid_surrounding_advncd(struct COLUMN_REC *clm_recs[9],
        const unsigned char *surr_slb,const unsigned char *surr_own,
        const short liq_level,const unsigned short *water_cube,const unsigned short *lava_cube)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*initial tests */
  if ((liq_level<0)||(liq_level>7)) return false;
  if (surr_slb==NULL) return false;
  short result=false;
  int i;
  /*corner columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER)||(surr_slb[dir_b[i]]==SLAB_TYPE_WATER))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=water_cube[dir_c[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA)||(surr_slb[dir_b[i]]==SLAB_TYPE_LAVA))
    {
        if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_b[i]];
        else
        if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_a[i]];
        else
         clm_recs[dir_c[i]]->c[liq_level]=lava_cube[dir_c[i]];
       result=true;
    }
  }
  /*And the edge columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_WATER))
    {
       clm_recs[dir_a[i]]->c[liq_level]=water_cube[dir_a[i]];
       result=true;
    } else
    if ((surr_slb[dir_a[i]]==SLAB_TYPE_LAVA))
    {
       clm_recs[dir_a[i]]->c[liq_level]=lava_cube[dir_a[i]];
       result=true;
    }
  }
  return result;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * The direction is made in a way that allows access column at room corner in IDIR_SE.
 */
unsigned short *get_room_corner_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned short slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
    if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
      return (unsigned short *)dir_rot_000;
    if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
      return (unsigned short *)dir_rot_090;
    if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
      return (unsigned short *)dir_rot_180;
    if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
      return (unsigned short *)dir_rot_270;
  return (unsigned short *)dir_rot_000;
}

/*
 * Returns one of the constant arrays for altering direction of room slabs.
 * It maps indices so that EAST is the "inside" of room.
 */
unsigned short *get_room_edge_direction_indices(unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned short slab=surr_slb[IDIR_CENTR];
    unsigned char ownr=surr_own[IDIR_CENTR];

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)))
    return (unsigned short *)dir_rot_000;
  if (((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)))
    return (unsigned short *)dir_rot_090;
  if (((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)))
    return (unsigned short *)dir_rot_180;
  if (((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
    return (unsigned short *)dir_rot_270;

  return (unsigned short *)dir_rot_000;
}

void create_columns_slb_room(cr_clm_func cr_floor,cr_clm_func cr_edge,
        cr_clm_func cr_corner,cr_clm_func cr_inside,cr_clm_func cr_nearinsd,
        struct COLUMN_REC *clm_recs[9], unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Checking if completely surrounded */
  if ((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr))
  {
      if  ((surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
           (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
           (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
           (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr))
      {
          cr_inside(clm_recs,surr_slb,surr_own,surr_tng);
      } else
      {
          /*The 'near inside' columns are usually same that floor, */
          /*but may differ for rooms with specific corners */
          cr_nearinsd(clm_recs,surr_slb,surr_own,surr_tng);
      }
      return;
  }
  /*If not completely, maybe we're surrounded from 3 sides (5 with corners)? */
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr)) ||
      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr)) ||
      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr)) ||
      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr)))
  {
      cr_edge(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  /*If still nothing, maybe we have same surround from two sides and 1 corner, */
  /* and another two are surely something else */
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
/*       (surr_slb[IDIR_NE]==slab)&&(surr_own[IDIR_NE]==ownr) && */
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
/*       (surr_slb[IDIR_SE]==slab)&&(surr_own[IDIR_SE]==ownr) && */
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
/*       (surr_slb[IDIR_SW]==slab)&&(surr_own[IDIR_SW]==ownr) && */
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))) ||

      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
/*       (surr_slb[IDIR_NW]==slab)&&(surr_own[IDIR_NW]==ownr) && */
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      cr_corner(clm_recs,surr_slb,surr_own,surr_tng);
      return;
  }
  /*If nothing found - just draw floor of this room */
  cr_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,__attribute__((unused)) unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_rec_sim(clm_recs[k*3+i], 0, 0x0f8,
           0x0ae +rnd(2), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0bb, 0x0ba);
}

void create_columns_slb_library_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  /*Library egde is just its floor */
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_library_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_library_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_library_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_library_floor(clm_recs,surr_slb,surr_own,surr_tng);
  place_column_library_bookcase_a(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_b(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  place_column_library_bookcase_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb, unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
      fill_column_dungheart_floor(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_dungheart_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_dungheart_inside(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_NORTH]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
    place_column_univ_stair(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_dungheart_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_dungheart_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SOUTH]], surr_own[IDIR_CENTR]);
    place_column_dungheart_raise(clm_recs[dir[IDIR_SW]], surr_own[IDIR_CENTR]);
    fill_column_rec_sim(clm_recs[dir[IDIR_CENTR]], 0, CUBE_PATH_SMOOTH3,
         0x080, 0x180, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_dungheart_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb, unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_dungheart_inside(clm_recs[k*3+i], surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_dungheart_nearinsd(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  create_columns_slb_dungheart_floor(clm_recs,surr_slb,surr_own,surr_tng);
  /*Raise the center a little bit */
  place_column_univ_stair(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  int i;
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*corner columns */
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)) ||
        ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)))
    {
      short i_op=(i+2)%4;
      if (((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)) ||
          ((surr_slb[dir_b[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr)))
        place_column_dungheart_raise(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    }
  }
  /*and the rest of columns */
  for (i=0;i<4;i++)
  {
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
      short i_op=(i+2)%4;
      if ((surr_slb[dir_a[i_op]]!=slab)||(surr_own[dir_a[i_op]]!=ownr))
        place_column_dungheart_raise(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
      else
        fill_column_dungheart_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    }
  }
}

void create_columns_slb_portal_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Center */
  fill_column_portal_floor(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  /*And the edge columns */
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_portal_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_portal_floor(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x13b, 0x15f);
}

void create_columns_slb_portal_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_edge_direction_indices(surr_slb,surr_own);
    fill_column_portal_step(clm_recs[dir[IDIR_NE]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_EAST]], surr_own[IDIR_CENTR]);
    fill_column_portal_step(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_portal_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_portal_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_portal_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i;
  for (i=0;i<9;i++)
    fill_column_rec_sim(clm_recs[i], 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+i, 0);
  fill_column_portal_inside_cntr(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_temple_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x075, 0x076);
}

void create_columns_slb_temple_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  const unsigned short dir_d[]={IDIR_SOUTH, IDIR_WEST, IDIR_NORTH, IDIR_EAST};
  const unsigned short dir_e[]={IDIR_EAST, IDIR_SOUTH, IDIR_WEST, IDIR_NORTH};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Temple edge is like its floor, */
  /*but it is higher by one cube from inside. */
  create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        (((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr)) ||
         ((surr_slb[dir_e[i]]==slab)&&(surr_own[dir_e[i]]==ownr))))
      place_column_temple_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  /*And the edge columns */
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr) &&
         (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr)) &&
        ((surr_slb[dir_d[i]]==slab)&&(surr_own[dir_d[i]]==ownr)))
      place_column_temple_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
}

void create_columns_slb_temple_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_temple_floor(clm_recs,surr_slb,surr_own, surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    place_column_temple_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
    place_column_temple_corner(clm_recs[dir[IDIR_CENTR]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_temple_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb, __attribute__((unused)) unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_rec_sim(clm_recs[i], 0, 0x177,
          CUBE_PATH_SMOOTH1 +rnd(5), 0x126 +i, 0x0, 0x0, 0x0, 0, 0, 0);
}

void create_columns_slb_hatchery_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb, unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Center */
  fill_column_hatchery_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)) ||
        ((surr_slb[dir_b[i]]!=slab)||(surr_own[dir_b[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);
  }
  /*And the edge columns */
  for (i=0;i<4;i++)
  {
    if (((surr_slb[dir_a[i]]!=slab)||(surr_own[dir_a[i]]!=ownr)))
      fill_column_hatchery_edge(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
    else
      fill_column_hatchery_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);
  }
  /*Liquid surrounding - lava surround is not as trivial as usually */
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  for (i=0;i<9;i++)
    water_cube[i]=0x159;
  lava_cube[IDIR_NW]=0x15e;
  lava_cube[IDIR_NE]=0x15d;
  lava_cube[IDIR_SW]=0x15c;
  lava_cube[IDIR_SE]=0x15e;
  lava_cube[IDIR_NORTH]=0x15d;
  lava_cube[IDIR_EAST]=0x15d;
  lava_cube[IDIR_SOUTH]=0x15c;
  lava_cube[IDIR_WEST]=rnd(2)?0x15c:0x13c;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_hatchery_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_hatchery_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_hatchery_floor(clm_recs,surr_slb,surr_own,surr_tng);
    unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
    fill_column_hatchery_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_hatchery_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_hatchery_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_lair_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Center */
  fill_column_lair_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*Surrounded by our area */
       clm_recs[dir_c[i]]->c[0]=CUBE_LAIR_INSIDE;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { /*our on side A only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*West only, not north */
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_NE:/*North only, not east */
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        case IDIR_SW:/*South only, not west */
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SE:/*East only, not south */
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*our on side B only */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x021;
           break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x024;
           break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x01f;
           break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x022;
           break;
        }
    } else
    { /*Surrounded by not our area */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x01e;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x020;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x023;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x025;
          break;
        }
    }
  }
  /*And the middle columns */
  for (i=0;i<4;i++)
  {
     fill_column_lair_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=CUBE_LAIR_INSIDE;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x024;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x021;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x01f;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x022;
           break;
        }
    }
  }
  /* Liquid surrounding is not supported for the lair ground, */
  /* so we've done */
}

void create_columns_slb_lair_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_lair_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_lair_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_lair_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_graveyard_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Center */
  fill_column_graveyard_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*Surrounded by our area */
       clm_recs[dir_c[i]]->c[0]=0x142;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { /*our on side A only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*West only, not north */
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_NE:/*North only, not east */
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        case IDIR_SW:/*South only, not west */
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SE:/*East only, not south */
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*our on side B only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*not west */
           clm_recs[dir_c[i]]->c[0]=0x146;
           break;
        case IDIR_SW:/*not south */
           clm_recs[dir_c[i]]->c[0]=0x149;
           break;
        case IDIR_NE:/*not north */
           clm_recs[dir_c[i]]->c[0]=0x144;
           break;
        case IDIR_SE:/*not east */
           clm_recs[dir_c[i]]->c[0]=0x147;
           break;
        }
    } else
    { /*Surrounded by not our area */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x143;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x145;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x148;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x14a;
          break;
        }
    }
  }
  /*And the middle columns */
  for (i=0;i<4;i++)
  {
     fill_column_graveyard_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x142;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x149;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x146;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x144;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x147;
           break;
        }
    }
  }
  /* Liquid surrounding is not supported for the graveyard ground, */
  /* so we've done */
}

void create_columns_slb_graveyard_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_graveyard_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_graveyard_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_graveyard_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_floor(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_barracks_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_barracks_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_barracks_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_barracks_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
    {
      fill_column_barracks_floor(clm_recs[i], surr_own[IDIR_CENTR]);
      place_column_barracks_raise(clm_recs[i], surr_own[IDIR_CENTR]);
    }
}

void create_columns_slb_training_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_training_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0fe, 0x0fd);
}

void create_columns_slb_training_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_training_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_training_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_training_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_training_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_treasure_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x0a4, 0x0a3);
}

void create_columns_slb_treasure_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_treasure_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_treasure_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_treasure_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_treasure_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_workshop_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x109, 0x108);
}

void create_columns_slb_workshop_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_workshop_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_workshop_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_workshop_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_workshop_floor(clm_recs,surr_slb,surr_own,surr_tng);
  /*On central slabs, we can use lava and water cubes too - they have same top. */
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     clm_recs[k*3+i]->c[0]=0x107 +rnd(3);
   }
}

void create_columns_slb_scavenger_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_scavenger_floor(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
  fill_column_scavenger_inside_cntr(clm_recs[IDIR_CENTR],surr_own[IDIR_CENTR]);
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x11b, 0x11a);
}

void create_columns_slb_scavenger_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_scavenger_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
  unsigned short *dir=get_room_corner_direction_indices(surr_slb,surr_own);
  fill_column_scavenger_pillar(clm_recs[dir[IDIR_SE]], surr_own[IDIR_CENTR]);
}

void create_columns_slb_scavenger_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_scavenger_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_NORTH, IDIR_EAST, IDIR_SOUTH, IDIR_WEST};
  const unsigned short dir_c[]={IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
  unsigned short slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  /*Center */
  fill_column_prison_inside(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
  /*Corners */
  int i;
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_c[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr)&&
        (surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*Surrounded by our area */
       clm_recs[dir_c[i]]->c[0]=0x0c8;
    } else
    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    { /*our on side A only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*West only, not north */
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_NE:/*North only, not east */
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        case IDIR_SW:/*South only, not west */
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SE:/*East only, not south */
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        }
    } else
    if ((surr_slb[dir_b[i]]==slab)&&(surr_own[dir_b[i]]==ownr))
    { /*our on side B only */
        switch (dir_c[i])
        {
        case IDIR_NW:/*not west */
           clm_recs[dir_c[i]]->c[0]=0x0cc;
           break;
        case IDIR_SW:/*not south */
           clm_recs[dir_c[i]]->c[0]=0x0cf;
           break;
        case IDIR_NE:/*not north */
           clm_recs[dir_c[i]]->c[0]=0x0ca;
           break;
        case IDIR_SE:/*not east */
           clm_recs[dir_c[i]]->c[0]=0x0cd;
           break;
        }
    } else
    { /*Surrounded by not our area */
        switch (dir_c[i])
        {
        case IDIR_NW:
           clm_recs[dir_c[i]]->c[0]=0x0c9;
          break;
        case IDIR_NE:
           clm_recs[dir_c[i]]->c[0]=0x0cb;
          break;
        case IDIR_SW:
           clm_recs[dir_c[i]]->c[0]=0x0ce;
          break;
        case IDIR_SE:
           clm_recs[dir_c[i]]->c[0]=0x0d0;
          break;
        }
    }
  }
  /*And the middle columns */
  for (i=0;i<4;i++)
  {
     fill_column_prison_inside(clm_recs[dir_a[i]], surr_own[IDIR_CENTR]);

    if ((surr_slb[dir_a[i]]==slab)&&(surr_own[dir_a[i]]==ownr))
    {
       clm_recs[dir_a[i]]->c[0]=0x0c8;
    } else
    {
        switch (dir_a[i])
        {
        case IDIR_SOUTH:
           clm_recs[dir_a[i]]->c[0]=0x0cf;
           break;
        case IDIR_WEST:
           clm_recs[dir_a[i]]->c[0]=0x0cc;
           break;
        case IDIR_NORTH:
           clm_recs[dir_a[i]]->c[0]=0x0ca;
           break;
        case IDIR_EAST:
           clm_recs[dir_a[i]]->c[0]=0x0cd;
           break;
        }
    }
  }
  /* Liquid surrounding - quite hard here */
  unsigned short *water_cube=malloc(9*sizeof(unsigned short));
  unsigned short *lava_cube  =malloc(9*sizeof(unsigned short));
  water_cube[IDIR_NW]=0x0d9;
  water_cube[IDIR_NE]=0x0db;
  water_cube[IDIR_SW]=0x0de;
  water_cube[IDIR_SE]=0x0e0;
  water_cube[IDIR_NORTH]=0x0da;
  water_cube[IDIR_EAST]= 0x0dd;
  water_cube[IDIR_SOUTH]=0x0df;
  water_cube[IDIR_WEST]= 0x0dc;
  lava_cube[IDIR_NW]=0x0d1;
  lava_cube[IDIR_NE]=0x0d3;
  lava_cube[IDIR_SW]=0x0d6;
  lava_cube[IDIR_SE]=0x0d8;
  lava_cube[IDIR_NORTH]=0x0d2;
  lava_cube[IDIR_EAST]= 0x0d5;
  lava_cube[IDIR_SOUTH]=0x0d7;
  lava_cube[IDIR_WEST]= 0x0d4;
  modify_liquid_surrounding_advncd(clm_recs,surr_slb,surr_own, 0,water_cube,lava_cube);
  free(water_cube);
  free(lava_cube);
}

void create_columns_slb_prison_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_prison_inside(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_prison_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_torture_floor(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_floor(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=CUBE_FLOOR_TORTCIRC;
}

void create_columns_slb_torture_edge(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_corner(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_torture_floor(clm_recs,surr_slb,surr_own,surr_tng);
}

void create_columns_slb_torture_inside(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
    create_columns_slb_prison_inside(clm_recs,surr_slb,surr_own,surr_tng);
    clm_recs[IDIR_CENTR]->c[0]=CUBE_FLOOR_TORTCIRC;
}

void create_columns_slb_guardpost(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    fill_column_guardpost_floor_c(clm_recs[IDIR_NORTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_NE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_EAST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_SE], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_SOUTH], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_a(clm_recs[IDIR_SW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_WEST], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_b(clm_recs[IDIR_NW], surr_own[IDIR_CENTR]);
    fill_column_guardpost_floor_c(clm_recs[IDIR_CENTR], surr_own[IDIR_CENTR]);
}

void create_columns_slb_bridge(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
    int i;
    for (i=0;i<9;i++)
      fill_column_bridge_inside(clm_recs[i], surr_own[IDIR_CENTR]);
}

void create_columns_slb_rock_gndlev(struct COLUMN_REC *clm_recs[9],
        __attribute__((unused)) unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_rock_gndlev(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
   }
}

void create_columns_slb_rockcaped_pathcave(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, __attribute__((unused)) unsigned char **surr_tng)
{
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     fill_column_path(clm_recs[k*3+i],surr_own[IDIR_CENTR]);
     clm_recs[k*3+i]->c[4]=CUBE_ROCK1;
     clm_recs[k*3+i]->solid=compute_clm_rec_solid(clm_recs[k*3+i]);
     clm_recs[k*3+i]->height=compute_clm_rec_height(clm_recs[k*3+i]);
   }
  modify_liquid_surrounding(clm_recs, surr_slb, 0, 0x02e, 0x02f);
}

void create_columns_slb_rockcaped_claimcave(struct COLUMN_REC *clm_recs[9],
        unsigned char *surr_slb,unsigned char *surr_own, unsigned char **surr_tng)
{
  create_columns_slb_claimed(clm_recs,surr_slb,surr_own,surr_tng);
  int i,k;
  for (i=0;i<3;i++)
   for (k=0;k<3;k++)
   {
     clm_recs[k*3+i]->c[4]=CUBE_ROCK1;
     clm_recs[k*3+i]->solid=compute_clm_rec_solid(clm_recs[k*3+i]);
     clm_recs[k*3+i]->height=compute_clm_rec_height(clm_recs[k*3+i]);
   }
}
