/*
 * lev_things.c
 *
 * Defines routines for maintaining list of TNG entries,
 * provides higher level TNG control than obj_things
 *
 */

#include "lev_things.h"

#include "globals.h"
#include "lev_data.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "lev_column.h"

/*
 * Verifies thing types and parameters. Returns VERIF_ERROR,
 * VERIF_WARN or VERIF_OK
 */
short things_verify(struct LEVEL *lvl, char *err_msg)
{
    char child_err_msg[LINEMSG_SIZE];
    strcpy(child_err_msg,"Unknown error");
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    //Sweeping through things
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          short result=thing_verify(thing,child_err_msg);
          if (result!=VERIF_OK)
          {
            sprintf(err_msg,"%s at slab %d,%d.",child_err_msg,i/MAP_SUBNUM_X,j/MAP_SUBNUM_Y);
            return result;
          }
        }
      }
    }
  return VERIF_OK;
}

/*
 * Sets lock state for the given door; creates/deletes spinning key thing.
 */
short set_door_lock(struct LEVEL *lvl, unsigned char *thing, unsigned char nlock)
{
    if (thing==NULL) return false;
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    unsigned char *thing_key=NULL;
    unsigned int tng_num=get_thing_subnums(lvl,sx,sy);
    while (tng_num>0)
    {
        tng_num--;
        thing_key=get_thing(lvl,sx,sy,tng_num);
        if ((get_thing_type(thing_key)==THING_TYPE_ITEM) && 
            (get_thing_subtype(thing_key)==ITEM_SUBTYPE_SPINNKEY))
          break;
        thing_key=NULL;
    }
    if (nlock==DOOR_PASS_UNLOCKED)
    {
        if (thing_key!=NULL)
          thing_del(lvl,sx,sy,tng_num);
    } else
    {
        if (thing_key==NULL)
        {
          thing_key = create_doorkey(lvl,sx,sy,ITEM_SUBTYPE_SPINNKEY);
          thing_add(lvl,thing_key);
        }
    }
    set_thing_level(thing,nlock);
    return true;
}

/*
 * Returns lock state for the given door.
 */
unsigned char get_door_lock(unsigned char *thing)
{
  return get_thing_level(thing);
}

/*
 * Returns the orientation that doors should have.
 */
unsigned char compute_door_orientation(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short tx=get_thing_tilepos_x(thing)/MAP_SUBNUM_X;
    unsigned short ty=get_thing_tilepos_y(thing)/MAP_SUBNUM_Y;
    // Check the previous orientation - if it matches, then it is preferred
    if (get_door_orientation(thing)==DOOR_ORIENT_NSPASS)
    {
      unsigned char slab_e=get_tile_slab(lvl, tx+1, ty);
      unsigned char slab_w=get_tile_slab(lvl, tx-1, ty);
      if (slab_is_tall(slab_e)&&slab_is_tall(slab_w))
        return DOOR_ORIENT_NSPASS;
      else
        return DOOR_ORIENT_EWPASS;
    } else
    {
      unsigned char slab_n=get_tile_slab(lvl, tx, ty-1);
      unsigned char slab_s=get_tile_slab(lvl, tx, ty+1);
      if (slab_is_tall(slab_n)&&slab_is_tall(slab_s))
        return DOOR_ORIENT_EWPASS;
      else
        return DOOR_ORIENT_NSPASS;
    }
}

/*
 * Sweeps things to find an unused hero gate number
 */
unsigned short get_free_herogate_number(struct LEVEL *lvl)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int k;
    int used_size=lvl->stats.hero_gates_count+16;
    unsigned char *used=malloc(used_size*sizeof(unsigned char));
    for (k=0;k<used_size;k++)
      used[k]=0;
    int cy, cx;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=lvl->tng_subnums[cx][cy];
          for (k=0; k<num_subs; k++)
          {
                char *thing=get_thing(lvl,cx,cy,k);
                unsigned char stype_idx=get_thing_subtype(thing);
                if (stype_idx==ITEM_SUBTYPE_HEROGATE)
                {
                  unsigned short cnum=-(char)get_thing_level(thing);
                  if (cnum<used_size) used[cnum]=1;
                }
          }
      }
    }
    // Note: numbering starts at 1, not 0
    int new_num=1;
    while (used[new_num]>0) new_num++;
    free(used);
    return new_num;
}

/*
 * Counts things belonging to each owner. count parameter must be
 * an array with size PLAYERS_COUNT
 */
short owned_things_count(int *count,struct LEVEL *lvl,
    unsigned char type_idx,unsigned char stype_idx)
{
    //Preparing array bounds
    int arr_entries_x=MAP_SIZE_X*MAP_SUBNUM_X;
    int arr_entries_y=MAP_SIZE_Y*MAP_SUBNUM_Y;
    int i,j,k;
    for (i=0; i < arr_entries_y; i++)
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          unsigned char type_id2=get_thing_type(thing);
          unsigned char stype_id2=get_thing_subtype(thing);
          if ((type_idx==type_id2)&&(stype_idx==stype_id2))
          {
            unsigned char own=get_thing_owner(thing);
            if (own>=PLAYERS_COUNT)
              count[PLAYER_UNSET]++;
            else
              count[own]++;
          }
        }
      }
    return true;
}

