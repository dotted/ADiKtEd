/******************************************************************************/
/** @file obj_slabs.h
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Functions for maintaining slabs. Also includes some routines for owner 
 *     identification and WBI files.
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

#include "obj_slabs.h"

#include <math.h>
#include "lev_data.h"
#include "globals.h"
#include "obj_things.h"
#include "bulcommn.h"
#include "arr_utils.h"

char const SLB_UNKN_LTEXT[]="Unknown slab";
char const SLB_ROCK_LTEXT[]="Rock";
char const SLB_GOLD_LTEXT[]="Gold";
char const SLB_EARTH_LTEXT[]="Earth";
char const SLB_TORCHDIRT_LTEXT[]="Torch plate earth";
char const SLB_WALLDRAPE_LTEXT[]="Wall with drape";
char const SLB_WALLTORCH_LTEXT[]="Wall with torch";
char const SLB_WALLWTWINS_LTEXT[]="Wall with twins";
char const SLB_WALLWWOMAN_LTEXT[]="Wall with woman";
char const SLB_WALLPAIRSHR_LTEXT[]="Wall w/pair shag.";
char const SLB_PATH_LTEXT[]="Path";
char const SLB_CLAIMED_LTEXT[]="Claimed ground";
char const SLB_LAVA_LTEXT[]="Lava";
char const SLB_WATER_LTEXT[]="Water";
char const SLB_PORTAL_LTEXT[]="Entrance";
char const SLB_TREASURE_LTEXT[]="Treasure room";
char const SLB_LIBRARY_LTEXT[]="Library";
char const SLB_PRISON_LTEXT[]="Prison";
char const SLB_TORTURE_LTEXT[]="Torture chamber";
char const SLB_TRAINING_LTEXT[]="Training room";
char const SLB_DUNGHEART_LTEXT[]="Dungeon heart";
char const SLB_WORKSHOP_LTEXT[]="Workshop";
char const SLB_SCAVENGER_LTEXT[]="Scavenger room";
char const SLB_TEMPLE_LTEXT[]="Temple";
char const SLB_GRAVEYARD_LTEXT[]="Graveyard";
char const SLB_HATCHERY_LTEXT[]="Hatchery";
char const SLB_LAIR_LTEXT[]="Lair";
char const SLB_BARRACKS_LTEXT[]="Barracks";
char const SLB_DOORWOOD_LTEXT[]="Wooden door";
char const SLB_DOORBRACE_LTEXT[]="Braced door";
char const SLB_DOORIRON_LTEXT[]="Iron door";
char const SLB_DOORMAGIC_LTEXT[]="Magic door";
char const SLB_BRIDGE_LTEXT[]="Bridge";
char const SLB_GEMS_LTEXT[]="Gems";
char const SLB_GUARDPOST_LTEXT[]="Guard post";
char const SLB_PURPLEPATH_LTEXT[]="Purple path";

const char *owner_fullnames[]={"Keeper 0 (Human)", "Keeper 1 (blue)", "Keeper 2 (green)",
    "Keeper 3 (yellow)", "Heroes (Player4)", "Unclaimed/Unowned"};
const char *owner_colornames[]={"Red", "Blue", "Green",
    "Yellow", "White", "Unowned"};

const unsigned char owners_list[]={
    PLAYER0, PLAYER1, PLAYER2, PLAYER3,
    PLAYER_GOOD, PLAYER_UNSET
    };

const unsigned short slabs_space[]={
    SLAB_TYPE_PATH, SLAB_TYPE_CLAIMED, SLAB_TYPE_LAVA,
    SLAB_TYPE_WATER,
};

const unsigned short slabs_liquid[]={
    SLAB_TYPE_LAVA, SLAB_TYPE_WATER,
};

const unsigned short slabs_short_unclmabl[]={
    SLAB_TYPE_PATH, SLAB_TYPE_LAVA, SLAB_TYPE_WATER,
};

const unsigned short slabs_wealth[]={
    SLAB_TYPE_GOLD, SLAB_TYPE_GEMS,
};

const unsigned short slabs_tall_unclmabl[]={
    SLAB_TYPE_ROCK, SLAB_TYPE_GOLD, SLAB_TYPE_EARTH,
    SLAB_TYPE_TORCHDIRT, SLAB_TYPE_GEMS
};

const unsigned short slabs_walls[]={
    SLAB_TYPE_WALLDRAPE, SLAB_TYPE_WALLTORCH, SLAB_TYPE_WALLWTWINS,
    SLAB_TYPE_WALLWWOMAN, SLAB_TYPE_WALLPAIRSHR,
};

const unsigned short slabs_rooms[]={
    SLAB_TYPE_DUNGHEART,SLAB_TYPE_PORTAL, SLAB_TYPE_TREASURE,
    SLAB_TYPE_LIBRARY, SLAB_TYPE_PRISONCASE, SLAB_TYPE_TORTURE,
    SLAB_TYPE_TRAINING, SLAB_TYPE_WORKSHOP, SLAB_TYPE_SCAVENGER,
    SLAB_TYPE_TEMPLE, SLAB_TYPE_GRAVEYARD, SLAB_TYPE_HATCHERY,
    SLAB_TYPE_LAIR, SLAB_TYPE_BARRACKS, SLAB_TYPE_BRIDGE,
    SLAB_TYPE_GUARDPOST,
};

const unsigned short slabs_doors[]={
    SLAB_TYPE_DOORWOOD1, SLAB_TYPE_DOORWOOD2,
    SLAB_TYPE_DOORBRACE1, SLAB_TYPE_DOORBRACE2,
    SLAB_TYPE_DOORIRON1, SLAB_TYPE_DOORIRON2,
    SLAB_TYPE_DOORMAGIC1, SLAB_TYPE_DOORMAGIC2,
};

const char *all_slabs_fullnames[]={

     SLB_ROCK_LTEXT,SLB_GOLD_LTEXT,              /*00 */
     SLB_EARTH_LTEXT,SLB_TORCHDIRT_LTEXT,
     SLB_WALLDRAPE_LTEXT,SLB_WALLTORCH_LTEXT,
     SLB_WALLWTWINS_LTEXT,SLB_WALLWWOMAN_LTEXT,
     SLB_WALLPAIRSHR_LTEXT,SLB_UNKN_LTEXT,        /*08 */
     SLB_PATH_LTEXT,SLB_CLAIMED_LTEXT,           /*0A */
     SLB_LAVA_LTEXT,SLB_WATER_LTEXT,
     SLB_PORTAL_LTEXT,SLB_UNKN_LTEXT,
     SLB_TREASURE_LTEXT,SLB_UNKN_LTEXT,
     SLB_LIBRARY_LTEXT,SLB_UNKN_LTEXT,
     SLB_PRISON_LTEXT,SLB_UNKN_LTEXT,
     SLB_TORTURE_LTEXT,SLB_UNKN_LTEXT,
     SLB_TRAINING_LTEXT,SLB_UNKN_LTEXT,
     SLB_DUNGHEART_LTEXT,SLB_UNKN_LTEXT,
     SLB_WORKSHOP_LTEXT,SLB_UNKN_LTEXT,
     SLB_SCAVENGER_LTEXT,SLB_UNKN_LTEXT,
     SLB_TEMPLE_LTEXT,SLB_UNKN_LTEXT,
     SLB_GRAVEYARD_LTEXT,SLB_UNKN_LTEXT,
     SLB_HATCHERY_LTEXT,SLB_UNKN_LTEXT,
     SLB_LAIR_LTEXT,SLB_UNKN_LTEXT,
     SLB_BARRACKS_LTEXT,SLB_UNKN_LTEXT,        /*28 */
     SLB_DOORWOOD_LTEXT,SLB_DOORWOOD_LTEXT,
     SLB_DOORBRACE_LTEXT,SLB_DOORBRACE_LTEXT,
     SLB_DOORIRON_LTEXT,SLB_DOORIRON_LTEXT,
     SLB_DOORMAGIC_LTEXT,SLB_DOORMAGIC_LTEXT, /*2f */
     SLB_UNKN_LTEXT,SLB_BRIDGE_LTEXT,
     SLB_GEMS_LTEXT,SLB_GUARDPOST_LTEXT,
     SLB_PURPLEPATH_LTEXT,
     SLB_UNKN_LTEXT,
     };