/*
 * Tries to find a thing that contains a light (lit thing)
 * on given slab and 1 square radius rectangle. If no such thing, returns NULL.
 */
unsigned char *find_lit_thing_on_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
        {
          unsigned char *thing;
          thing=find_thing_on_tile(lvl,is_lit_thing,i,k);
          if (thing!=NULL) return thing;
        }
      }
    return NULL;
}

/*
 * Tries to find a thing that contains a light (lit thing)
 * on given slab. If no such thing, returns NULL.
 */
unsigned char *find_thing_on_tile(struct LEVEL *lvl,is_thing_subtype check_func, int tx, int ty)
{
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
          for (i=get_thing_subnums(lvl,sx,sy)-1; i >=0; i--)
          {
            unsigned char *thing;
            thing=get_thing(lvl,sx,sy,i);
            if (check_func(thing))
              return thing;
          }
    return NULL;
}

/*
 * Returns an acceptable value of sensitive tile for torch
 */
unsigned short compute_torch_sensitile(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    int ntx=tx+(int)(sx%MAP_SUBNUM_X)-1;
    int nty=ty+(int)(sy%MAP_SUBNUM_Y)-1;
    if (ntx<0) ntx=0;
    if (ntx>=MAP_SIZE_X) ntx=MAP_MAXINDEX_X;
    if (nty<0) nty=0;
    if (nty>=MAP_SIZE_X) nty=MAP_MAXINDEX_X;
    if ((ntx==tx)&&(nty==ty))
        return ty*MAP_SIZE_X+tx;
    unsigned char slab;
    // Trying fo find torch wall around
    // First - try to put it in X axis
    slab=get_tile_slab(lvl,ntx,ty);
    if (slab_needs_adjacent_torch(slab))
        return ty*MAP_SIZE_X+ntx;
    //Now in Y axis
    slab=get_tile_slab(lvl,tx,nty);
    if (slab_needs_adjacent_torch(slab))
        return nty*MAP_SIZE_X+tx;
    //Now in XY corner
    slab=get_tile_slab(lvl,ntx,nty);
    if (slab_needs_adjacent_torch(slab))
        return nty*MAP_SIZE_X+ntx;
    // No torch wall - searching for any tall slab
    // First - try to put it in X axis
    slab=get_tile_slab(lvl,ntx,ty);
    if (slab_is_tall(slab))
        return ty*MAP_SIZE_X+ntx;
    //Now in Y axis
    slab=get_tile_slab(lvl,tx,nty);
    if (slab_is_tall(slab))
        return nty*MAP_SIZE_X+tx;
    //Now in XY corner
    slab=get_tile_slab(lvl,ntx,nty);
    if (slab_is_tall(slab))
        return nty*MAP_SIZE_X+ntx;
    return THING_SENSITILE_NONE;
}

/*
 * Returns an acceptable value of sensitive tile for room effect
 */
unsigned short compute_roomeffect_sensitile(struct LEVEL *lvl, unsigned char *thing)
{
    unsigned short sx=get_thing_tilepos_x(thing);
    unsigned short sy=get_thing_tilepos_y(thing);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    return ty*MAP_SIZE_X+tx;
}

/*
 * Creates a new thing of type door. Sets orientation and owner for given position.
 */
unsigned char *create_door(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_DOOR);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    //This will be updated in update_tile_things_subpos_and_height
    set_thing_subtpos(thing,128,128);
    set_thing_tilepos_h(thing,5);
    set_door_orientation(thing,compute_door_orientation(lvl,thing));
    //Set default lock state
    set_thing_level(thing,DOOR_PASS_UNLOCKED);
    return thing;
}

unsigned char *create_torch(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    //This will be updated in update_tile_things_subpos_and_height
    unsigned char sub_x=0x080;
    unsigned char sub_y=0x080;
    unsigned char subtl_h=2;
    unsigned char subpos_h=224;
    if ((sx%MAP_SUBNUM_X)==0)
      sub_x=0x040;
    else
    if ((sx%MAP_SUBNUM_X)==2)
      sub_x=0x0c0;
    else
    if ((sy%MAP_SUBNUM_Y)==0)
      sub_y=0x040;
    else
    if ((sy%MAP_SUBNUM_Y)==2)
      sub_y=0x0c0;
    if (((sx%MAP_SUBNUM_X)==1)&&((sy%MAP_SUBNUM_Y)==1))
    {
      subtl_h=3;
      subpos_h=0;
    }
    set_thing_subtpos(thing,sub_x,sub_y);
    set_thing_tilepos_h(thing,subtl_h);
    set_thing_subtpos_h(thing,subpos_h);
    //Sensitive tile
    unsigned short sensitile=compute_torch_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

unsigned char *create_doorkey(struct LEVEL *lvl, unsigned int sx, unsigned int sy,  unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    set_thing_type(thing,THING_TYPE_ITEM);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,tx,ty));
    //This will be updated in update_tile_things_subpos_and_height
    set_thing_tilepos_h(thing,4);
    set_thing_subtpos_h(thing,0x000);
    set_thing_subtpos(thing,0x080,0x080);
    //Note: in most DK maps, sensitile is set to 0, but I believe it is an error
    unsigned short sensitile=ty*MAP_SIZE_X+tx;
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type room effect
 */