unsigned char get_owner_next(unsigned char plyr_idx)
{
  plyr_idx++;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return 0;
}

unsigned char get_owner_prev(unsigned char plyr_idx)
{
  plyr_idx--;
  if (plyr_idx<PLAYERS_COUNT) return plyr_idx;
  return PLAYERS_COUNT-1;
}

/*
 * Sets slabs which can't be claimed as unowned (PLAYER_UNSET).
 */
void update_slab_owners(struct LEVEL *lvl)
{
    /*Resetting owners on tiles */
    int tx, ty;    
    for (ty=0; ty<lvl->tlsize.y; ty++)
      for (tx=0; tx<lvl->tlsize.x; tx++)
      {
        unsigned short slb=get_tile_slab(lvl, tx, ty);
        if (!slab_is_clmabl(slb))
          set_tile_owner(lvl,tx,ty,PLAYER_UNSET);
      }
    /*Resetting the last column */
    int sx, sy;
    sx=lvl->subsize.x-1;
    for (sy=0; sy<lvl->subsize.y; sy++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
    sy=lvl->subsize.y-1;
    for (sx=0; sx<lvl->subsize.x; sx++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
}

/*
 * Places room of given size and player at random position.
 * Tries to select position far from other players.
 * Used mainly for putting Dungeon Hearts.
 */
short place_room_rndpos(struct LEVEL *lvl, __attribute__((unused)) const unsigned short rslab,
    __attribute__((unused)) const unsigned char rown, __attribute__((unused)) const struct IPOINT_2D *rsize)
{
    int surrnd_size=20;
    /*struct IPOINT_2D rpos;*/
    while (surrnd_size>4)
    {
      int lottery_count;
      for (lottery_count=0;lottery_count<16;lottery_count++)
      {
        /*rpos.x=(surrnd_size>>1)+1+*/rnd(lvl->tlsize.x-surrnd_size);
        /*rpos.y=(surrnd_size>>1)+1+*/rnd(lvl->tlsize.y-surrnd_size);
/*TODO!!!!!!!!!!! */
/*
4 Check if there's no enemy/short slab/rock in range of surrnd_size
5 Check if there IS short slab/rock in range of surrnd_size+5
6 if conditions met - goto 20

20 get random position within surrnd_size rectangle (whole heart must fit inside surrnd_size)
21 put the heart
*/
      }
      surrnd_size--;
    }
    /* If we're here, this means we couldn't find place for heart. */
    return false;
}

/*
 * Returns tile owner name string
 */
char *get_owner_type_fullname(unsigned char own_idx)
{
     int types_count=sizeof(owner_fullnames)/sizeof(char *);
     if (own_idx<types_count)
       return (char *)owner_fullnames[own_idx];
     else
       return "unknown(?!)";
}

/*
 * Returns tile owner name string
 */
char *get_owner_type_colorname(unsigned char own_idx)
{
     int types_count=sizeof(owner_colornames)/sizeof(char *);
     if (own_idx<types_count)
       return (char *)owner_colornames[own_idx];
     else
       return "unknown(?!)";
}

/*
 * Says whether a slab is surrounded by the same things
 */
short slab_is_central(struct LEVEL *lvl,int tx,int ty)
{
    /* Can't be central if it is on map edge */
    if ((tx<=0) || (ty<=0) || (tx+1>=lvl->tlsize.x) || (ty+1>=lvl->tlsize.y))
      return false;
    /*Preparing variables */
    int i, j;
    unsigned short slab=get_tile_slab(lvl,tx,ty);
    unsigned char owner=get_tile_owner(lvl,tx,ty);
    /*Sweeping and comparing slab type and owner */
    for (i=tx-1; i<=tx+1; i++)
      for (j=ty-1; j<=ty+1; j++)
        if ((slab!=get_tile_slab(lvl,i,j))||(owner!=get_tile_owner(lvl,i,j)))
          return false;
    return true;
}

/*
 * Verifies slab types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short slabs_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
  /*Preparing array bounds */
  int max_tl_idx_x=lvl->tlsize.x-1;
  int max_tl_idx_y=lvl->tlsize.y-1;
  short result;
  int i,j;
  for (i=1; i < max_tl_idx_y; i++)
    for (j=1; j < max_tl_idx_x; j++)
    {
      result=slab_verify_entry(get_tile_slab(lvl,i,j),err_msg);
      if (result!=VERIF_OK)
      {
        errpt->x=i;
        errpt->y=j;
        return result;
      }
    }
  return VERIF_OK;
}


/*
 * Says how many slabs of given type are in 3x3 area surrounding given slab,
 * and including it; returns 0..9;
 */
int slab_siblings_oftype(struct LEVEL *lvl,int x,int y,unsigned short slab_type)
{
    int amount=0;
    /*Preparing variables */
    int i, j;
    /*Sweeping and comparing slab type and owner */
    for (i=x-1; i<=x+1; i++)
      for (j=y-1; j<=y+1; j++)
      {
      if ((i>=0) && (j>=0) && (i<lvl->tlsize.x) && (j<lvl->tlsize.y))
        if (get_tile_slab(lvl,i,j)==slab_type)
          amount++;
      }
    return amount;
}

short slab_is_room(unsigned short slab_type)
{
     int array_count=sizeof(slabs_rooms)/sizeof(unsigned short);
    /*All rooms are listed in slabs_rooms array */
    int idx=arr_ushort_pos(slabs_rooms,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_door(unsigned short slab_type)
{
     int array_count=sizeof(slabs_doors)/sizeof(unsigned short);
    /*All doors are listed in slabs_doors array */
    int idx=arr_ushort_pos(slabs_doors,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_wall(unsigned short slab_type)
{
     int array_count=sizeof(slabs_walls)/sizeof(unsigned short);
    /*All walls are listed in slabs_walls array */
    int idx=arr_ushort_pos(slabs_walls,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

unsigned short get_random_wall_slab(void)
{
     int array_count=sizeof(slabs_walls)/sizeof(unsigned short);
    /*All walls are listed in slabs_walls array */
    int idx=rnd(array_count);
    return slabs_walls[idx];
}

short slab_is_wealth(unsigned short slab_type)
{
     int array_count=sizeof(slabs_wealth)/sizeof(unsigned short);
    int idx=arr_ushort_pos(slabs_wealth,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_space(unsigned short slab_type)
{
     int array_count=sizeof(slabs_space)/sizeof(unsigned short);
    /*All such things are listed in slabs_space array */
    int idx=arr_ushort_pos(slabs_space,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_liquid(unsigned short slab_type)
{
     int array_count=sizeof(slabs_liquid)/sizeof(unsigned short);
    /*All such things are listed in slabs_liquid array */
    int idx=arr_ushort_pos(slabs_liquid,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_tall_unclmabl(unsigned short slab_type)
{
     int array_count=sizeof(slabs_tall_unclmabl)/sizeof(unsigned short);
    /*All such things are listed in slabs_tall_unclmabl array */
    int idx=arr_ushort_pos(slabs_tall_unclmabl,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_short_unclmabl(unsigned short slab_type)
{
     int array_count=sizeof(slabs_short_unclmabl)/sizeof(unsigned short);
    /*All such things are listed in slabs_x array */
    int idx=arr_ushort_pos(slabs_short_unclmabl,slab_type,array_count);
    if (idx>=0) return true;
    return false;
}

short slab_is_short(unsigned short slab_type)
{
    /*Rooms are short */
    if (slab_is_room(slab_type)) return true;
    /*But there are also other short things */
    if (slab_is_space(slab_type)) return true;
    return false;
}

short slab_is_tall(unsigned short slab_type)
{
    if (slab_is_tall_unclmabl(slab_type)) return true;
    if (slab_is_wall(slab_type)) return true;
    return false;
}

short slab_is_short_clmabl(unsigned short slab_type)
{
    /*Rooms are short and usually claimed */
    if (slab_is_room(slab_type)) return true;
    /*But there are also other claimed things */
    if (slab_is_door(slab_type)) return true;
    if (slab_type==SLAB_TYPE_CLAIMED) return true;
    return false;
}

short slab_is_claimedgnd(unsigned short slab_type)
{
    if (slab_is_door(slab_type)) return true;
    if (slab_type==SLAB_TYPE_CLAIMED) return true;
    return false;
}

short slab_is_clmabl(unsigned short slab_type)
{
    if (slab_is_short_unclmabl(slab_type)) return false;
    if (slab_is_tall_unclmabl(slab_type)) return false;
    return true;
}

/*
 * Returns if the slab can have torch on.
 */
short slab_allows_torch(unsigned short slab_type)
{
    if (!slab_is_short(slab_type)) return false;
    if (slab_is_room(slab_type)) return false;
    if (slab_is_liquid(slab_type)) return false;
    return true;
}

/*
 * Returns if the slab should have torch near it.
 */
short slab_needs_adjacent_torch(unsigned short slab_type)
{
    if (slab_type==SLAB_TYPE_TORCHDIRT) return true;
    if (slab_type==SLAB_TYPE_WALLTORCH) return true;
    return false;
}

/*
 * Draws a smear of selected slab on the map
 */
void slab_draw_smear(struct LEVEL *lvl,int startx,int starty,int startr,
                     int endx,int endy,int endr,int bend,unsigned short slab_type)
{
    int dist_x=(startx-endx);
    int dist_y=(starty-endy);
    int distance=floor(sqrt(pow(dist_x,2)+pow(dist_y,2))+0.5f);
    if (distance<1) distance=1;
    int cx,cy,r;
    int i;
    for (i=0;i<distance;i++)
    {
        int ir=distance-i-1;
        float bend_val=2.0f*sqrt(pow(distance/2,2)-pow(i-distance/2,2))*(float)bend/distance;
        int bend_x,bend_y;
        bend_x=floor((bend_val*(float)dist_y)/(float)distance+0.5f);
        bend_y=floor((bend_val*(float)dist_x)/(float)distance+0.5f);
        /*Linear change of point and radius */
        cx=(startx*ir+endx*i)/distance+bend_x;
        cy=(starty*ir+endy*i)/distance-bend_y;
        r= (startr*ir+endr*i)/distance;
        /*Drawing circle of given slab */
        slab_draw_circle(lvl,cx,cy,r,slab_type);
    }
}

void slab_draw_circle(struct LEVEL *lvl,int x,int y,int rad,unsigned short slab_type)
{
     int i,j;
     int cx,cy;
     for (i=-rad;i<rad;i++)
       for (j=-rad;j<rad;j++)
       {
         cx=x+i;
         cy=y+j;
         if ((cx>=0) && (cy>=0) && (cx<lvl->tlsize.x) && (cy<lvl->tlsize.y))
         {
           int crad=floor(sqrt(i*i+j*j)+0.5);
           if (crad<rad)
             set_tile_slab(lvl,cx,cy,slab_type);
         }
       }
}

/*
 * Verifies values in the SLB entry.
 */
short slab_verify_entry(unsigned short slab_type, char *err_msg)
{
    if (slab_is_door(slab_type)) return VERIF_OK;
    if (slab_is_room(slab_type)) return VERIF_OK;
    if (slab_is_wall(slab_type)) return VERIF_OK;
    if (slab_is_space(slab_type)) return VERIF_OK;
    if (slab_is_tall_unclmabl(slab_type)) return VERIF_OK;
    sprintf(err_msg,"Unknown slab entry %d",(int)slab_type);
    return VERIF_WARN;
}

/*
 * Returns slab type name string
 */
char *get_slab_fullname(unsigned short slb_type)
{
     int types_count=sizeof(all_slabs_fullnames)/sizeof(char *);
     if (slb_type<types_count)
       return (char *)all_slabs_fullnames[slb_type];
     else
       return "unknown(?!)";
}

short subtl_is_near_tall_slab(struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    int subtl_x=sx%MAP_SUBNUM_X;
    int subtl_y=sy%MAP_SUBNUM_Y;
    unsigned short slab;
    const int sub_val[]=  {0, 2};
    const int til_delta[]={-1,1};
    int i,k;
    /* Edges */
    for (i=0;i<2;i++)
    {
      if (subtl_x==sub_val[i])
      {
        slab=get_tile_slab(lvl,tx+til_delta[i],ty);
        if (slab_is_tall(slab)) return true;
      }
      if (subtl_y==sub_val[i])
      {
        slab=get_tile_slab(lvl,tx,ty+til_delta[i]);
        if (slab_is_tall(slab)) return true;
      }
    }
    /* Corners */
    for (k=0;k<2;k++)
      for (i=0;i<2;i++)
      {
        if ((subtl_x==sub_val[i])&&(subtl_y==sub_val[k]))
        {
          slab=get_tile_slab(lvl,tx+til_delta[i],ty+til_delta[k]);
          if (slab_is_tall(slab)) return true;
        }
      }
    return false;
}