unsigned char *create_roomeffect(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_ROOMEFFECT);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    unsigned short sensitile=compute_roomeffect_sensitile(lvl,thing);
    set_thing_sensitile(thing,sensitile);
    return thing;
}

/*
 * Creates a new thing of type creature
 */
unsigned char *create_creature(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_CREATURE);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    return thing;
}

/*
 * Creates a new thing of type trap
 */
unsigned char *create_trap(unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    thing = create_thing(sx,sy);
    set_thing_type(thing,THING_TYPE_TRAP);
    set_thing_subtype(thing,stype_idx);
    set_thing_owner(thing,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    return thing;
}

/*
 * Creates a new thing of type item. Advanced version - uses one of specialized
 * functions to create the item.
 */
unsigned char *create_item_adv(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char stype_idx)
{
    unsigned char *thing;
    int tx=sx/MAP_SUBNUM_X;
    int ty=sy/MAP_SUBNUM_Y;
    if (is_torch_stype(stype_idx))
    {
        thing=create_torch(lvl,sx,sy,stype_idx);
    } else
    if (stype_idx==ITEM_SUBTYPE_HEROGATE)
    {
        thing=create_item(sx,sy,stype_idx);
        set_thing_owner(thing,PLAYER_GOOD);
        //Hero gate must be numbered
        set_thing_level(thing,(char)(-get_free_herogate_number(lvl)));
    } else
    if (stype_idx==ITEM_SUBTYPE_DNHEART)
    {
        thing=create_item(sx,sy,stype_idx);
        set_thing_tilepos_h(thing,3); // Raise it up a bit
        set_thing_sensitile(thing,THING_SENSITILE_NONE);
    } else
    if (stype_idx==ITEM_SUBTYPE_PRISONBAR)
    {
        thing=create_item(sx,sy,stype_idx);
        set_thing_subtpos(thing,128,128);
        unsigned short sensitile=ty*MAP_SIZE_X+tx;
        set_thing_sensitile(thing,sensitile);
    } else
    if (stype_idx==ITEM_SUBTYPE_SPINNKEY)
    {
        thing=create_doorkey(lvl,sx,sy,stype_idx);
    } else
    {
        thing=create_item(sx,sy,stype_idx);
        if (is_room_thing(thing))
        {
          unsigned short sensitile=ty*MAP_SIZE_X+tx;
          set_thing_sensitile(thing,sensitile);
        }
    }
    return thing;
}

/*
 * Updates TNG, APT and LGT entries for the whole map - all tiles
 * and subtiles are reset. Additionally, USE values in columns
 * are recomputed to avoid mistakes.
 */
void update_obj_for_whole_map(struct LEVEL *lvl)
{
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_clmaffective_obj_for_slab(lvl, i, k);
}

/*
 * Updates TNG, APT and LGT entries for given map tile coordinates
 * and also updates enties in all neightbour squares
 */
void update_obj_for_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_clmaffective_obj_for_slab(lvl, i, k);
      }
}

/*
 * Updates TNG, APT and LGT entries for given map tile coordinates range
 */
void update_obj_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last)
{
    int i,k;
    for (k=ty_first;k<=ty_last;k++)
      for (i=tx_first;i<=tx_last;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_clmaffective_obj_for_slab(lvl, i, k);
      }
}

void update_clmaffective_obj_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    remove_misplaced_objs_on_slab(lvl,tx,ty);
    unsigned char slab=get_tile_slab(lvl,tx,ty);
    if (slab_is_room(slab))
    {
      update_room_things_on_slab(lvl,tx,ty);
    } else
    if (slab_is_door(slab))
    {
      update_door_things_on_slab(lvl,tx,ty);
    } else
    if (slab_needs_adjacent_torch(slab))
    {
      update_torch_things_near_slab(lvl,tx,ty);
    }
}

/*
 * Removes objects that usually belong do different slab type than the one we're on
 */
void remove_misplaced_objs_on_slab(struct LEVEL *lvl, int tx, int ty)
{
    unsigned char slab=get_tile_slab(lvl,tx,ty);
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            unsigned char expect_slb=get_usual_thing_slab(thing);
            if ((expect_slb!=slab)&&
               ((expect_slb!=SLAB_TYPE_CLAIMED)||(slab_is_tall(slab))))
                thing_del(lvl,sx, sy, i);
          }
     }
}

/*
 * Updates things associated with room - for every room calls the spefific
 * update function.
 * It uses only SLB/OWN entries to determine things parameters. CLM entries
 * can't be used because they may be not valid.
 */
void update_room_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned char slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
    case SLAB_TYPE_PORTAL:
      create_things_slb_room(delete_room_things,delete_room_things,
        delete_room_things,update_things_slb_portal_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TREASURE:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_treasure_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_PRISONCASE:
      create_things_slb_room_simple(update_things_slb_prison,lvl,tx,ty);
      break;
    case SLAB_TYPE_TORTURE:
      create_things_slb_room(update_things_slb_torture_edge,update_things_slb_torture_edge,
        update_things_slb_torture_edge,update_things_slb_torture_inside,update_things_slb_torture_inside,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TRAINING:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_training_corner,update_things_slb_training_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_DUNGHEART:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_dungheart_corner,update_things_slb_dungheart_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_WORKSHOP:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_workshop_corner,update_things_slb_workshop_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_SCAVENGER:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_scavenger_corner,update_things_slb_scavenger_inside,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_TEMPLE:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_temple_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_GRAVEYARD:
      create_things_slb_room(update_things_slb_graveyard_floor,update_things_slb_graveyard_floor,
        update_things_slb_graveyard_corner,update_things_slb_graveyard_floor,update_things_slb_graveyard_floor,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_HATCHERY:
      create_things_slb_room(delete_room_things,delete_room_things,
        update_things_slb_hatchery_corner,delete_room_things,delete_room_things,
        lvl, tx, ty);
      break;
    case SLAB_TYPE_BRIDGE:
      create_things_slb_room_simple(delete_room_things,lvl,tx,ty);
      break;
    case SLAB_TYPE_GUARDPOST:
      create_things_slb_room_simple(update_things_slb_guardpost_floor,lvl,tx,ty);
      break;
    case SLAB_TYPE_LIBRARY:
    case SLAB_TYPE_LAIR:
    case SLAB_TYPE_BARRACKS:
    default:
      create_things_slb_room_simple(delete_room_things,lvl,tx,ty);
      break;
    }
}

/*
 * Updates things on a slab containing door. If there is no door thing,
 * the function creates it, otherwise updates its parameters.
 * It uses only SLB/OWN entries to determine door position. CLM entries
 * can't be used because they may be not valid.
 */
void update_door_things_on_slab(struct LEVEL *lvl, int tx, int ty)
{
  unsigned char door_subtype;
  unsigned char slab=get_tile_slab(lvl,tx,ty);
  switch (slab)
  {
    case SLAB_TYPE_DOORWOOD1:
    case SLAB_TYPE_DOORWOOD2:
      door_subtype=DOOR_SUBTYPE_WOOD;
      break;
    case SLAB_TYPE_DOORBRACE1:
    case SLAB_TYPE_DOORBRACE2:
      door_subtype=DOOR_SUBTYPE_BRACED;
      break;
    case SLAB_TYPE_DOORIRON1:
    case SLAB_TYPE_DOORIRON2:
      door_subtype=DOOR_SUBTYPE_IRON;
      break;
    case SLAB_TYPE_DOORMAGIC1:
    case SLAB_TYPE_DOORMAGIC2:
      door_subtype=DOOR_SUBTYPE_MAGIC;
      break;
    default:
      door_subtype=DOOR_SUBTYPE_WOOD;
      break;
    }
    //Search for door things on all subtiles, delete if there is more than one
    unsigned char *thing=NULL;
    unsigned char *sec_thing;
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
          for (i=get_thing_subnums(lvl,sx,sy)-1; i >=0; i--)
          {
            sec_thing=get_thing(lvl,sx,sy,i);
            if (is_door(sec_thing))
            {
              // delete if we already have proper thing
              if (thing!=NULL)
                thing_del(lvl,sx, sy, i);
              else
              {
                //If that's firs one - keep it, but remove from level structure
                // (we will add it later to make sure it is in central subtile)
                thing=sec_thing;
                thing_drop(lvl,sx, sy, i);
              }
            }
          }
    // If we didn't find door thing, create the new one
    if (thing==NULL)
    {
      thing=create_door(lvl, tx*MAP_SUBNUM_X+1, ty*MAP_SUBNUM_Y+1, door_subtype);
    } else
    // and if we found one, modify its parameters
    {
      set_thing_subtype(thing,door_subtype);
      set_thing_tilepos(thing,tx*MAP_SUBNUM_X+1,ty*MAP_SUBNUM_Y+1);
      set_door_lock(lvl, thing, get_door_lock(thing));
      set_door_orientation(thing,compute_door_orientation(lvl,thing));
    }
    thing_add(lvl,thing);
}

/*
 * Updates torch things near a slab which should have torches near.
 * If there is no torch thing, the function creates it,
 * otherwise updates its parameters.
 * It uses only SLB/OWN entries to determine torch position. CLM entries
 * can't be used because they may be not valid.
 */
void update_torch_things_near_slab(struct LEVEL *lvl, int tx, int ty)
{
  const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH};
  const unsigned short dir_b[]={IDIR_EAST, IDIR_SOUTH, IDIR_WEST, IDIR_NORTH};
  int i;
  short allow_torch[9];
  //Filling matrix
  for (i=0;i<9;i++)
  {
    int cur_tx,cur_ty;
    cur_tx=tx+idir_subtl_x[i]-1;
    cur_ty=ty+idir_subtl_y[i]-1;
    unsigned char slab;
    slab=get_tile_slab(lvl,cur_tx,cur_ty);
    allow_torch[i]=slab_is_short(slab);
    if (slab==SLAB_TYPE_LAVA)
      allow_torch[i]=false;
  }
  // Well, let's just make torch where it can be made
  for (i=0;i<4;i++)
    if (allow_torch[dir_a[i]])
    {
      int cur_tx,cur_ty;
      cur_tx=tx+idir_subtl_x[dir_a[i]]-1;
      cur_ty=ty+idir_subtl_y[dir_a[i]]-1;
      //Check if there already is a light source on the destination slab
      if (find_lit_thing_on_square_radius1(lvl,cur_tx,cur_ty)==NULL)
      {
        int sx,sy;
        sx=cur_tx*3+idir_subtl_x[dir_b[i]];
        sy=cur_ty*3+idir_subtl_y[dir_b[i]];
        update_thing_slb_room_one_item_subtl(lvl,sx,sy,ITEM_SUBTYPE_TORCH,false);
      }
    }
}

/*
 * Updates things on any room slab by calling proper function
 * from these given as parameters.
 */
void create_things_slb_room(cr_tng_func cr_floor,cr_tng_func cr_edge,
        cr_tng_func cr_corner,cr_tng_func cr_inside,cr_tng_func cr_nearinsd,
        struct LEVEL *lvl, int tx, int ty)
{
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,NULL,tx,ty);
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Checking if completely surrounded
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
          cr_inside(lvl,tx,ty,surr_slb,surr_own);
      } else
      {
          //The 'near inside' columns are usually same that floor,
          //but may differ for rooms with specific corners
          cr_nearinsd(lvl,tx,ty,surr_slb,surr_own);
      }
      return;
  }
  //If not completely, maybe we're surrounded from 3 sides (5 with corners)?
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
      cr_edge(lvl,tx,ty,surr_slb,surr_own);
      return;
  }
  //If still nothing, maybe we have same surround from two sides and 1 corner,
  // and another two are surely something else
  if (((surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
       (surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_EAST]==slab)&&(surr_own[IDIR_EAST]==ownr) &&
       (surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr))) ||

      ((surr_slb[IDIR_SOUTH]==slab)&&(surr_own[IDIR_SOUTH]==ownr) &&
       (surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
      ((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))) ||

      ((surr_slb[IDIR_WEST]==slab)&&(surr_own[IDIR_WEST]==ownr) &&
       (surr_slb[IDIR_NORTH]==slab)&&(surr_own[IDIR_NORTH]==ownr) &&
      ((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      ((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr))))
  {
      cr_corner(lvl,tx,ty,surr_slb,surr_own);
      return;
  }
  //If nothing found - update as floor of this room
  cr_floor(lvl,tx,ty,surr_slb,surr_own);
}

/*
 * Updates things on any room slab; simplified version, for some easy rooms.
 */
void create_things_slb_room_simple(cr_tng_func cr_any,
        struct LEVEL *lvl, int tx, int ty)
{
  unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
  unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
  get_slab_surround(surr_slb,surr_own,NULL,tx,ty);
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  //Very simple...
  cr_any(lvl,tx,ty,surr_slb,surr_own);
}

void delete_room_things(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    int sx, sy;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
        delete_room_things_subtl(lvl,sx,sy);
      }
}

void delete_room_things_subtl(struct LEVEL *lvl, int sx, int sy)
{
    int last_thing=get_thing_subnums(lvl,sx,sy)-1;
    int i;
    for (i=last_thing; i>=0; i--)
    {
      unsigned char *thing=get_thing(lvl,sx,sy,i);
      if (is_room_thing(thing))
      {
          //checking surrounding slabs to be sure
          // if we should delete torches
          if (is_torch(thing))
            if (subtl_is_near_tall_slab(lvl,sx,sy))
              continue;
          // Now deleting, without mercy
          thing_del(lvl,sx, sy, i);
      }
    }
}

void update_things_slb_portal_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    int sx, sy, i;
    unsigned char *thing_eff=NULL;
    unsigned char *thing_eff2=NULL;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_thing(thing))
            {
                unsigned char stype_idx=get_thing_subtype(thing);
                if (is_roomeffect(thing)&&(stype_idx==ROOMEFC_SUBTP_ENTRICE)&&(thing_eff==NULL))
                  thing_eff=thing;
                else
                //Let's leave two entrance ice effects - we sometimes use two of them
                if (is_roomeffect(thing)&&(stype_idx==ROOMEFC_SUBTP_ENTRICE)&&(thing_eff2==NULL))
                  thing_eff2=thing;
                else
                thing_del(lvl,sx, sy, i);
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_eff==NULL)
    {
      thing_eff=create_roomeffect(sx,sy, ROOMEFC_SUBTP_ENTRICE);
      thing_add(lvl,thing_eff);
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff,get_tile_owner(lvl,tx,ty));
    }
    //Second effect is not auto-created - user must make it
    if (thing_eff2!=NULL)
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff2,get_tile_owner(lvl,tx,ty));
    }
}

unsigned char *update_thing_slb_room_one_central_item(struct LEVEL *lvl, int tx, int ty,
        unsigned char itm_stype_idx,short allow_create)
{
    int sx, sy, i;
    unsigned char *thing_dst=NULL;
    short other_thing_at_center=false;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_thing(thing))
            {
                unsigned char type_idx=get_thing_type(thing);
                unsigned char stype_idx=get_thing_subtype(thing);
                if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
                {
                  thing_dst=thing;
                } else
                // If we found torch, and we're searching for something else - leave the torch.
                if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(!is_torch_stype(itm_stype_idx)))
                {
                  if ((sx==tx*MAP_SUBNUM_X+1)&&(sy==ty*MAP_SUBNUM_Y+1))
                    other_thing_at_center=true;
                } else
                {
                  thing_del(lvl,sx, sy, i);
                }
            } else
            {
              if ((sx==tx*MAP_SUBNUM_X+1)&&(sy==ty*MAP_SUBNUM_Y+1))
                other_thing_at_center=true;
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_dst==NULL)
    {
      if ((!other_thing_at_center)&&(allow_create))
      {
        thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
        thing_add(lvl,thing_dst);
      }
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_dst,get_tile_owner(lvl,tx,ty));
    }
    return thing_dst;
}

unsigned char *update_thing_slb_room_one_item_subtl(struct LEVEL *lvl, int sx, int sy,
        unsigned char itm_stype_idx, short allow_delete)
{
    int i;
    unsigned char *thing_dst=NULL;
    short other_thing_there=false;
    int last_thing=get_thing_subnums(lvl,sx,sy)-1;
    for (i=last_thing; i>=0; i--)
    {
        unsigned char *thing=get_thing(lvl,sx,sy,i);
        if (is_room_thing(thing))
        {
            unsigned char type_idx=get_thing_type(thing);
            unsigned char stype_idx=get_thing_subtype(thing);
            if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
            {
                thing_dst=thing;
            } else
            // If we found torch, and we're searching for something else - leave the torch.
            if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(!is_torch_stype(itm_stype_idx)))
            {
                other_thing_there=true;
            } else
            if (allow_delete)
            {
                thing_del(lvl,sx, sy, i);
            }
        } else
        {
            other_thing_there=true;
        }
    }
    if (thing_dst==NULL)
    {
      if (!other_thing_there)
      {
        thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
        thing_add(lvl,thing_dst);
      }
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_dst,get_tile_owner(lvl,sx/MAP_SUBNUM_X,sy/MAP_SUBNUM_Y));
    }
    return thing_dst;
}

void update_things_slb_training_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TRAINPOST,true);
}

void update_things_slb_scavenger_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_SCAVNGEYE,true);
}

void update_things_slb_graveyard_floor(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_GRAVSTONE,true);
}

void update_things_slb_workshop_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_WRKSHPMCH,true);
}

void update_things_slb_torture_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORTURER,true);
}

void update_things_slb_treasure_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_CANDLSTCK,true);
}

void update_things_slb_torture_edge(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORTSPIKE,true);
}

void update_things_slb_training_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_scavenger_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_hatchery_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TORCH,true);
}

void update_things_slb_temple_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_TEMPLESTA,true);
}

void update_things_slb_workshop_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_ANVIL,true);
}

void update_things_slb_dungheart_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned char flame_stype;
    switch (surr_own[IDIR_CENTR])
    {
    case PLAYER0:
        flame_stype=ITEM_SUBTYPE_HEARTFLMR;
        break;
    case PLAYER1:
        flame_stype=ITEM_SUBTYPE_HEARTFLMB;
        break;
    case PLAYER2:
        flame_stype=ITEM_SUBTYPE_HEARTFLMG;
        break;
    case PLAYER3:
        flame_stype=ITEM_SUBTYPE_HEARTFLMY;
        break;
    case PLAYER_GOOD:
    case PLAYER_UNSET:
    default:
        flame_stype=0;
        break;
    }
    if (flame_stype!=0)
      update_thing_slb_room_one_central_item(lvl,tx,ty,flame_stype,true);
    else
      delete_room_things(lvl,tx,ty,surr_slb,surr_own);
}

void update_things_slb_dungheart_inside(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned char own=surr_own[IDIR_CENTR];
    if (own>=PLAYERS_COUNT) own=PLAYER_UNSET;
    if (own==PLAYER_UNSET)
    {
      delete_room_things(lvl,tx,ty,surr_slb,surr_own);
      return;
    }
    //Array for storing players heart count
    int hearts[PLAYERS_COUNT];
    int i;
    for (i=0; i < PLAYERS_COUNT; i++)
      hearts[i]=0;
    owned_things_count(hearts,lvl,THING_TYPE_ITEM,ITEM_SUBTYPE_DNHEART);
    update_thing_slb_room_one_central_item(lvl,tx,ty,ITEM_SUBTYPE_DNHEART,(hearts[own]==0));
}

void update_things_slb_graveyard_corner(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    int sx, sy, i;
    const unsigned char itm_stype_idx=ITEM_SUBTYPE_GRAVSTONE;
    const unsigned char eff_stype_idx=ROOMEFC_SUBTP_DRYICE;
    unsigned char *thing_dst=NULL;
    unsigned char *thing_eff=NULL;
    for (sx=tx*MAP_SUBNUM_X; sx < (tx+1)*MAP_SUBNUM_X; sx++)
      for (sy=ty*MAP_SUBNUM_Y; sy < (ty+1)*MAP_SUBNUM_Y; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            unsigned char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_thing(thing))
            {
                unsigned char type_idx=get_thing_type(thing);
                unsigned char stype_idx=get_thing_subtype(thing);
                if ((type_idx==THING_TYPE_ITEM)&&(stype_idx==itm_stype_idx)&&(thing_dst==NULL))
                  thing_dst=thing;
                else
                if ((type_idx==THING_TYPE_ROOMEFFECT)&&(stype_idx==eff_stype_idx)&&(thing_eff==NULL))
                  thing_eff=thing;
                else
                // If we found torch, and we're searching for something else - leave the torch.
                if ((type_idx==THING_TYPE_ITEM)&&(is_torch_stype(stype_idx))&&(!is_torch_stype(itm_stype_idx)))
                  continue;
                else
                  thing_del(lvl,sx, sy, i);
            }
          }
     }
    sx=tx*MAP_SUBNUM_X+1;
    sy=ty*MAP_SUBNUM_Y+1;
    if (thing_dst==NULL)
    {
      thing_dst=create_item_adv(lvl,sx,sy,itm_stype_idx);
      thing_add(lvl,thing_dst);
    } else
    {
      set_thing_owner(thing_dst,get_tile_owner(lvl,tx,ty));
    }
//TODO: put the dry ice effect on right position!
    if (thing_eff==NULL)
    {
      thing_eff=create_roomeffect(sx,sy,eff_stype_idx);
      thing_add(lvl,thing_eff);
    } else
    {
      //Position is not crucial, so leaving it as it was
      set_thing_owner(thing_eff,get_tile_owner(lvl,tx,ty));
    }
}

void update_things_slb_guardpost_floor(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
    unsigned char flag_stype;
    switch (surr_own[IDIR_CENTR])
    {
    case PLAYER0:
        flag_stype=ITEM_SUBTYPE_GURDFLAGR;
        break;
    case PLAYER1:
        flag_stype=ITEM_SUBTYPE_GURDFLAGB;
        break;
    case PLAYER2:
        flag_stype=ITEM_SUBTYPE_GURDFLAGG;
        break;
    case PLAYER3:
        flag_stype=ITEM_SUBTYPE_GURDFLAGY;
        break;
    case PLAYER_GOOD:
    case PLAYER_UNSET:
    default:
        flag_stype=ITEM_SUBTYPE_FLAGPOST;
        break;
    }
    update_thing_slb_room_one_central_item(lvl,tx,ty,flag_stype,true);
}

void update_things_slb_prison(struct LEVEL *lvl, int tx, int ty,
        unsigned char *surr_slb,unsigned char *surr_own)
{
  unsigned char slab=surr_slb[IDIR_CENTR];
  unsigned char ownr=surr_own[IDIR_CENTR];
  unsigned char pris_bars[9];
  int i;
  for (i=0;i<9;i++)
    pris_bars[i]=false;
  // Base directions
  if (((surr_slb[IDIR_NORTH]!=slab)||(surr_own[IDIR_NORTH]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_NORTH]))
    pris_bars[IDIR_NORTH]=true;
  if (((surr_slb[IDIR_EAST]!=slab)||(surr_own[IDIR_EAST]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_EAST]))
    pris_bars[IDIR_EAST]=true;
  if (((surr_slb[IDIR_SOUTH]!=slab)||(surr_own[IDIR_SOUTH]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_SOUTH]))
    pris_bars[IDIR_SOUTH]=true;
  if (((surr_slb[IDIR_WEST]!=slab)||(surr_own[IDIR_WEST]!=ownr)) &&
      !slab_is_tall(surr_slb[IDIR_WEST]))
    pris_bars[IDIR_WEST]=true;
  //Edges
  if ((pris_bars[IDIR_NORTH]==true)||(pris_bars[IDIR_EAST]==true))
    pris_bars[IDIR_NE]=true;
  if ((pris_bars[IDIR_NORTH]==true)||(pris_bars[IDIR_WEST]==true))
    pris_bars[IDIR_NW]=true;
  if ((pris_bars[IDIR_SOUTH]==true)||(pris_bars[IDIR_WEST]==true))
    pris_bars[IDIR_SW]=true;
  if ((pris_bars[IDIR_SOUTH]==true)||(pris_bars[IDIR_EAST]==true))
    pris_bars[IDIR_SE]=true;
  //Creating the bars where needed
  for (i=0;i<9;i++)
  {
    int sx=tx*3+idir_subtl_x[i];
    int sy=ty*3+idir_subtl_y[i];
    if (pris_bars[i])
    {
      //Create/update prison bar
      update_thing_slb_room_one_item_subtl(lvl,sx,sy,ITEM_SUBTYPE_PRISONBAR,true);
    } else
    {
      //Delete unwanted things
      delete_room_things_subtl(lvl,sx,sy);
    }
  }
}

/*
 * Updates position within subtile and height of all things on the whole map - all tiles
 * and subtiles are modified.
 */
void update_obj_subpos_and_height_for_whole_map(struct LEVEL *lvl)
{
    int i,k;
    for (k=0;k<MAP_SIZE_Y;k++)
      for (i=0;i<MAP_SIZE_X;i++)
          update_things_subpos_and_height_for_slab(lvl, i, k);
}

/*
 * Updates position within subtile and height of all things in given map tile coordinates
 * and also updates enties in all neightbour squares
 */
void update_obj_subpos_and_height_for_square_radius1(struct LEVEL *lvl, int tx, int ty)
{
    int i,k;
    for (k=ty-1;k<=ty+1;k++)
      for (i=tx-1;i<=tx+1;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_things_subpos_and_height_for_slab(lvl, i, k);
      }
}

/*
 * Updates position within subtile and height of all things
 * in given map tile coordinates range
 */
void update_obj_subpos_and_height_for_square(struct LEVEL *lvl, int tx_first, int tx_last,
    int ty_first, int ty_last)
{
    int i,k;
    for (k=ty_first;k<=ty_last;k++)
      for (i=tx_first;i<=tx_last;i++)
      {
        if ((i>=0) && (k>=0) && (i<MAP_SIZE_X) && (k<MAP_SIZE_Y))
          update_things_subpos_and_height_for_slab(lvl, i, k);
      }
}

/*
 * Updates position within subtile and height of all things in a tile.
 * Uses Column entries to find valid position.
 */
short update_things_subpos_and_height_for_slab(struct LEVEL *lvl, int tx, int ty)
{
    int sx, sy, i;
    short result=true;
    unsigned short clm_height[9];
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          //Getting height of the adjacent columns
          for (i=0;i<9;i++)
          {
            int cur_sx,cur_sy;
            cur_sx=sx+idir_subtl_x[i]-1;
            cur_sy=sy+idir_subtl_y[i]-1;
            unsigned char *clmentry;
            clmentry=get_subtile_column(lvl,cur_sx,cur_sy);
            clm_height[i]=get_clm_entry_height(clmentry);
          }
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            result&=update_thing_subpos_and_height(clm_height,thing);
          }
     }
    return result;
}

/*
 * Updates position within subtile and height of a thing.
 * Uses Column entries heigh to find valid position.
 */
short update_thing_subpos_and_height(unsigned short *clm_height,unsigned char *thing)
{
    if (thing==NULL) return false;
    unsigned char stype_idx=get_thing_subtype(thing);
    int sx, sy, i;
    sx=get_thing_tilepos_x(thing);
    sy=get_thing_tilepos_y(thing);
    //Setting things parameters
    if (is_door(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR];
        unsigned char subpos_h=0;
        if (subtl_h<4) { subtl_h=4; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_tilepos_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_doorkey(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR]-1;
        unsigned char subpos_h=0;
        if (subtl_h<1) { subtl_h=1; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_tilepos_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_heartflame(thing))
    {
        unsigned char sub_x=0x080;
        unsigned char sub_y=0x080;
        unsigned char subtl_h=clm_height[IDIR_CENTR];
        unsigned char subpos_h=0;
        if (subtl_h<1) { subtl_h=1; subpos_h=0; }
        if (subtl_h>7) { subtl_h=7; subpos_h=255; }
        set_thing_subtpos(thing,sub_x,sub_y);
        set_thing_tilepos_h(thing,subtl_h);
        set_thing_subtpos_h(thing,subpos_h);
    } else
    if (is_torch(thing))
    {
      const unsigned short dir_a[]={IDIR_WEST, IDIR_NORTH, IDIR_EAST, IDIR_SOUTH,
                                    IDIR_NW, IDIR_NE, IDIR_SE, IDIR_SW};
      const unsigned short dir_a_subx[]={0x040, 0x080, 0x0c0, 0x080,
                                         0x040, 0x0c0, 0x0c0, 0x040};
      const unsigned short dir_a_suby[]={0x080, 0x040, 0x080, 0x0c0,
                                         0x040, 0x040, 0x0c0, 0x0c0};
      unsigned char sub_x=0x080;
      unsigned char sub_y=0x080;
      unsigned char subtl_h=clm_height[IDIR_CENTR]+1;
      unsigned char subpos_h=224;
      //Set the subtile position tall column
      for (i=0;i<8;i++)
        if (clm_height[dir_a[i]]>subtl_h)
        {
          sub_x=dir_a_subx[i];
          sub_y=dir_a_suby[i];
          // If found a position - stop searching
          break;
        }
      // If it is at center - rise it a little higher
      if (((sx%MAP_SUBNUM_X)==1)&&((sy%MAP_SUBNUM_Y)==1))
      {
        subtl_h=clm_height[IDIR_CENTR]+2;
        subpos_h=0;
      }
      if (subtl_h<2) { subtl_h=2; subpos_h=0; }
      if (subtl_h>7) { subtl_h=7; subpos_h=255; }
      set_thing_subtpos(thing,sub_x,sub_y);
      set_thing_tilepos_h(thing,subtl_h);
      set_thing_subtpos_h(thing,subpos_h);
    }
    return true;
}

/*
 * Removes all things that usually exist only in specific rooms,
 * and aren't crucial for the game (like gates and dungeon hearts)
 */
void remove_noncrucial_room_things(int tx, int ty)
{
    int sx, sy, i;
    for (sx=tx*3; sx < tx*3+3; sx++)
      for (sy=ty*3; sy < ty*3+3; sy++)
      {
          int last_thing=get_thing_subnums(lvl,sx,sy)-1;
          for (i=last_thing; i>=0; i--)
          {
            char *thing=get_thing(lvl,sx,sy,i);
            if (is_room_thing(thing) && (!is_crucial_thing(thing)))
                thing_del(lvl,sx, sy, i);
          }
     }
}
