/******************************************************************************/
/** @file lev_data.c
 * Struct definitions and LEVEL structure handling routines.
 * @par Purpose:
 *     Defines functions for maintaining the level memory structure.
 *     This includes creating elements, deleting them and clearing
 *     whole structure.
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "lev_data.h"

#include <math.h>
#include <string.h>
#include <time.h>
#include "globals.h"
#include "obj_column_def.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "obj_column.h"
#include "lev_script.h"
#include "draw_map.h"
#include "msg_log.h"
#include "lbfileio.h"
#include "lev_column.h"
#include "graffiti.h"
#include "lev_things.h"
#include "obj_actnpts.h"
#include "bulcommn.h"
#include "arr_utils.h"
#include "rng.h"

const int idir_subtl_x[]={
    0, 1, 2,
    0, 1, 2,
    0, 1, 2,};

const int idir_subtl_y[]={
    0, 0, 0,
    1, 1, 1,
    2, 2, 2,};

const char default_map_name[]="Unnamed %Y.%m.%d map";

/**
 * Creates object for storing one level. Allocates memory and inits
 * the values to zero; drops any previous pointers without deallocating.
 * @param lvl_ptr Double pointer to the new level structure.
 * @param map_version Map version constant, from MAP_FORMAT_VERSION enumeration.
 * @param lvl_size Level size struct.This can be NULL if map_version 
 *     does not support multiple sizes of maps.
 * @return Returns false on error, true on success.
 */
short level_init(struct LEVEL **lvl_ptr,short map_version,struct UPOINT_3D *lvl_size)
{
     message_log(" level_init: started");
    (*lvl_ptr)=(struct LEVEL *)malloc(sizeof(struct LEVEL));
    struct LEVEL *lvl;
    lvl=(*lvl_ptr);
    if (lvl==NULL)
    {
        message_error("level_init: Cannot alloc memory for level");
        return false;
    }
    lvl->format_version=map_version;
    /* map file name */
    lvl->fname=(char *)malloc(DISKPATH_SIZE*sizeof(char));
    memset(lvl->fname,0,DISKPATH_SIZE*sizeof(char));
    lvl->savfname=(char *)malloc(DISKPATH_SIZE*sizeof(char));
    memset(lvl->savfname,0,DISKPATH_SIZE*sizeof(char));



    if ((lvl->format_version!=MFV_DKSTD)&&(lvl->format_version!=MFV_DKGOLD)&&(lvl_size!=NULL))
    {
        lvl->tlsize.x=lvl_size->x;
        lvl->tlsize.y=lvl_size->y;
    } else
    {
        lvl->tlsize.x=MAP_SIZE_DKSTD_X;
        lvl->tlsize.y=MAP_SIZE_DKSTD_Y;
    }
    lvl->subsize.x=lvl->tlsize.x*MAP_SUBNUM_X+1;
    lvl->subsize.y=lvl->tlsize.y*MAP_SUBNUM_Y+1;
    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;

  { /*allocating CLM structures */
    lvl->clm = (unsigned char **)malloc(COLUMN_ENTRIES*sizeof(char *));
    if (lvl->clm==NULL)
    {
        message_error("level_init: Cannot alloc clm memory");
        return false;
    }
    int i;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      lvl->clm[i]=(unsigned char *)malloc(SIZEOF_DK_CLM_REC);
      if (lvl->clm[i]==NULL)
    {
        message_error("level_init: Cannot alloc clm entries");
        return false;
    }
    }
    lvl->clm_hdr=(unsigned char *)malloc(SIZEOF_DK_CLM_HEADER);
    lvl->clm_utilize=(unsigned int *)malloc(COLUMN_ENTRIES*sizeof(unsigned int *));
  }
  { /*allocating SLB structures */
    int i;
    lvl->slb = (unsigned short **)malloc(lvl->tlsize.y*sizeof(unsigned short *));
    if (lvl->slb==NULL)
    {
        message_error("level_init: Cannot alloc slb memory");
        return false;
    }
    for (i=0; i < lvl->tlsize.y; i++)
    {
      lvl->slb[i] = (unsigned short *)malloc(lvl->tlsize.x*sizeof(unsigned short));
      if (lvl->slb[i]==NULL)
      {
        message_error("level_init: Cannot alloc slb entries");
        return false;
      }
    }
  }
  { /*allocating OWN structures */
    int i;
    lvl->own = (unsigned char **)malloc(lvl->subsize.y*sizeof(char *));
    if (lvl->own==NULL)
    {
        message_error("level_init: Cannot alloc own memory");
        return false;
    }
    for (i=0; i < lvl->subsize.y; i++)
    {
      lvl->own[i] = (unsigned char *)malloc(lvl->subsize.x*sizeof(char));
      if (lvl->own[i]==NULL)
      {
        message_error("level_init: Cannot alloc own entries");
        return false;
      }
    }
  }
  { /*allocating DAT structures */
    lvl->dat= (unsigned short **)malloc(lvl->subsize.y*sizeof(short *));
    if (lvl->dat==NULL)
    {
        message_error("level_init: Cannot alloc dat memory");
        return false;
    }
    int i;
    for (i=0; i < lvl->subsize.y; i++)
    {
      lvl->dat[i]= (unsigned short *)malloc(lvl->subsize.x*sizeof(unsigned short));
      if (lvl->dat[i]==NULL)
      {
        message_error("level_init: Cannot alloc dat entries");
        return false;
      }
    }
  }
  { /*allocating WIB structures */
    lvl->wib= (unsigned char **)malloc(lvl->subsize.y*sizeof(char *));
    if (lvl->wib==NULL)
    {
        message_error("level_init: Cannot alloc wib memory");
        return false;
    }
    int i;
    for (i=0; i < lvl->subsize.y; i++)
    {
      lvl->wib[i]= (unsigned char *)malloc(lvl->subsize.x*sizeof(char));
      if (lvl->wib[i]==NULL)
      {
        message_error("level_init: Cannot alloc wib entries");
        return false;
      }
    }
  }
  { /*allocating FLG structures */
    lvl->flg= (unsigned short **)malloc(lvl->subsize.y*sizeof(short *));
    if (lvl->flg==NULL)
    {
        message_error("level_init: Cannot alloc flg memory");
        return false;
    }
    int i;
    for (i=0; i < lvl->subsize.y; i++)
    {
      lvl->flg[i]= (unsigned short *)malloc(lvl->subsize.x*sizeof(unsigned short));
      if (lvl->flg[i]==NULL)
      {
        message_error("level_init: Cannot alloc flg entries");
        return false;
      }
    }
  }
  {
    int i;
    lvl->tng_apt_lgt_nums = (unsigned short **)malloc(lvl->tlsize.y*sizeof(unsigned short *));
    if (lvl->tng_apt_lgt_nums==NULL)
    {
        message_error("level_init: Cannot alloc apt memory");
        return false;
    }
    for (i=0; i<lvl->tlsize.y; i++)
    {
      lvl->tng_apt_lgt_nums[i]= (unsigned short *)malloc(lvl->tlsize.x*sizeof(unsigned short));
      if (lvl->tng_apt_lgt_nums[i]==NULL)
      {
        message_error("level_init: Cannot alloc apt entries");
        return false;
      }
    }
  }
  { /*Allocating "things" structure */
    lvl->tng_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->tng_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->tng_lookup==NULL) || (lvl->tng_subnums==NULL))
    {
        message_error("level_init: Cannot alloc tng memory");
        return false;
    }
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->tng_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->tng_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->tng_lookup[i]==NULL) || (lvl->tng_subnums[i]==NULL))
      {
        message_error("level_init: Cannot alloc tng subnums");
        return false;
      }
    }
  }
  { /*Allocating "action points" structure */
    lvl->apt_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->apt_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->apt_lookup==NULL) || (lvl->apt_subnums==NULL))
    {
        message_error("level_init: Cannot alloc apt lookup");
        return false;
    }
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->apt_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->apt_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->apt_lookup[i]==NULL) || (lvl->apt_subnums[i]==NULL))
      {
        message_error("level_init: Cannot alloc apt entries");
        return false;
      }
    }
  }
  { /*Allocating "static lights" structure */
    lvl->lgt_lookup = (unsigned char ****)malloc(arr_entries_y*sizeof(unsigned char ***));
    lvl->lgt_subnums= (unsigned short **)malloc(arr_entries_y*sizeof(unsigned short *));
    if ((lvl->lgt_lookup==NULL) || (lvl->lgt_subnums==NULL))
    {
        message_error("level_init: Cannot alloc lgt lookup");
        return false;
    }
    int i;
    for (i=0; i<arr_entries_y; i++)
    {
      lvl->lgt_lookup[i]=(unsigned char ***)malloc (arr_entries_x*sizeof(unsigned char **));
      lvl->lgt_subnums[i]=(unsigned short *)malloc (arr_entries_x*sizeof(unsigned short));
      if ((lvl->lgt_lookup[i]==NULL) || (lvl->lgt_subnums[i]==NULL))
      {
        message_error("level_init: Cannot alloc lgt entries");
        return false;
      }
    }
  }
  { /*Allocating WLB structure */
    lvl->wlb = (unsigned char **)malloc(lvl->tlsize.y*sizeof(unsigned char *));
    if (lvl->wlb==NULL)
    {
        message_error("level_init: Cannot alloc wlb memory");
        return false;
    }
    int i;
    for (i=0; i<lvl->tlsize.y; i++)
    {
      lvl->wlb[i]=(unsigned char *)malloc(lvl->tlsize.x*sizeof(unsigned char));
      if (lvl->wlb[i]==NULL)
      {
        message_error("level_init: Cannot alloc wlb entries");
        return false;
      }
    }
  }
  { /* allocating script structures */
    int idx;
    lvl->script.par.player=(struct DK_SCRIPT_PLAYER *)malloc(PLAYERS_COUNT*sizeof(struct DK_SCRIPT_PLAYER));
    lvl->script.par.creature_pool=(unsigned int *)malloc(creatures_cmd_arrsize()*sizeof(unsigned int));
    if ((lvl->script.par.player==NULL)||(lvl->script.par.creature_pool==NULL))
    {
        message_error("level_init: Cannot alloc script params memory");
        return false;
    }
    /* Double arrays with first indices PLAYER0..PLAYER_UNSET */
    for (idx=0;idx<PLAYERS_COUNT;idx++)
    {
      lvl->script.par.player[idx].ally=(unsigned short *)malloc(PLAYERS_COUNT*sizeof(unsigned short));
      lvl->script.par.player[idx].creature_avail=(unsigned short *)malloc(creatures_cmd_arrsize()*sizeof(unsigned short));
      lvl->script.par.player[idx].creature_maxlvl=(int *)malloc(creatures_cmd_arrsize()*sizeof(int));
      lvl->script.par.player[idx].room_avail=(unsigned short *)malloc(room_cmd_arrsize()*sizeof(unsigned short));
      lvl->script.par.player[idx].spell_avail=(unsigned short *)malloc(spell_cmd_arrsize()*sizeof(unsigned short));
      lvl->script.par.player[idx].trap_avail=(unsigned short *)malloc(trap_cmd_arrsize()*sizeof(unsigned short));
      lvl->script.par.player[idx].door_avail=(unsigned short *)malloc(door_cmd_arrsize()*sizeof(unsigned short));
      lvl->script.par.player[idx].trap_amount=(unsigned int *)malloc(trap_cmd_arrsize()*sizeof(unsigned int));
      lvl->script.par.player[idx].door_amount=(unsigned int *)malloc(door_cmd_arrsize()*sizeof(unsigned int));
      if ((lvl->script.par.player[idx].ally==NULL)||(lvl->script.par.player[idx].creature_avail==NULL)||
          (lvl->script.par.player[idx].creature_maxlvl==NULL)||
          (lvl->script.par.player[idx].room_avail==NULL)||(lvl->script.par.player[idx].spell_avail==NULL)||
          (lvl->script.par.player[idx].trap_avail==NULL)||(lvl->script.par.player[idx].door_avail==NULL)||
          (lvl->script.par.player[idx].trap_amount==NULL)||(lvl->script.par.player[idx].door_amount==NULL))
      {
          message_error("level_init: Cannot alloc script params entries");
          return false;
      }
    }
  }
  { /*allocating cust.columns structures */
    lvl->cust_clm_lookup= (struct DK_CUSTOM_CLM ***)malloc(lvl->subsize.y*sizeof(struct DK_CUSTOM_CLM **));
    if (lvl->cust_clm_lookup==NULL)
    {
        message_error("level_init: Cannot alloc clm memory");
        return false;
    }
    int i;
    for (i=0; i < lvl->subsize.y; i++)
    {
      lvl->cust_clm_lookup[i]= (struct DK_CUSTOM_CLM **)malloc(lvl->subsize.x*sizeof(struct DK_CUSTOM_CLM *));
      if (lvl->cust_clm_lookup[i]==NULL)
      {
        message_error("level_init: Cannot alloc clm lookup");
        return false;
      }
    }
  }
  message_log(" level_init: finished, now clearing");
  level_clear_options(&(lvl->optns));
  return level_clear(lvl);
}

/**
 * Resizes arrays in object for storing level. Allocates memory and inits
 * the values to zero; deallocates previous pointers.
 * @param lvl Pointer to the level structure.
 * @param lvl_size Level size struct.
 * @return Returns ERR_NONE on success, error code on failure.
 * @todo Finish, based on level_init.
 */
short level_set_size(struct LEVEL *lvl,struct UPOINT_3D *lvl_size)
{
    message_log(" level_set_size: started");
    if (lvl==NULL)
    {
        message_error("level_set_size: Level structure doesn't exist");
        return false;
    }
    lvl->tlsize.x=lvl_size->x;
    lvl->tlsize.y=lvl_size->y;
    lvl->subsize.x=lvl->tlsize.x*MAP_SUBNUM_X+1;
    lvl->subsize.y=lvl->tlsize.y*MAP_SUBNUM_Y+1;
    /*TODO: Allocate the structures of given size */
    message_log(" level_set_size: finished");
    return true;
}

/**
 * Clears options for given level. This one is not executed
 * when clearing the level, only when making new one.
 * @param optns The LEVOPTIONS structure to clear.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short level_clear_options(struct LEVOPTIONS *optns)
{
    optns->unaffected_gems=false;
    optns->unaffected_rock=false;
    optns->fill_reinforced_corner=true;
    optns->frail_columns=true;
    optns->datclm_auto_update=true;
    optns->obj_auto_update=true;
    optns->levels_path=NULL;
    optns->data_path=NULL;
    optns->load_redundant_objects=true;
    optns->verify_warn_flags=VWFLAG_NONE;
    optns->picture.rescale=4;
    optns->picture.data_path=NULL;
    optns->picture.bmfonts=BMFONT_DONT_LOAD;
    optns->picture.tngflags=TNGFLG_NONE;
    optns->script.level_spaces=4;
    return ERR_NONE;
}

/**
 * Sets all options for the LEVEL structure from another LEVOPTIONS.
 * Copies the whole LEVOPTIONS structure into the one in LEVEL.
 * @param lvl Pointer to the LEVEL structure.
 * @param optns The LEVOPTIONS structure to copy from.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short level_set_options(struct LEVEL *lvl,struct LEVOPTIONS *optns)
{
    memcpy(&(lvl->optns),optns,sizeof(struct LEVOPTIONS));
    lvl->optns.picture.data_path=lvl->optns.data_path;
    return ERR_NONE;
}

/**
 * Gets all options for the LEVEL structure.
 * Returns direct pointer to the structure.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns LEVOPTIONS structure.
 */
struct LEVOPTIONS *level_get_options(struct LEVEL *lvl)
{
    return &(lvl->optns);
}

/**
 * Sets the map drawing options for the LEVEL structure from another MAPDRAW_OPTIONS.
 * Copies the whole MAPDRAW_OPTIONS structure into the one in LEVEL.
 * Keeps only previous value of data_path.
 * @param lvl Pointer to the LEVEL structure.
 * @param mdrwopts The MAPDRAW_OPTIONS structure to copy from.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short level_set_mapdraw_options(struct LEVEL *lvl,struct MAPDRAW_OPTIONS *mdrwopts)
{
    memcpy(&(lvl->optns.picture),mdrwopts,sizeof(struct MAPDRAW_OPTIONS));
    lvl->optns.picture.data_path=lvl->optns.data_path;
    return ERR_NONE;
}

/**
 * Gets the map drawing options for the LEVEL structure.
 * Returns direct pointer to the structure.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns MAPDRAW_OPTIONS structure from the level.
 */
struct MAPDRAW_OPTIONS *level_get_mapdraw_options(struct LEVEL *lvl)
{
    return &(lvl->optns.picture);
}

/**
 * Clears the "things" structure for storing level. Drops any old pointers
 * without deallocating them. Requies level_init() to be run first.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_tng(struct LEVEL *lvl)
{
  /*Preparing array bounds */
  int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
  int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
  /*Clearing single variables */
  lvl->tng_total_count=0;
  /*Clearing pointer arrays */
  int i,j;
  for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->tng_lookup[i][j]=NULL;
          lvl->tng_subnums[i][j]=0;
      }
  for (i=0; i<lvl->tlsize.y; i++)
      for (j=0; j<lvl->tlsize.x; j++)
          lvl->tng_apt_lgt_nums[i][j]=0;

  /*Clearing related stats variables */
  lvl->stats.hero_gates_count=0;
  return true;
}

/**
 * clears the "actions" structure for storing level. Drops any old pointers
 * without deallocating them. Requies level_init() to be run first.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_apt(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Clearing single variables */
    lvl->apt_total_count=0;
    /*Clearing pointer arrays */
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->apt_lookup[i][j]=NULL;
          lvl->apt_subnums[i][j]=0;
      }
  return true;
}

/**
 * clears the "lights" structure for storing level. Drops any old pointers
 * without deallocating them. Requies level_init() to be run first.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_lgt(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Clearing single variables */
    lvl->lgt_total_count=0;
    /*Clearing pointer arrays */
    int i,j;
    for (i=0; i<arr_entries_y; i++)
      for (j=0; j<arr_entries_x; j++)
      {
          lvl->lgt_lookup[i][j]=NULL;
          lvl->lgt_subnums[i][j]=0;
      }
  return true;
}

/**
 * Clears the "column" structure for storing level. Will not drop any pointers.
 * Requies level_init() to be run first.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_datclm(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    /*const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;*/

    /* zero-filling CLM memory */
    int i,k;
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      lvl->clm_utilize[i]=0;
      clear_clm_entry(lvl->clm[i]);
    }
    /*Clearing CLM header */
    memset(lvl->clm_hdr,0,SIZEOF_DK_CLM_HEADER);
    write_int32_le_buf(lvl->clm_hdr+0,COLUMN_ENTRIES);
    /* Setting all DAT entries to one, first column */
    /* (it is unused in all maps) */
    for (k=0; k<lvl->subsize.y; k++)
      for (i=0; i<lvl->subsize.x; i++)
      {
          set_dat_subtile(lvl, i, k, 0);
          lvl->clm_utilize[0]++;
      }

    /*Filling CLM entries with unused, zero-filled ones */
    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    /*First one is special - should have nonzero use at start */
    clmentry = (unsigned char *)(lvl->clm[0]);
    fill_column_rec_sim(clm_rec,lvl->clm_utilize[0], 0,  0, 0, 0, 0, 0, 0, 0, 0);
    set_clm_entry(clmentry, clm_rec);

    #if 0
    /* filling with zeros again is now not needed, */
    /* but may become needed on future modifications (if USE will be required to set here) */
    fill_column_rec_sim(clm_rec,0, 0,  0, 0, 0, 0, 0, 0, 0, 0);
    for (i=1; i < COLUMN_ENTRIES; i++)
    {
      clmentry = (unsigned char *)(lvl->clm[i]);
      set_clm_entry(clmentry, clm_rec);
    }
    #endif

    free_column_rec(clm_rec);
    return true;
}

/**
 * Clears (via zeroing) stats for given level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_stats(struct LEVEL *lvl)
{
    lvl->stats.creatures_count=0;
    lvl->stats.effectgenrts_count=0;
    lvl->stats.traps_count=0;
    lvl->stats.doors_count=0;
    lvl->stats.items_count=0;
    /*Items stats */
    lvl->stats.hero_gates_count=0;
    lvl->stats.dn_hearts_count=0;
    int i;
    for (i=0;i<THING_CATEGR_COUNT;i++)
      lvl->stats.things_count[i]=0;
    /*Various stats */
    lvl->stats.room_things_count=0;
    /*Stats on objects adding/removal */
    lvl->stats.things_removed=0;
    lvl->stats.things_added=0;
    /* Count of saves in this session */
    lvl->stats.saves_count=0;
    /* Number of unsaved changes */
    lvl->stats.unsaved_changes=0;
    return true;
}

/**
 * Clears (sets for new map) info for given level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_info(struct LEVEL *lvl)
{
    /* User stats */
    lvl->info.usr_cmds_count=0;
    lvl->info.usr_mdswtch_count=0;
    lvl->info.usr_slbchng_count=0;
    lvl->info.usr_creatobj_count=0;
    /* Map creation date */
    lvl->info.creat_date=time(NULL);
    lvl->info.lastsav_date=lvl->info.creat_date;
    /* Map version */
    lvl->info.ver_major=0;
    lvl->info.ver_minor=0;
    lvl->info.ver_rel=0;
    int name_len=strlen(default_map_name)+10;
    char *name_text=malloc(name_len);
    if (name_text!=NULL)
        strftime(name_text,name_len, default_map_name, localtime(&(lvl->info.creat_date)) );
    lvl->info.name_text=name_text;
    lvl->info.desc_text=NULL;
    lvl->info.author_text=NULL;
    lvl->info.editor_text=NULL;
    return true;
}

/**
 * Clears (via zeroing) TXT script info for given level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_script(struct LEVEL *lvl)
{
    message_log("  level_clear_script: started");
    lvl->script.list=NULL;
    lvl->script.txt=NULL;
    lvl->script.lines_count=0;
    /* Zeroing DK_SCRIPT_PARAMETERS struct */
    return level_clear_script_param(&(lvl->script.par));
}

/**
 * Clears (via zeroing) given script parameters struct.
 * @param par Pointer to the DK_SCRIPT_PARAMETERS structure.
 * @return Returns true on success, false on failure.
 */
short level_clear_script_param(struct DK_SCRIPT_PARAMETERS *par)
{
    par->portal_gen_speed=600;
    par->end_level=0;
    int i;
    int max_idx;
    for (i=0;i<PLAYERS_COUNT;i++)
    {
      /* Arrays with indices PLAYER0..PLAYER_UNSET */
      par->player[i].max_creatures=0;
      par->player[i].start_gold=0;
      par->player[i].computer_player=-1;
      /* Double arrays with first indices PLAYER0..PLAYER_UNSET */
      int k;
      for (k=0;k<PLAYERS_COUNT;k++)
      {
        par->player[i].ally[k]=false;
      }
      max_idx=creatures_cmd_arrsize();
      for (k=0;k<max_idx;k++)
      {
        par->player[i].creature_avail[k]=AVAIL_NO;
        par->player[i].creature_maxlvl[k]=-1;
      }
      max_idx=trap_cmd_arrsize();
      for (k=0;k<max_idx;k++)
      {
        par->player[i].trap_avail[k]=AVAIL_NO;
        par->player[i].trap_amount[k]=0;
      }
      max_idx=door_cmd_arrsize();
      for (k=0;k<max_idx;k++)
      {
        par->player[i].door_avail[k]=AVAIL_NO;
        par->player[i].door_amount[k]=0;
      }
      max_idx=room_cmd_arrsize();
      for (k=0;k<max_idx;k++)
      {
        par->player[i].room_avail[k]=AVAIL_NO;
      }
      max_idx=spell_cmd_arrsize();
      for (k=0;k<max_idx;k++)
      {
        par->player[i].spell_avail[k]=AVAIL_NO;
      }
    }
    max_idx=creatures_cmd_arrsize();
    for (i=0;i<max_idx;i++)
      par->creature_pool[i]=0;
    return true;
}

/**
 * Clears minor LEVEL substructures.
 * Clears the structures for storing level which do not have separate
 * clearing function. Drops any old pointers without deallocating them,
 * only file name remains. Requies level_init() to be run first.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_clear_other(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    /*const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;*/

    int i;
    for (i=0; i < lvl->tlsize.y; i++)
    {
      if (lvl->slb[i]!=NULL)
        memset(lvl->slb[i],0,lvl->tlsize.x*sizeof(unsigned short));
    }
    for (i=0; i < lvl->subsize.y; i++)
    {
      if (lvl->own[i]!=NULL)
        memset(lvl->own[i],PLAYER_UNSET,lvl->subsize.x*sizeof(char));
    }

    for (i=0; i < lvl->subsize.y; i++)
    {
      if (lvl->dat[i]!=NULL)
        memset(lvl->dat[i],0,lvl->subsize.x*sizeof(unsigned short));
    }

    for (i=0; i < lvl->subsize.y; i++)
    {
      if (lvl->wib[i]!=NULL)
        memset(lvl->wib[i],COLUMN_WIB_SKEW,lvl->subsize.x*sizeof(char));
    }

    for (i=0; i < lvl->subsize.y; i++)
    {
      if (lvl->flg[i]!=NULL)
        memset(lvl->flg[i],0,lvl->subsize.x*sizeof(unsigned short));
    }

    for (i=0; i < lvl->tlsize.y; i++)
    {
      if (lvl->wlb[i]!=NULL)
        memset(lvl->wlb[i],0,lvl->tlsize.x*sizeof(char));
    }
    
    /* INF file is easy */
    lvl->inf=0x00;

    /* The Adikted-custom elements */
    if (lvl->cust_clm_lookup!=NULL)
    {
      for (i=0; i < lvl->subsize.y; i++)
      {
        if (lvl->cust_clm_lookup[i]!=NULL)
          memset(lvl->cust_clm_lookup[i],0,lvl->subsize.x*sizeof(struct DK_CUSTOM_CLM  *));
      }
    }
    lvl->cust_clm_count=0;
    lvl->graffiti=NULL;
    lvl->graffiti_count=0;

    memset(lvl->slx_data, 0, sizeof(lvl->slx_data));
    return true;
}

/**
 * Clears the whole object for storing level. Drops any old pointers
 * without deallocating them. Requies level_init() to be run first.
 * Leaves only optns struct unaffected.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_clear(struct LEVEL *lvl)
{
  message_log(" level_clear: started");
  short result=true;
  result&=level_clear_lgt(lvl);
  result&=level_clear_apt(lvl);
  result&=level_clear_tng(lvl);
  result&=level_clear_datclm(lvl);
  result&=level_clear_stats(lvl);
  result&=level_clear_info(lvl);
  result&=level_clear_script(lvl);
  result&=level_clear_other(lvl);
    message_log(" level_clear: finished");
  return result;
}

/**
 * Frees all sub-structures of given DK_SCRIPT_PARAMETERS.
 * @param par Pointer to the DK_SCRIPT_PARAMETERS structure.
 * @return Returns true on success, false on error.
 */
short level_free_script_param(struct DK_SCRIPT_PARAMETERS *par)
{
  message_log(" level_free_script_param: starting");
  int idx;
  free(par->creature_pool);
  /* Double arrays with first indices PLAYER0..PLAYER_UNSET */
/*  message_log(" level_free_script_param: freeing 2d player arrays"); */
  for (idx=0;idx<PLAYERS_COUNT;idx++)
  {
    free(par->player[idx].ally);
    free(par->player[idx].creature_avail);
    free(par->player[idx].creature_maxlvl);
    free(par->player[idx].room_avail);
    free(par->player[idx].spell_avail);
    free(par->player[idx].trap_avail);
    free(par->player[idx].door_avail);
    free(par->player[idx].trap_amount);
    free(par->player[idx].door_amount);
  }
  free(par->player);
  return true;
}

/**
 * Frees structures for storing level. Frees only the memory
 * allocated by level_init(); to free the content of loaded level,
 * you must call level_free() first.
 * @see level_free
 * @param lvl_ptr Double pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_deinit(struct LEVEL **lvl_ptr)
{
    message_log(" level_deinit: started");
    if ((lvl_ptr==NULL)||((*lvl_ptr)==NULL))
      return false;
    struct LEVEL *lvl;
    lvl=(*lvl_ptr);
    /*Preparing array bounds */
    /*const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;*/
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;

/*    message_log(" level_deinit: Freeing SLB structure"); */
    if (lvl->slb!=NULL)
    {
      int i;
      for (i=0; i<lvl->tlsize.y; i++)
          free(lvl->slb[i]);
      free(lvl->slb);
    }

/*    message_log(" level_deinit: Freeing OWN structure"); */
    if (lvl->own!=NULL)
    {
      int i;
      for (i=0; i<lvl->subsize.y; i++)
          free(lvl->own[i]);
      free(lvl->own);
    }

/*    message_log(" level_deinit: Freeing DAT structure"); */
    if (lvl->dat!=NULL)
    {
      int i;
      for (i=0; i<lvl->subsize.y; i++)
          free(lvl->dat[i]);
      free (lvl->dat);
    }

/*    message_log(" level_deinit: Freeing WIB structure"); */
    if (lvl->wib!=NULL)
    {
      int i;
      for (i=0; i<lvl->subsize.y; i++)
          free(lvl->wib[i]);
      free (lvl->wib);
    }

/*    message_log(" level_deinit: Freeing FLG structure"); */
    if (lvl->flg!=NULL)
    {
      int i;
      for (i=0; i<lvl->subsize.y; i++)
          free(lvl->flg[i]);
      free (lvl->flg);
    }

/*    message_log(" level_deinit: Freeing \"things\" structure"); */
    if (lvl->tng_apt_lgt_nums!=NULL)
    {
      int i;
      for (i=0; i<lvl->tlsize.y; i++)
        free(lvl->tng_apt_lgt_nums[i]);
      free(lvl->tng_apt_lgt_nums);
    }
    if (lvl->tng_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_lookup[i]);
      free(lvl->tng_lookup);
    }
    if (lvl->tng_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->tng_subnums[i]);
      free(lvl->tng_subnums);
    }

/*    message_log(" level_deinit: Freeing action points structure"); */
    if (lvl->apt_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_lookup[i]);
      free(lvl->apt_lookup);
    }
    if (lvl->apt_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->apt_subnums[i]);
      free(lvl->apt_subnums);
    }

/*    message_log(" level_deinit: Freeing static lights structure"); */
    if (lvl->lgt_lookup!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->lgt_lookup[i]);
      free(lvl->lgt_lookup);
    }
    if (lvl->lgt_subnums!=NULL)
    {
      int i;
      for (i=0; i<arr_entries_y; i++)
        free(lvl->lgt_subnums[i]);
      free(lvl->lgt_subnums);
    }

/*    message_log(" level_deinit: Freeing column structure"); */
    if (lvl->clm!=NULL)
    {
      int i;
      for (i=0; i<COLUMN_ENTRIES; i++)
          free (lvl->clm[i]);
      free(lvl->clm);
      free(lvl->clm_hdr);
      free(lvl->clm_utilize);
    }

/*    message_log(" level_deinit: Freeing WLB structure"); */
    if (lvl->wlb!=NULL)
    {
      int i;
      for (i=0; i<lvl->tlsize.y; i++)
          free(lvl->wlb[i]);
      free(lvl->wlb);
    }

    level_free_script_param(&(lvl->script.par));

/*    message_log(" level_deinit: Freeing cust.columns structure"); */
    if (lvl->cust_clm_lookup!=NULL)
    {
      int i;
      for (i=0; i<lvl->subsize.y; i++)
          free(lvl->cust_clm_lookup[i]);
      free (lvl->cust_clm_lookup);
    }
    
    /*TODO: free graffiti */

    free(lvl->info.name_text);
    free(lvl->info.desc_text);
    free(lvl->info.author_text);
    free(lvl->info.editor_text);

    free(lvl->fname);
    free(lvl->savfname);

    /*Final freeing - main lvl object */
    free(lvl);
    *lvl_ptr=NULL;
    message_log(" level_deinit: finished");
    return true;
}

/**
 * Frees "things" structure for storing level. Disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared).
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_free_tng(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Freeing object arrays */
    if ((lvl->tng_subnums!=NULL) && (lvl->tng_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->tng_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                thing_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/**
 * Frees "action points" structure for storing level. Disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared).
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_free_apt(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Freeing object arrays */
    if ((lvl->apt_subnums!=NULL) && (lvl->apt_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->apt_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                actnpt_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/**
 * Frees TXT structure for storing level script. Disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared).
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_free_txt(struct LEVEL *lvl)
{
  int idx=lvl->script.lines_count;
  while (idx>0)
  {
    idx--;
    free(lvl->script.txt[idx]);
    free(lvl->script.list[idx]);
  }
  free(lvl->script.txt);
  free(lvl->script.list);
  lvl->script.lines_count=0;
  return true;
}

/**
 * Frees any text file loaded as lines.
 * Pointer value and lines count is cleared.
 * @param lines Pointer to the lines array.
 * @param lines_count Amount of allocated lines.
 * @return Returns true on success, false on error.
 */
short free_text_file(char ***lines,int *lines_count)
{
  int idx=(*lines_count);
  while (idx>0)
  {
    idx--;
    free((*lines)[idx]);
  }
  free(*lines);
  (*lines)=NULL;
  (*lines_count)=0;
  return true;
}

/**
 * Frees LGT structure for storing level lights. Disposes only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared).
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_free_lgt(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Freeing object arrays */
    if ((lvl->lgt_subnums!=NULL) && (lvl->lgt_lookup!=NULL))
    {
      int cx,cy,k;
      for (cx=0; cx<arr_entries_x; cx++)
      {
          for (cy=0; cy<arr_entries_y; cy++)
          {
            int last_idx=lvl->lgt_subnums[cx][cy]-1;
            for (k=last_idx; k >=0; k--)
                stlight_del(lvl,cx, cy, k);
          }
      }
    }
  return true;
}

/**
 * Frees structures for storing level. Frees only data pointers,
 * the array structure remains intact (as after level_init(), but values
 * are not cleared - use level_clear() to set NULLs to pointers).
 * @see level_clear
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns true on success, false on error.
 */
short level_free(struct LEVEL *lvl)
{
  message_log(" level_free: started");
  short result=true;
  result&=level_free_lgt(lvl);
  result&=level_free_apt(lvl);
  result&=level_free_tng(lvl);
  result&=level_free_txt(lvl);
  result&=level_free_graffiti(lvl);
  message_log(" level_free: finished");
  return result;
}

/**
 * Returns DK_SCRIPT_PARAMETERS struct for given level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns pointer to DK_SCRIPT_PARAMETERS, or NULL on error.
 */
struct DK_SCRIPT_PARAMETERS *level_get_script_param(struct LEVEL *lvl)
{
  if (lvl==NULL)
    return NULL;
  return &(lvl->script.par);
}

/**
 * Verifies the whole level. On error adds description message to the
 * error messages log.
 * @param lvl Pointer to the LEVEL structure.
 * @param actn_name Name of the action which invoked verification.
 * @param errpt Coordinates of the map tile containing the error.
 * @return Returns VERIF_ERROR, VERIF_WARN or VERIF_OK.
 *     If a problem was found, adds error message and sets errpt accordingly.
 */
short level_verify(struct LEVEL *lvl, char *actn_name,struct IPOINT_2D *errpt) {
    return level_verify_control( lvl, actn_name, 0, errpt );
}

short level_verify_control(struct LEVEL *lvl, char *actn_name, unsigned long skip_step_flags, struct IPOINT_2D *errpt) {
  char err_msg[LINEMSG_SIZE];
  strcpy(err_msg,"Unknown error");
  short result=VERIF_OK;
  short nres;
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_STRUCT) )
  {
    nres=level_verify_struct(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_THINGS) )
  {
    nres=things_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_SLABS) )
  {
    nres=slabs_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_ACTNPNTS) )
  {
    nres=actnpts_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_COLUMNS) )
  {
    nres=columns_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_DAT) )
  {
    nres=dat_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_TXT) )
  {
    nres=txt_verify(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  if ( result!=VERIF_ERROR && !(skip_step_flags & VSF_LOGIC) )
  {
    nres=level_verify_logic(lvl,err_msg,errpt);
    if (nres!=VERIF_OK) result=nres;
  }
  switch (result)
  {
    case VERIF_OK:
      message_info("Level verification passed.");
      return VERIF_OK;
    case VERIF_WARN:
      if (actn_name==NULL)
        message_error("Warning: %s",err_msg);
      else
        message_error("Warning: %s (%s performed)",err_msg,actn_name);
      return VERIF_WARN;
    default:
      if (actn_name==NULL)
      message_error("Error: %s",err_msg);
      else
      message_error("Error: %s (%s cancelled)",err_msg,actn_name);
      return VERIF_ERROR;
  }
}

/**
 * Verifies internal LEVEL structure integrity.
 * @param lvl Pointer to the LEVEL structure.
 * @param err_msg Error message output buffer.
 * @param errpt Coordinates of the map tile containing the error.
 * @return Returns VERIF_ERROR, VERIF_WARN or VERIF_OK.
 *     If a problem was found, sets err_msg and errpt accordingly.
 */
short level_verify_struct(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Checking base pointers */
    if (lvl->tng_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object tng_subnums!",LINEMSG_SIZE);
          errpt->x=-1;errpt->y=-1;
          return VERIF_ERROR;
    }
    if (lvl->apt_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object apt_subnums!",LINEMSG_SIZE);
          errpt->x=-1;errpt->y=-1;
          return VERIF_ERROR;
    }
    if (lvl->lgt_subnums==NULL)
    {
          strncpy(err_msg,"Null internal object lgt_subnums!",LINEMSG_SIZE);
          errpt->x=-1;errpt->y=-1;
          return VERIF_ERROR;
    }
    /*Sweeping through structures */
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          if (thing==NULL)
          {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Null thing pointer at slab %d,%d.",errpt->x,errpt->y);
              return VERIF_ERROR;
          }
        }

        int actpt_count=lvl->apt_subnums[i][j];
        for (k=0; k <actpt_count ; k++)
        {
          unsigned char *actnpt = lvl->apt_lookup[i][j][k];
          if (actnpt==NULL)
          {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Null action point pointer at slab %d,%d.",errpt->x,errpt->y);
              return VERIF_ERROR;
          }
        }
        
        int stlight_count=lvl->lgt_subnums[i][j];
        for (k=0; k <stlight_count ; k++)
        {
          unsigned char *stlight = lvl->lgt_lookup[i][j][k];
          if (stlight==NULL)
          {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Null static light pointer at slab %d,%d.",errpt->x,errpt->y);
              return VERIF_ERROR;
          }
        }
      }
    }
    for (i=0; i<COLUMN_ENTRIES; i++)
    {
      if (lvl->clm[i]==NULL)
      {
        errpt->x=-1;errpt->y=-1;
        sprintf(err_msg,"Null CoLuMn entry at index %d.",i);
        return VERIF_ERROR;
      }
    }
    if ((lvl->clm_hdr==NULL)||(lvl->clm_utilize==NULL))
    {
      errpt->x=-1;errpt->y=-1;
      sprintf(err_msg,"Null CoLuMn help arrays.");
      return VERIF_ERROR;
    }
    if (lvl->inf>7)
    {
          errpt->x=-1;errpt->y=-1;
          strncpy(err_msg,"Unexpected value of INF entry.",LINEMSG_SIZE);
          return VERIF_WARN;
    }
  return VERIF_OK;
}

/**
 * Verifies action points parameters.
 * @param lvl Pointer to the LEVEL structure.
 * @param err_msg Error message output buffer.
 * @param errpt Coordinates of the map tile containing the error.
 * @return Returns VERIF_ERROR, VERIF_WARN or VERIF_OK.
 *     If a problem was found, sets err_msg and errpt accordingly.
 */
short actnpts_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
      for (j=0; j < arr_entries_x; j++)
      {
        int actnpt_count=get_actnpt_subnums(lvl,i,j);
        for (k=0; k <actnpt_count ; k++)
        {
            unsigned char *actnpt = get_actnpt(lvl,i,j,k);
            unsigned char subt_x=get_actnpt_subtile_x(actnpt);
            unsigned char subt_y=get_actnpt_subtile_y(actnpt);
            unsigned char subt_r=get_actnpt_range_subtile(actnpt);
            unsigned short n=get_actnpt_number(actnpt);
            /*unsigned short slab=get_tile_slab(lvl,i/MAP_SUBNUM_X,j/MAP_SUBNUM_Y);*/
            /*int col_h=get_subtile_column_height(lvl,i,j);*/
            if ((subt_x>=arr_entries_x)||(subt_y>=arr_entries_y))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Action point has bad position data on slab %d,%d.",errpt->x,errpt->y);
              return VERIF_WARN;
            }
            if (subt_r>60)
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Action point range too big on slab %d,%d.",errpt->x,errpt->y);
              return VERIF_WARN;
            }
            if ((n<1)||(n>4096))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Incorrect action point number on slab %d,%d.",errpt->x,errpt->y);
              return VERIF_WARN;
            }
        }
      }
  return VERIF_OK;
}

/**
 * Verifies various logic aspects of a map.
 * @param lvl Pointer to the LEVEL structure.
 * @param err_msg Error message output buffer.
 * @param errpt Coordinates of the map tile containing the error.
 * @return Returns VERIF_ERROR, VERIF_WARN or VERIF_OK.
 *     If a problem was found, sets err_msg and errpt accordingly.
 */
short level_verify_logic(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          /*unsigned char type_idx=get_thing_type(thing);*/
/*          if ((type_idx==THING_TYPE_ITEM)) */
          {
            int pos_h=(unsigned short)get_thing_subtile_h(thing);
            /*int subt_x=(unsigned short)get_thing_subtpos_x(thing);*/
            /*int subt_y=(unsigned short)get_thing_subtpos_y(thing);*/
            unsigned short slab=get_tile_slab(lvl,i/MAP_SUBNUM_X,j/MAP_SUBNUM_Y);
            int col_h=get_subtile_column_height(lvl,i,j);
            /*Checking if we're close to sibling column - if we are, its heigh may be */
            /* importand too. (disabled after verifying if official editor) */
/*            if ((subt_x<=64)||(subt_x>=192)||
                (subt_y<=64)||(subt_y>=192))
            {
              int modx=0;
              int mody=0;
              if (subt_x<=64) modx=1;
              if (subt_x>=192) modx=-1;
              if (subt_y<=64) mody=1;
              if (subt_y>=192) mody=-1;
              int mod_h=get_subtile_column_height(lvl,i+modx,j+mody);
              if (mod_h<col_h)
                col_h=mod_h;
            }*/
            if (((pos_h<col_h)&&(!slab_is_door(slab)))||(pos_h<min(col_h,1)))
            {
              errpt->x=i/MAP_SUBNUM_X;
              errpt->y=j/MAP_SUBNUM_Y;
              sprintf(err_msg,"Thing trapped in solid column on slab %d,%d (h=%d<%d).",errpt->x,errpt->y,pos_h,col_h);
              return VERIF_WARN;
            }
          }
        }
        
      }
    }

    /*Array for storing players heart count */
    int hearts[PLAYERS_COUNT];
    for (i=0; i < PLAYERS_COUNT; i++)
      hearts[i]=0;
    owned_things_count(hearts,lvl,THING_TYPE_ITEM,ITEM_SUBTYPE_DNHEART);

    if (hearts[PLAYER_UNSET]>0)
    {
        errpt->x=-1;errpt->y=-1;
        sprintf(err_msg,"Found %d unowned dungeon heart things.",hearts[PLAYER_UNSET]);
        return VERIF_WARN;
    }
    for (i=0; i < PLAYERS_COUNT; i++)
    {
      if ((hearts[i]>1)&&((lvl->optns.verify_warn_flags&VWFLAG_NOWARN_MANYHEART)==0))
      {
        errpt->x=-1;errpt->y=-1;
        sprintf(err_msg,"Player %d owns %d dungeon heart things.",i, hearts[i]);
        return VERIF_WARN;
      }
    }
    if (hearts[0]==0)
    {
        errpt->x=-1;errpt->y=-1;
        sprintf(err_msg,"Human player doesn't have a dungeon heart thing.");
        return VERIF_WARN;
    }
  return VERIF_OK;
}

/**
 * Fills SLB/OWN structure with "default" background.
 * The default is unowned "def_slab" surrounded by rock.
 * DAT/CLM values are not updated here.
 * @param lvl Pointer to the LEVEL structure.
 * @param def_slab Default slab.
 */
void generate_slab_bkgnd_default(struct LEVEL *lvl,unsigned short def_slab)
{
    const struct UPOINT_2D tl_maxindex={lvl->tlsize.x-1,lvl->tlsize.y-1};
    /* Filling the map with def_slab */
    int i,j;
    for (i=1; i < tl_maxindex.y; i++)
      for (j=1; j < tl_maxindex.x; j++)
      {
          set_tile_slab(lvl,i,j,def_slab);
          set_tile_owner(lvl,i,j,PLAYER_UNSET); /*=5, simply ;) */
      }
    /* Impenetrable rock */
    for (i=0; i < lvl->tlsize.x; i++)
    {
      set_tile_slab(lvl,i,0,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,i,tl_maxindex.y,SLAB_TYPE_ROCK);
      set_tile_owner(lvl,i,0,PLAYER_UNSET);
      set_tile_owner(lvl,i,tl_maxindex.y,PLAYER_UNSET);
    }
    for (i=0; i < lvl->tlsize.y; i++)
    {
      set_tile_slab(lvl,0,i,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,tl_maxindex.x,i,SLAB_TYPE_ROCK);
      set_tile_owner(lvl,0,i,PLAYER_UNSET);
      set_tile_owner(lvl,tl_maxindex.x,i,PLAYER_UNSET);
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

/**
 * Fills SLB/OWN structure with "random" background.
 * The resulting map is made of earth with random rock at borders.
 * DAT/CLM values are not updated here.
 * @param lvl Pointer to the LEVEL structure.
 */
void generate_slab_bkgnd_random(struct LEVEL *lvl)
{
/* TODO: check if there are not closed regions of earth, link them with */
/* earth corridors */
/*HOW: */
/*1. Create array lvl->tlsize.x x lvl->tlsize.y of unsigned short, maybe call it "slab_group"? */
/*2. Find an earth slab, start at map center, check at larger radius (rectangle) until found */
/*3. Mark the found slab as group 1 */
/*4. From the marked slab, extend the radius and mark slabs which have */
/*   a neightbour alreadey marked; repeat until map edge reached */
/* Note: use only 4 base directions for checking neightbour slabs */
/*5. Sweep through whole map and mark slabs which have */
/*   a neightbour alreadey marked; count the slabs marked in every sweep */
/*6. Finish sweeping when count=0 */
/*7. Sweep through whole map searching for unmarked earth slab. If found, */
/*   mark it with last_group+1 (=2 at first sweep) */
/*8. Repeat 5. and 6. */
/*9. Repeat 7 and 8 until no unmarked earth slab found */
/*10. Create array of unsigned int with size=last_group+1. Fill it with lvl->tlsize.x's */
/*11. Sweep through all slabs, and for every which has index>1 find the shortest way */
/*    to slab from group with smaller index (use one of four basic directions only). */
/*    Write it in array. */
/*12. Sweep throufg all slabs again and fill the shortest ways to slabs with dirt/water/lava */
/*13. Free the two allocaded arrays */
    int i,j,k,l;
    /* Filling the map with SLAB_TYPE_EARTH */
    const struct UPOINT_2D tl_maxindex={lvl->tlsize.x-1,lvl->tlsize.y-1};
    for (i=1; i < tl_maxindex.y; i++)
      for (j=1; j < tl_maxindex.x; j++)
          set_tile_slab(lvl,i,j,SLAB_TYPE_EARTH); /* Digable dirt */
    /* Impenetrable rock */
    for (i=0; i < lvl->tlsize.x; i++)
    {
      set_tile_slab(lvl,i,0,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,i,tl_maxindex.y,SLAB_TYPE_ROCK);
    }
    for (i=0; i < lvl->tlsize.y; i++)
    {
      set_tile_slab(lvl,0,i,SLAB_TYPE_ROCK);
      set_tile_slab(lvl,tl_maxindex.x,i,SLAB_TYPE_ROCK);
    }

    /*Resetting the last column */
    int sx, sy;
    sx=lvl->subsize.x-1;
    for (sy=0; sy<lvl->subsize.y; sy++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);
    sy=lvl->subsize.y-1;
    for (sx=0; sx<lvl->subsize.x; sx++)
      set_subtl_owner(lvl,sx,sy,PLAYER_UNSET);

    /* Impenetrable rock at borders */
    for (i=1; i < (tl_maxindex.y>>1); i++)
      for (j=0; j < tl_maxindex.x-1; j++)
      {
        /*int ir=tl_maxindex.y-i;*/
        int rnd_bound = rng_rand_max() / ((i/2)+1);
        if (rng_rand()<rnd_bound)
        {
          set_tile_slab(lvl,j,i,SLAB_TYPE_ROCK);
        }
        if (rng_rand()<rnd_bound)
        {
          set_tile_slab(lvl,j,tl_maxindex.y-i,SLAB_TYPE_ROCK);
        }
        if (rng_rand()<rnd_bound)
        {
          set_tile_slab(lvl,i,j,SLAB_TYPE_ROCK);
        }
        if (rng_rand()<rnd_bound)
        {
          set_tile_slab(lvl,tl_maxindex.x-i,j,SLAB_TYPE_ROCK);
        }
      }
    int num_smears=(rng_rand()%20);
    if (num_smears<10)
    {
      while (num_smears>4) num_smears=num_smears>>1;
      for (l=0;l<num_smears;l++)
      {
        int val=rng_rand();
        int smr_startx = val%lvl->tlsize.x;
        int smr_starty=(val>>8)%lvl->tlsize.y;
        val=rng_rand();
        int smr_endx = (val)%lvl->tlsize.x;
        int smr_endy=(val>>8)%lvl->tlsize.y;
        val=rng_rand();
        int startr=(val)%4+2;
        int endr=(val>>8)%3+1;
        val=rng_rand();
        int distance=ceil(sqrt((smr_startx-smr_endx)*(smr_startx-smr_endx)+(smr_starty-smr_endy)*(smr_starty-smr_endy)));
        int bend=((val)%(distance+1))-(distance>>1);
        slab_draw_smear(lvl,smr_startx,smr_starty,startr,smr_endx,smr_endy,endr,bend,SLAB_TYPE_ROCK);
      }
    }
    /*Deleting small rocks and enlarging big rocks */
    const int slabgrow_bound_val=4;
    int cx,cy;
    for (i=1; i < (lvl->tlsize.x>>1); i++)
      for (j=1; j < (lvl->tlsize.y>>1); j++)
        for (k=0; k < 4; k++)
        {
            if ((k%2)==0) cx=i; else cx=tl_maxindex.x-i;
            if ((k>>1)==0) cy=j; else cy=tl_maxindex.y-j;
            int nsibln=slab_siblings_oftype(lvl,cx,cy,SLAB_TYPE_ROCK);
            if (nsibln>slabgrow_bound_val)
            {
              set_tile_slab(lvl,cx,cy,SLAB_TYPE_ROCK);
            } else
            {
              set_tile_slab(lvl,cx,cy,SLAB_TYPE_EARTH);
            }
        }
    for (i=1; i < tl_maxindex.x-1; i++)
      for (j=1; j < tl_maxindex.y-1; j++)
      {
          int nsibln=slab_siblings_oftype(lvl,i,j,SLAB_TYPE_ROCK);
          if (nsibln<2)
          {
              set_tile_slab(lvl,i,j,SLAB_TYPE_EARTH);
          }
          if (nsibln>7)
          {
              set_tile_slab(lvl,i,j,SLAB_TYPE_ROCK);
          }
      }
    /*Everything generated here should be unclaimed */
    for (i=0; i < lvl->tlsize.x; i++)
      for (j=0; j < lvl->tlsize.y; j++)
          set_tile_owner(lvl, i, j, PLAYER_UNSET);
}

/**
 * Creates new level. Requies the memory to be allocated by level_init().
 * Calls level_clear(), but not level_free() at start.
 * @param lvl Pointer to the LEVEL structure.
 */
void start_new_map(struct LEVEL *lvl)
{
    message_log(" start_new_map: starting");
    level_clear(lvl);
    /*Preparing array bounds */
    /*int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;*/
    /*int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;*/

    message_log(" start_new_map: generating background");
    generate_slab_bkgnd_default(lvl,SLAB_TYPE_EARTH);

    message_log(" start_new_map: updating columns");
    /* CLM should be empty, add permanent entries */
    add_permanent_columns(lvl);
    /*And update all DAT/CLM values; it also updates the WIB values. */
    if (lvl->optns.datclm_auto_update)
        update_datclm_for_whole_map(lvl);

    lvl->inf=0x00;

    message_log(" start_new_map: updating level stats");
    update_level_stats(lvl);
    message_log(" start_new_map: finished");
}

/**
 * Creates random level. Requies the memory to be allocated by level_init().
 * Calls level_clear(), but not level_free() at start.
 * @param lvl Pointer to the LEVEL structure.
 */
void generate_random_map(struct LEVEL *lvl)
{
    level_clear(lvl);
    /*Preparing array bounds */
    /*int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;*/
    /*int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;*/

    generate_slab_bkgnd_random(lvl);

    /* CLM should be empty, add permanent entries */
    add_permanent_columns(lvl);
    /*And update all DAT/CLM values; it also updates the WIB values. */
    if (lvl->optns.datclm_auto_update)
        update_datclm_for_whole_map(lvl);
    lvl->inf=rnd(8);
    update_level_stats(lvl);
}

/**
 * Frees the level data stored in LEVEL structure.
 * Then clears LEVEL allowing to reuse it.
 * @param lvl Pointer to the LEVEL structure.
 */
void free_map(struct LEVEL *lvl)
{
    level_free(lvl);
    level_clear(lvl);
}

/**
 * Adds random extension to the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param ret_msg Text message returned by function.
 * @todo This one is unfinished.
 */
short level_generate_random_extension(struct LEVEL *lvl,char *ret_msg)
{
  short result;
  const struct IPOINT_2D heart_size={3,3};
  unsigned char rown;
  /* Trying to place Dungeon Heart */
  /* TODO */
  rown=PLAYER0;
  if (true)
  {
      result=place_room_rndpos(lvl,SLAB_TYPE_DUNGHEART,rown,&heart_size);
      if (result)
      {
        sprintf(ret_msg,"Dungeon Heart placed.");
        return true;
      }
  }
  sprintf(ret_msg,"Can't find anything to add.");
  return false;
}

/**
 * Returns thing data for thing at given position.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the thing is.
 * @param num Index of thing on the subtile.
 * @return Returns the thing data, or NULL on error.
 */
char *get_thing(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    unsigned char *thing=NULL;
    if (num < lvl->tng_subnums[sx][sy])
      thing = lvl->tng_lookup[sx][sy][num];
    return thing;
}

/**
 * Adds a thing to the structure and returns its index.
 * Also updates statistics.
 * @param lvl Pointer to the LEVEL structure.
 * @param thing Pointer to the thing data to add.
 * @return Returns the index at which thing is added, or -1 on error.
 */
int thing_add(struct LEVEL *lvl,unsigned char *thing)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_thing_subtile_x(thing)%arr_entries_x;
    y = get_thing_subtile_y(thing)%arr_entries_y;
    lvl->tng_total_count++;
    /*setting TNG entries */
    lvl->tng_subnums[x][y]++;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->tng_lookup[x][y]==NULL)
      lvl->tng_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->tng_lookup[x][y]=(unsigned char **)realloc(lvl->tng_lookup[x][y],
                          lvl->tng_subnums[x][y]*sizeof(char *));
    if (lvl->tng_lookup[x][y]==NULL)
    {
        message_error("thing_add: Cannot alloc tng entry");
        return -1;
    }
    int new_idx=lvl->tng_subnums[x][y]-1;
    lvl->tng_lookup[x][y][new_idx]=thing;
    update_thing_stats(lvl,thing,1);
    return new_idx;
}

/**
 * Removes given thing from the LEVEL structure, updates counter variables.
 * Also frees memory allocated for the thing.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the thing is.
 * @param num Index of thing on the subtile.
 */
void thing_del(struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned int num)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y)) return;
    unsigned char *thing;
    thing = lvl->tng_lookup[sx][sy][num];
    thing_drop(lvl,sx,sy,num);
    free(thing);
}

/**
 * Removes given thing from the LEVEL structure, updates counter variables.
 * Does not frees memory allocated for the thing - just drops the pointers.
 * Updates thing statistics.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the thing is.
 * @param num Index of thing on the subtile.
 */
void thing_drop(struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned int num)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y)) return;
    int i;
    if (num >= lvl->tng_subnums[sx][sy])
      return;
    lvl->tng_total_count--;
    unsigned char *thing;
    thing = lvl->tng_lookup[sx][sy][num];
    update_thing_stats(lvl,thing,-1);
    for (i=num; i < lvl->tng_subnums[sx][sy]-1; i++)
      lvl->tng_lookup[sx][sy][i]=lvl->tng_lookup[sx][sy][i+1];
    lvl->tng_subnums[sx][sy]--;
    lvl->tng_apt_lgt_nums[sx/3][sy/3]--;
    lvl->tng_lookup[sx][sy]=(unsigned char **)realloc(lvl->tng_lookup[sx][sy], 
                        lvl->tng_subnums[sx][sy]*sizeof(char *));
}

/**
 * Gives amount of things existing at given subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which we're counting thing.
 * @return Amount of things on the subtile.
 */
unsigned int get_thing_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y)) return 0;
    return lvl->tng_subnums[sx][sy];
}

/**
 * Returns action point data for action point at given position.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the action point is.
 * @param num Index of action point on the subtile.
 * @return Returns the action point data, or NULL on error.
 */
char *get_actnpt(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    unsigned char *actnpt=NULL;
    if (num < get_actnpt_subnums(lvl,sx,sy))
      actnpt = lvl->apt_lookup[sx][sy][num];
    return actnpt;
}

/**
 * Adds a given action point to the LEVEL structure.
 * Updates counter variables.
 * @param lvl Pointer to the LEVEL structure.
 * @param actnpt Pointer to the action point data to add.
 * @return Returns the index at which action point is added, or -1 on error.
 */
int actnpt_add(struct LEVEL *lvl,unsigned char *actnpt)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_actnpt_subtile_x(actnpt)%arr_entries_x;
    y = get_actnpt_subtile_y(actnpt)%arr_entries_y;
    lvl->apt_total_count++;
    /*setting APT entries */
    unsigned int apt_snum=get_actnpt_subnums(lvl,x,y);
    apt_snum++;
    lvl->apt_subnums[x][y]=apt_snum;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->apt_lookup[x][y]==NULL)
      lvl->apt_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->apt_lookup[x][y]=(unsigned char **)realloc(lvl->apt_lookup[x][y],
                          apt_snum*sizeof(char *));
    if (lvl->apt_lookup[x][y]==NULL)
    {
        message_error("actnpt_add: Cannot allocate memory");
        return -1;
    }
    unsigned int new_idx=apt_snum-1;
    lvl->apt_lookup[x][y][new_idx]=actnpt;
    return new_idx;
}

/**
 * Removes given action point from the LEVEL structure. Updates counter variables.
 * Also frees memory allocated for the action point.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the action point is.
 * @param num Index of action point on the subtile.
 */
void actnpt_del(struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned int num)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;
    unsigned int apt_snum=get_actnpt_subnums(lvl,sx,sy);
    if (num >= apt_snum)
      return;
    lvl->apt_total_count--;
    unsigned char *actnpt;
    actnpt = lvl->apt_lookup[sx][sy][num];
    free(actnpt);
    int i;
    apt_snum--;
    for (i=num; i < apt_snum; i++)
      lvl->apt_lookup[sx][sy][i]=lvl->apt_lookup[sx][sy][i+1];
    lvl->apt_subnums[sx][sy]=apt_snum;
    lvl->tng_apt_lgt_nums[sx/MAP_SUBNUM_X][sy/MAP_SUBNUM_Y]--;
    lvl->apt_lookup[sx][sy]=(unsigned char **)realloc(lvl->apt_lookup[sx][sy], 
                        apt_snum*sizeof(char *));
}

/**
 * Gives amount of action points existing at given subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which we're counting action points.
 * @return Amount of action points on the subtile.
 */
unsigned int get_actnpt_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    return lvl->apt_subnums[sx][sy];
}

/**
 * Returns static light data for light at given position.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the static light is.
 * @param num Index of static light on the subtile.
 * @return Returns the static light data, or NULL on error.
 */
char *get_stlight(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    unsigned char *stlight=NULL;
    if (num < lvl->lgt_subnums[sx][sy])
      stlight = lvl->lgt_lookup[sx][sy][num];
    return stlight;
}

/**
 * Adds a given static light to the LEVEL structure. Updates counter variables.
 * @param lvl Pointer to the LEVEL structure.
 * @param stlight Pointer to the static light data to add.
 * @return Returns the index at which static light is added, or -1 on error.
 */
int stlight_add(struct LEVEL *lvl,unsigned char *stlight)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    unsigned int x, y;
    x = get_stlight_subtile_x(stlight)%arr_entries_x;
    y = get_stlight_subtile_y(stlight)%arr_entries_y;
    lvl->lgt_total_count++;
    /*setting LGT entries */
    unsigned int lgt_snum=lvl->lgt_subnums[x][y];
    lgt_snum++;
    lvl->lgt_subnums[x][y]=lgt_snum;
    lvl->tng_apt_lgt_nums[(x/MAP_SUBNUM_X)][(y/MAP_SUBNUM_Y)]++;
    if (lvl->lgt_lookup[x][y]==NULL)
      lvl->lgt_lookup[x][y]=(unsigned char **)malloc(sizeof(char *));
    else
      lvl->lgt_lookup[x][y]=(unsigned char **)realloc(lvl->lgt_lookup[x][y],
                          lgt_snum*sizeof(char *));
    if (lvl->lgt_lookup[x][y]==NULL)
    {
        message_error("stlight_add: Cannot allocate memory");
        return -1;
    }
    unsigned int new_idx=lgt_snum-1;
    lvl->lgt_lookup[x][y][new_idx]=stlight;
    return new_idx;
}

/**
 * Removes given static light from the LEVEL structure. Updates counter variables.
 * Also frees memory allocated for the static light.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the static light is.
 * @param num Index of static light on the subtile.
 */
void stlight_del(struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned int num)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    sx%=arr_entries_x;
    sy%=arr_entries_y;
    unsigned int lgt_snum=lvl->lgt_subnums[sx][sy];
    if (num >= lgt_snum)
      return;
    lvl->lgt_total_count--;
    /* commented due to -Wunused-but-set-variable
    unsigned char *stlight;
    stlight = lvl->lgt_lookup[sx][sy][num];*/
    free(lvl->lgt_lookup[sx][sy][num]);
    int i;
    lgt_snum--;
    for (i=num; i < lgt_snum; i++)
      lvl->lgt_lookup[sx][sy][i]=lvl->lgt_lookup[sx][sy][i+1];
    lvl->lgt_subnums[sx][sy]=lgt_snum;
    lvl->tng_apt_lgt_nums[sx/MAP_SUBNUM_X][sy/MAP_SUBNUM_Y]--;
    lvl->lgt_lookup[sx][sy]=(unsigned char **)realloc(lvl->lgt_lookup[sx][sy], 
                        lgt_snum*sizeof(char *));
}

/**
 * Gives amount of static lights existing at given subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which we're counting static lights.
 * @return Amount of static lights on the subtile.
 */
unsigned int get_stlight_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    sx %= arr_entries_x;
    sy %= arr_entries_y;
    return lvl->lgt_subnums[sx][sy];
}

/**
 * Checks what type the object is. Objects are action points, things or lights.
 * This function merges things, action points and static lights,
 * treating them all just as 'objects'.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the object is.
 * @param z Index of the object. Things have lowest indices, then comes
 *     action points and static lights.
 * @return Returns one of OBJECT_TYPE_* value.
 */
short get_object_type(const struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned int z)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding indices */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y))
      return OBJECT_TYPE_NONE;
    int tng_num=lvl->tng_subnums[sx][sy];
    if (z<tng_num) return OBJECT_TYPE_THING;
    int apt_num=get_actnpt_subnums(lvl,sx,sy);
    if (z<(tng_num+apt_num)) return OBJECT_TYPE_ACTNPT;
    int lgt_num=lvl->lgt_subnums[sx][sy];
    if (z<(tng_num+apt_num+lgt_num)) return OBJECT_TYPE_STLIGHT;
    return OBJECT_TYPE_NONE;
}

/**
 * Returns data for specific object at given position.
 * This function merges things, action points and static lights,
 * treating them all just as 'objects'.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the object is.
 * @param z Index of the object. Things have lowest indices, then comes
 *     action points and static lights.
 * @return Returns the object data, or NULL on error.
 */
unsigned char *get_object(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding indices */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y))
      return NULL;
    int tng_num=lvl->tng_subnums[sx][sy];
    if (z<tng_num)
      return get_thing(lvl,sx,sy,z);
    int apt_num=get_actnpt_subnums(lvl,sx,sy);
    if (z<(tng_num+apt_num))
      return get_actnpt(lvl,sx,sy,z-tng_num);
    int lgt_num=lvl->lgt_subnums[sx][sy];
    if (z<(tng_num+apt_num+lgt_num))
      return get_stlight(lvl,sx,sy,z-tng_num-apt_num);
    return NULL;
}

/**
 * Deletes object with given coordinates and index.
 * This function merges things, action points and static lights,
 * treating them all just as 'objects'.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the object is.
 * @param z Index of the object. Things have lowest indices, then comes
 *     action points and static lights.
 */
void object_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding indices */
    sx%=arr_entries_x;
    sy%=arr_entries_y;
    int tng_num=lvl->tng_subnums[sx][sy];
    if (z<tng_num)
    {
      thing_del(lvl,sx,sy,z);
      return;
    }
    int apt_num=get_actnpt_subnums(lvl,sx,sy);
    if (z<(tng_num+apt_num))
    {
      actnpt_del(lvl,sx,sy,z-tng_num);
      return;
    }
    int lgt_num=lvl->lgt_subnums[sx][sy];
    if (z<(tng_num+apt_num+lgt_num))
    {
      stlight_del(lvl,sx,sy,z-tng_num-apt_num);
      return;
    }
}

/**
 * Gives amount of objects existing at given subtile.
 * This function merges things, action points and static lights,
 * treating them all just as 'objects'.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which we're counting objects.
 * @return Amount of objects on the subtile.
 */
unsigned int get_object_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y))
      return 0;
    return lvl->tng_subnums[sx][sy]+get_actnpt_subnums(lvl,sx,sy)+lvl->lgt_subnums[sx][sy];
}

/**
 * Gives amount of objects existing at given tile.
 * This function merges things, action points and static lights,
 * treating them all just as 'objects'.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Tile at which we're counting objects.
 * @return Amount of objects on the tile.
 */
unsigned int get_object_tilnums(const struct LEVEL *lvl,unsigned int tx,unsigned int ty)
{
    if (lvl->tng_apt_lgt_nums==NULL) return 0;
    return lvl->tng_apt_lgt_nums[tx%lvl->tlsize.x][ty%lvl->tlsize.y];
}

/**
 * Gives index of the last object with given type.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Subtile at which the object is.
 * @param obj_type Type of the object, one of OBJECT_TYPE_* values.
 * @return Returns index of the last object with given type.
 *     May return -1 if no object found.
 */
int get_object_subtl_last(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,short obj_type)
{
    /*Preparing array bounds */
    unsigned int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    unsigned int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Bounding position */
    if ((sx>=arr_entries_x)||(sy>=arr_entries_y))
      return 0;
    int last=-1;
    last+=lvl->tng_subnums[sx][sy];
    if (obj_type==OBJECT_TYPE_THING)
      return last;
    last+=get_actnpt_subnums(lvl,sx,sy);
    if (obj_type==OBJECT_TYPE_ACTNPT)
      return last;
    last+=lvl->lgt_subnums[sx][sy];
    if (obj_type==OBJECT_TYPE_STLIGHT)
      return last;
    return get_object_subnums(lvl,sx,sy)-1;
}

/**
 * Gives WIBble value for a subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want the WIB value.
 * @return Returns WIB value.
 */
short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    /*Bounding position */
    sx %= lvl->subsize.x;
    sy %= lvl->subsize.y;
    return lvl->wib[sx][sy];
}

/**
 * Sets WIBble value for a subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want to set WIB value.
 * @param nval New value for the subtile wibble.
 */
void set_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy, short nval)
{
    /*Bounding position */
    if ((sx>=lvl->subsize.x)||(sy>=lvl->subsize.y))
        return;
    lvl->wib[sx][sy]=nval;
}

/**
 * Gives WLB (Water-Lava Block) value for a tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want the WLB value.
 * @return Returns WLB value.
 */
short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    /*Bounding position */
    tx %= lvl->tlsize.x;
    ty %= lvl->tlsize.y;
    return lvl->wlb[tx][ty];
}

/**
 * Sets WLB (Water-Lava Block) value for a tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want to set WLB value.
 * @param nval New value for the tile WLB.
 */
void set_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty, short nval)
{
    /*Bounding position */
    if ((tx>=lvl->tlsize.x)||(ty>=lvl->tlsize.y)) return;
    lvl->wlb[tx][ty]=nval;
}

/**
 * Gives owner (OWN value) for a subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want the owner value.
 * @return Returns subtile owner index.
 */
unsigned char get_subtl_owner(const struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    /*Bounding position */
    sx %= lvl->subsize.x;
    sy %= lvl->subsize.y;
    return lvl->own[sx][sy];
}

/**
 * Sets owner (OWN value) for a subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want to set owner value.
 * @param nval New value for the subtile owner.
 */
void set_subtl_owner(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char nval)
{
    /*Bounding position */
    if ((sx>=lvl->subsize.x)||(sy>=lvl->subsize.y)) return;
    lvl->own[sx][sy]=nval;
}

/**
 * Gives owner (OWN value) for a tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want the owner value.
 * @return Returns owner index for subtile at centre of given tile.
 */
unsigned char get_tile_owner(const struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    return get_subtl_owner(lvl,tx*MAP_SUBNUM_X+1,ty*MAP_SUBNUM_Y+1);
}

/**
 * Sets owner (OWN value) for a tile. Sets given owner value for
 * all subtiles on given tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want to set owner value.
 * @param nval New value for the tile owner.
 */
void set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned char nval)
{
    int subtl_x,subtl_y;
    for (subtl_y=0;subtl_y<MAP_SUBNUM_Y;subtl_y++)
      for (subtl_x=0;subtl_x<MAP_SUBNUM_X;subtl_x++)
        set_subtl_owner(lvl,tx*MAP_SUBNUM_X+subtl_x,ty*MAP_SUBNUM_Y+subtl_y,nval);
}

/**
 * Gives slab (SLB value) for a tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want the slab value.
 * @return Returns tile slab value.
 */
unsigned short get_tile_slab(const struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    if (lvl->slb==NULL) return SLAB_TYPE_ROCK;
    /*Bounding position */
    if ((tx>=lvl->tlsize.x)||(ty>=lvl->tlsize.y)) return SLAB_TYPE_ROCK;
    return lvl->slb[tx][ty];
}

/**
 * Sets a new value to SLB item, without updating DAT or CLM or anything.
 * To update level graphics after putting slab, use user_set_slab() instead.
 * @see user_set_slab
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want to set slab value.
 * @param nval New value for the tile slab.
 */
void set_tile_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval)
{
    /*Bounding position */
    if ((tx>=lvl->tlsize.x)||(ty>=lvl->tlsize.y)) return;
    lvl->slb[tx][ty]=nval;
}

/**
 * Gives extended slab value for a tile.
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want the slab value.
 * @return Returns tileset number
 */
short get_slx_tileset(const struct LEVEL *lvl, unsigned int tx, unsigned int ty)
{
    if (lvl->slb==NULL) return 0;
    /*Bounding position */
    if ( (tx >= lvl->tlsize.x) || (ty >= lvl->tlsize.y) ) return 0;
    return (lvl->slx_data[tx + ty * lvl->tlsize.x] & 0x0F);
}

/**
 * Sets a new tileset for a slab
 * @see user_set_slab
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map tile at which we want to set slab value.
 * @param nval New tileset value
 */
void set_slx_tileset(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval)
{
    /*Bounding position */
    if ( (tx >= lvl->tlsize.x) || (ty >= lvl->tlsize.y) ) return;
    lvl->slx_data[tx + ty * lvl->tlsize.x] &= 0xF0;
    lvl->slx_data[tx + ty * lvl->tlsize.x] |= nval & 0x0F;
}

/**
 * Returns raw DAT value for one subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want the DAT value.
 * @return Returns raw DAT value.
 */
unsigned int get_dat_val(const struct LEVEL *lvl, const unsigned int sx, const unsigned int sy)
{
    if (lvl->dat==NULL) return 0;
    if ((sx>=lvl->subsize.x)||(sy>=lvl->subsize.y)) return 0;
    return lvl->dat[sx][sy];
}

/**
 * Sets a DAT value for one subtile.
 * Low level - sets the RAW value, not column index.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want to set DAT value.
 * @param d Raw DAT value to set.
 */
void set_dat_val(struct LEVEL *lvl, int sx, int sy, unsigned int d)
{
    if (lvl->dat==NULL) return;
    if ((sx<0)||(sy<0)||(sx>=lvl->subsize.x)||(sy>=lvl->subsize.y)) return;
    lvl->dat[sx][sy]=d;
}

/**
 * Returns FLG value for one subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want the FLG value.
 * @return Returns FLG value.
 */
unsigned short get_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy)
{
    if (lvl->flg==NULL) return 0;
    if ((sx>=lvl->subsize.x)||(sy>=lvl->subsize.y)) return 0;
    return lvl->flg[sx][sy];
}

/**
 * Sets a FLG value for one subtile.
 * @param lvl Pointer to the LEVEL structure.
 * @param sx,sy Map subtile at which we want to set FLG value.
 * @param nval FLG value to set.
 */
void set_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned short nval)
{
    if (lvl->flg==NULL) return;
    if ((sx>=lvl->subsize.x)||(sy>=lvl->subsize.y)) return;
    lvl->flg[sx][sy]=nval;
}

/**
 * Updates some statistics about the level. The update includes
 * "utilize" values of columns.
 * @param lvl Pointer to the LEVEL structure.
 */
void update_level_stats(struct LEVEL *lvl)
{
     update_clm_utilize_counters(lvl);
     update_things_stats(lvl);
}

/**
 * Updates statistics about the level. Does not updates
 * "utilize" values of columns.
 * @param lvl Pointer to the LEVEL structure.
 */
void update_things_stats(struct LEVEL *lvl)
{
    /*Preparing array bounds */
    int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    /*Clearing previous stats */
    level_clear_stats(lvl);
    /*Sweeping through structures */
    int i, j, k;
    for (i=0; i < arr_entries_y; i++)
    {
      for (j=0; j < arr_entries_x; j++)
      {
        int things_count=get_thing_subnums(lvl,i,j);
        for (k=0; k <things_count ; k++)
        {
          unsigned char *thing = get_thing(lvl,i,j,k);
          update_thing_stats(lvl,thing,1);
        }
      }
    }
}

/**
 * Updates statistics about the thing.
 * @param lvl Pointer to the LEVEL structure.
 * @param thing Pointer to the thing data.
 * @param change How the amount of such things have changes.
 *     Positive if the thing was added, negative if removed.
 */
void update_thing_stats(struct LEVEL *lvl,const unsigned char *thing,short change)
{
          if (thing==NULL) return;
          unsigned char type_idx=get_thing_type(thing);
          switch (type_idx)
          {
          case THING_TYPE_CREATURE:
              lvl->stats.creatures_count+=change;
              break;
          case THING_TYPE_EFFECTGEN:
              lvl->stats.effectgenrts_count+=change;
              break;
          case THING_TYPE_TRAP:
              lvl->stats.traps_count+=change;
              break;
          case THING_TYPE_DOOR:
              lvl->stats.doors_count+=change;
              break;
          case THING_TYPE_ITEM:
              lvl->stats.items_count+=change;
              break;
          }
          if (is_herogate(thing))
              lvl->stats.hero_gates_count+=change;
          if (is_dnheart(thing))
              lvl->stats.dn_hearts_count+=change;

          int categr=get_thing_subtypes_arridx(thing);
          if (categr<THING_CATEGR_COUNT)
            lvl->stats.things_count[categr]+=change;

          if (is_room_inventory(thing))
              lvl->stats.room_things_count+=change;
          if (change>0)
              lvl->stats.things_added+=change;
          else
              lvl->stats.things_removed-=change;
}

/**
 * Returns total number of static lights on the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns total number of static lights.
 */
unsigned int get_lgt_total_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->lgt_total_count;
}

/**
 * Returns total number of action points on the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns total number of action points.
 */
unsigned int get_apt_total_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->apt_total_count;
}

/**
 * Returns total number of things on the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns total number of things.
 */
unsigned int get_tng_total_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->tng_total_count;
}

/**
 * Returns state of the datclm_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns state of datclm_auto_update.
 */
short get_datclm_auto_update(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    return lvl->optns.datclm_auto_update;
}

/**
 * Switches state of the datclm_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns new state of datclm_auto_update.
 */
short switch_datclm_auto_update(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    short nval=!(lvl->optns.datclm_auto_update);
    lvl->optns.datclm_auto_update=nval;
    return nval;
}

/**
 * Sets state of the datclm_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param val New state of the option.
 * @return Returns true if datclm_auto_update was successfully changed.
 */
short set_datclm_auto_update(struct LEVEL *lvl,short val)
{
    if (lvl==NULL) return false;
    lvl->optns.datclm_auto_update=val;
    return true;
}

/**
 * Returns state of the obj_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns state of obj_auto_update.
 */
short get_obj_auto_update(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    return lvl->optns.obj_auto_update;
}

/**
 * Switches state of the obj_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns new state of obj_auto_update.
 */
short switch_obj_auto_update(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    short nval=!(lvl->optns.obj_auto_update);
    lvl->optns.obj_auto_update=nval;
    return nval;
}

/**
 * Sets state of the obj_auto_update option for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param val New state of the option.
 * @return Returns true if obj_auto_update was successfully changed.
 */
short set_obj_auto_update(struct LEVEL *lvl,short val)
{
    if (lvl==NULL) return false;
    lvl->optns.obj_auto_update=val;
    return true;
}

/**
 * Sets map file name for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname The new file name, which is copied to the level.
 * @return Returns true on success, false on error.
 */
short set_lvl_fname(struct LEVEL *lvl,char *fname)
{
    if (lvl==NULL) return false;
    if (fname == NULL)
    {
      free(lvl->fname);
      lvl->fname=NULL;
    }
    free(lvl->fname);
    lvl->fname=strdup(fname);
    if (lvl->fname==NULL)
      return false;
    return true;
}

/**
 * Sets map file name for the level, using given format string.
 * The string may be map file name, or map number.
 * @param lvl Pointer to the LEVEL structure.
 * @param namefmt The new file name, which is copied to the level.
 * @return Returns true on success, false on error.
 */
short format_lvl_fname(struct LEVEL *lvl,char *namefmt)
{
    if (lvl==NULL) return false;
    if (namefmt == NULL)
    {
      free(lvl->fname);
      lvl->fname=NULL;
    }
    free(lvl->fname);
    lvl->fname=(char *)malloc(DISKPATH_SIZE*sizeof(char));
    if (lvl->fname==NULL)
      return false;
    return format_map_fname(lvl->fname,namefmt,lvl->optns.levels_path);
}

/**
 * Returns map file name for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns map file name, or "" on error.
 */
char *get_lvl_fname(struct LEVEL *lvl)
{
    if (lvl==NULL) return "";
    if (lvl->fname == NULL)
      return "";
    return lvl->fname;
}

/**
 * Sets map file name for saving the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname The new file name to save on.
 * @return Returns true on success, false on error.
 */
short set_lvl_savfname(struct LEVEL *lvl,char *fname)
{
    if (lvl==NULL) return false;
    if (fname == NULL)
    {
      free(lvl->savfname);
      lvl->savfname=NULL;
    }
    free(lvl->savfname);
    lvl->savfname=strdup(fname);
    if (lvl->savfname==NULL)
      return false;
    return true;
}

/**
 * Sets map save file name for the level, using given format string.
 * The string may be map file name, or map number.
 * @param lvl Pointer to the LEVEL structure.
 * @param namefmt The save file name, which is copied to the level.
 * @return Returns true on success, false on error.
 */
short format_lvl_savfname(struct LEVEL *lvl,char *namefmt)
{
    if (lvl==NULL) return false;
    if (namefmt == NULL)
    {
      free(lvl->savfname);
      lvl->savfname=NULL;
    }
    free(lvl->savfname);
    lvl->savfname=(char *)malloc(DISKPATH_SIZE*sizeof(char));
    if (lvl->savfname==NULL)
      return false;
    return format_map_fname(lvl->savfname,namefmt,lvl->optns.levels_path);
}

/**
 * Returns save file name for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns save file name, or "" on error.
 */
char *get_lvl_savfname(struct LEVEL *lvl)
{
    if (lvl==NULL) return "";
    if (lvl->savfname == NULL)
      return "";
    return lvl->savfname;
}

/**
 * Returns text name of the level, from LIF file.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns text name of the level, or "" on error.
 */
char *get_lif_name_text(struct LEVEL *lvl)
{
    if (lvl==NULL) return "";
    if (lvl->info.name_text == NULL)
      return "";
    return lvl->info.name_text;
}

/**
 * Sets text name of the level, from LIF file.
 * Directly sets the name pointer, without duplicating it.
 * @param lvl Pointer to the LEVEL structure.
 * @param name New text name of the level.
 * @return Returns true on success, false on error.
 */
short set_lif_name_text(struct LEVEL *lvl,char *name)
{
    if (lvl==NULL) return false;
    free(lvl->info.name_text);
    lvl->info.name_text=name;
    return true;
}

/**
 * Sets path at which maps are. The path should be without ending slash,
 * and will be used only if map name to load/save doesn't have its own path.
 * @param lvl Pointer to the LEVEL structure.
 * @param lvpath The new path for levels folder.
 * @return Returns true on success, false on error.
 */
short set_levels_path(struct LEVEL *lvl,char *lvpath)
{
    if (lvl==NULL) return false;
    if (lvpath == NULL)
    {
      free(lvl->optns.levels_path);
      lvl->optns.levels_path=NULL;
      return true;
    }
    free(lvl->optns.levels_path);
    lvl->optns.levels_path=strdup(lvpath);
    if (lvl->optns.levels_path==NULL)
      return false;
    return true;
}

/**
 * Returns path to folder with levels.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns path text, or NULL on error.
 */
char *get_levels_path(struct LEVEL *lvl)
{
    if (lvl==NULL) return NULL;
    return lvl->optns.levels_path;
}

/**
 * Sets path at which data files are. The path should be without ending slash,
 * and will be used only for graphics related commands.
 * @param lvl Pointer to the LEVEL structure.
 * @param datpath The new path for data files folder.
 * @return Returns true on success, false on error.
 */
short set_data_path(struct LEVEL *lvl,char *datpath)
{
    if (lvl==NULL) return false;
    if (datpath == NULL)
    {
      free(lvl->optns.data_path);
      lvl->optns.data_path=NULL;
      lvl->optns.picture.data_path=lvl->optns.data_path;
      return true;
    }
    free(lvl->optns.data_path);
    lvl->optns.data_path=strdup(datpath);
    lvl->optns.picture.data_path=lvl->optns.data_path;
    if (lvl->optns.data_path==NULL)
      return false;
    return true;
}

/**
 * Returns path to folder with data files.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns path text, or NULL on error.
 */
char *get_data_path(struct LEVEL *lvl)
{
    if (lvl==NULL) return NULL;
    return lvl->optns.data_path;
}

/**
 * Returns pointer to the level script structure.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns pointer to DK_SCRIPT, or NULL on error.
 */
struct DK_SCRIPT *get_lvl_script(struct LEVEL *lvl)
{
    if (lvl==NULL) return NULL;
    return &(lvl->script);
}

/**
 * Returns pointer to the level statistics structure.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns pointer to LEVSTATS, or NULL on error.
 */
struct LEVSTATS *get_lvl_stats(struct LEVEL *lvl)
{
    if (lvl==NULL) return NULL;
    return &(lvl->stats);
}

/**
 * Increases mode switches counter.
 * Used for user commands statistics for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current mode switches count.
 */
unsigned long inc_info_usr_mdswtch_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.usr_mdswtch_count++;
    return lvl->info.usr_mdswtch_count;
}

/**
 * Increases SLB changes counter.
 * Used for user commands statistics for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current SLB changes count.
 */
unsigned long inc_info_usr_slbchng_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.usr_slbchng_count++;
    return lvl->info.usr_slbchng_count;
}

/**
 * Increases total user commands counter.
 * Used for user commands statistics for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current user commands count.
 */
unsigned long inc_info_usr_cmds_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.usr_cmds_count++;
    return lvl->info.usr_cmds_count;
}

/**
 * Increases user objects created counter.
 * Used for user commands statistics for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current objects created count.
 */
unsigned long inc_info_usr_creatobj_count(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.usr_creatobj_count++;
    return lvl->info.usr_creatobj_count;
}

/**
 * Increases major version number for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current major version number.
 */
unsigned int inc_info_ver_major(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.ver_major++;
    lvl->info.ver_minor=0;
    lvl->info.ver_rel=0;
    return lvl->info.ver_major;
}

/**
 * Increases minor version number for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current minor version number.
 */
unsigned int inc_info_ver_minor(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.ver_minor++;
    lvl->info.ver_rel=0;
    return lvl->info.ver_minor;
}

/**
 * Increases version release number for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns the current version release number.
 */
unsigned int inc_info_ver_rel(struct LEVEL *lvl)
{
    if (lvl==NULL) return false;
    lvl->info.ver_rel++;
    return lvl->info.ver_rel++;
}

/**
 * Returns INF entry (texture index) for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns INF entry for the level.
 */
unsigned char get_lvl_inf(struct LEVEL *lvl)
{
    if (lvl==NULL) return 0;
    return lvl->inf;
}

/**
 * Returns level format version. The format version was used
 * when creating the LEVEL structure with level_init().
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns format version for the level.
 */
short get_lvl_format_version(struct LEVEL *lvl)
{
    if (lvl==NULL) return MFV_DKSTD;
    return lvl->format_version;
}

/**
 * Sets INF entry (texture index) for the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param ninf New INF entry for the level.
 * @return Returns true on success, false on error.
 */
short set_lvl_inf(struct LEVEL *lvl,unsigned char ninf)
{
    if (lvl==NULL) return false;
    lvl->inf=ninf;
    return true;
}

/**
 * Formats object statistics text line.
 * @param lvl Pointer to the LEVEL structure.
 * @param stat_buf Destination text buffer.
 * @param line_num Line number to retrieve.
 * @return Returns type of the line.
 */
short get_level_objstats_textln(struct LEVEL *lvl,char *stat_buf,const int line_num)
{
    struct LEVSTATS *stats=get_lvl_stats(lvl);
    switch (line_num)
    {
    case 0:
        sprintf(stat_buf,"Map objects statistics");
        return STLT_HEADER;
    case 1:
        sprintf(stat_buf,"Static lights:%4d",get_lgt_total_count(lvl));
        return STLT_MAINITEM;
    case 2:
        sprintf(stat_buf,"Graffiti:%5d",get_graffiti_count(lvl));
        return STLT_MAINITEM;
    case 3:
        sprintf(stat_buf,"Action points:%4d",get_apt_total_count(lvl));
        return STLT_MAINITEM;
    case 4:
        sprintf(stat_buf,"Cust.clms:%4d",get_cust_clm_count(lvl));
        return STLT_MAINITEM;
    case 5:
        sprintf(stat_buf,"Things on map:%4d",get_tng_total_count(lvl));
        return STLT_HEADER;
    case 6:
        sprintf(stat_buf,"Creatures:%6d",stats->creatures_count);
        return STLT_SUBITEM;
    case 7:
        sprintf(stat_buf,"Traps:%4d",stats->traps_count);
        return STLT_SUBITEM;
    case 8:
        sprintf(stat_buf,"EffctGenrts:%4d",stats->effectgenrts_count);
        return STLT_SUBITEM;
    case 9:
        sprintf(stat_buf,"Doors:%4d",stats->doors_count);
        return STLT_SUBITEM;
    case 10:
        sprintf(stat_buf,"Room things:%4d",stats->room_things_count);
        return STLT_SUBITEM;
    case 11:
        sprintf(stat_buf,"Items:%4d",stats->items_count);
        return STLT_SUBITEM;
    case 12:
        sprintf(stat_buf,"Detailed items");
        return STLT_HEADER;
    case 13:
        sprintf(stat_buf,"Dung hearts:%4d",stats->dn_hearts_count);
        return STLT_SUBITEM;
    case 14:
        sprintf(stat_buf,"Hero gates:%3d",stats->hero_gates_count);
        return STLT_SUBITEM;
    case 15:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_ITEMEFFCT),
          stats->things_count[THING_CATEGR_ITEMEFFCT]);
        return STLT_SUBITEM;
    case 16:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_CREATLAIR),
          stats->things_count[THING_CATEGR_CREATLAIR]);
        return STLT_SUBITEM;
    case 17:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_SPECIALBOX),
          stats->things_count[THING_CATEGR_SPECIALBOX]);
        return STLT_SUBITEM;
    case 18:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_SPELLBOOK),
          stats->things_count[THING_CATEGR_SPELLBOOK]);
        return STLT_SUBITEM;
    case 19:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_WRKSHOPBOX),
          stats->things_count[THING_CATEGR_WRKSHOPBOX]);
        return STLT_SUBITEM;
    case 20:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_SPINNTNG),
          stats->things_count[THING_CATEGR_SPINNTNG]);
        return STLT_SUBITEM;
    case 21:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_FOOD),
          stats->things_count[THING_CATEGR_FOOD]);
        return STLT_SUBITEM;
    case 22:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_GOLD),
          stats->things_count[THING_CATEGR_GOLD]);
        return STLT_SUBITEM;
    case 23:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_TORCHCNDL),
          stats->things_count[THING_CATEGR_TORCHCNDL]);
        return STLT_SUBITEM;
    case 24:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_HEARTFLAME),
          stats->things_count[THING_CATEGR_HEARTFLAME]);
        return STLT_SUBITEM;
    case 25:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_POLEBAR),
          stats->things_count[THING_CATEGR_POLEBAR]);
        return STLT_SUBITEM;
    case 26:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_STATUE),
          stats->things_count[THING_CATEGR_STATUE]);
        return STLT_SUBITEM;
    case 27:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_FURNITURE),
          stats->things_count[THING_CATEGR_FURNITURE]);
        return STLT_SUBITEM;
    case 28:
        sprintf(stat_buf,"%9s:%4d",get_thing_category_shortname(THING_CATEGR_ROOMEQUIP),
          stats->things_count[THING_CATEGR_ROOMEQUIP]);
        return STLT_SUBITEM;
    case 29:
        sprintf(stat_buf,"%9s:%6d",get_thing_category_shortname(THING_CATEGR_PWHAND),
          stats->things_count[THING_CATEGR_PWHAND]);
        return STLT_SUBITEM;
    default:
        stat_buf[0]='\0';
        return STLT_NONE;
    }
}

/**
 * Puts a new slab on map. Updates level graphics, things and statistics.
 * @see set_tile_slab
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map coordinates of the slab to change.
 * @param nslab New value for the tile slab.
 * @return Returns true on success, false on error.
 */
short user_set_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nslab)
{
  /* Put the slab value */
  set_tile_slab(lvl,tx,ty,nslab);
  /* Update user commands statistics */
  inc_info_usr_slbchng_count(lvl);
  /* Update the level graphics */
  if (get_obj_auto_update(lvl))
      update_obj_for_square_radius1(lvl,tx,ty);
  if (get_datclm_auto_update(lvl))
      update_datclm_for_square_radius1(lvl,tx,ty);
  if (get_obj_auto_update(lvl))
      update_obj_subpos_and_height_for_square_radius1(lvl,tx,ty);
  return true;
}

/**
 * Changes owner of one slab on map. Updates level graphics, things and statistics.
 * @see set_tile_owner
 * @param lvl Pointer to the LEVEL structure.
 * @param tx,ty Map coordinates of the owner to change.
 * @param nown New value for the tile owner.
 * @return Returns true on success, false on error.
 */
short user_set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nown)
{
  /* Put the slab value */
  set_tile_owner(lvl,tx,ty,nown);
  /* Update user commands statistics */
  inc_info_usr_slbchng_count(lvl);
  /* Update the level graphics */
  if (get_obj_auto_update(lvl))
      update_obj_for_square_radius1(lvl,tx,ty);
  if (get_datclm_auto_update(lvl))
      update_datclm_for_square_radius1(lvl,tx,ty);
  if (get_obj_auto_update(lvl))
      update_obj_subpos_and_height_for_square_radius1(lvl,tx,ty);
  return true;
}

/**
 * Puts a rectangle of new slabs on map. Updates level graphics, things and statistics.
 * @param lvl Pointer to the LEVEL structure.
 * @param startx,starty Top left of the changed rectangle.
 * @param endx,endy Bottom right of the changed rectangle.
 * @param nslab New value for the slab to fill rectangle.
 * @return Returns true on success, false on error.
 */
short user_set_slab_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nslab)
{
    /* Sanity check */
    if ((endx>=lvl->tlsize.x) || (endy>=lvl->tlsize.y) ||
       (startx>endx) || (starty>endy) )
    {
      message_error("Map coordinates out of bounds");
      return false;
    }
    int tile_x,tile_y;
    for (tile_x=startx; tile_x<=endx; tile_x++)
      for (tile_y=starty; tile_y<=endy; tile_y++)
      {
/*          unsigned char oldslb; */
/*          oldslb = get_tile_slab(lvl,tile_x,tile_y); */
          set_tile_slab(lvl,tile_x,tile_y,nslab);
          inc_info_usr_slbchng_count(lvl);
      }
    if (get_obj_auto_update(lvl))
      update_obj_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_datclm_auto_update(lvl))
      update_datclm_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_obj_auto_update(lvl))
      update_obj_subpos_and_height_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    return true;
}

/**
 * Changes owner of a map rectangle. Updates level graphics, things and statistics.
 * @param lvl Pointer to the LEVEL structure.
 * @param startx,starty Top left of the changed rectangle.
 * @param endx,endy Bottom right of the changed rectangle.
 * @param nown New value for the rectangle owner.
 * @return Returns true on success, false on error.
 */
short user_set_owner_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nown)
{
    /* Sanity check */
    if ((endx>=lvl->tlsize.x) || (endy>=lvl->tlsize.y) ||
       (startx>endx) || (starty>endy) )
    {
      message_error("Map coordinates out of bounds");
      return false;
    }
    int tile_x,tile_y;
    for (tile_x=startx; tile_x<=endx; tile_x++)
      for (tile_y=starty; tile_y<=endy; tile_y++)
      {
          set_tile_owner(lvl,tile_x,tile_y,nown);
          inc_info_usr_slbchng_count(lvl);
      }
    if (get_obj_auto_update(lvl))
      update_obj_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_datclm_auto_update(lvl))
      update_datclm_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_obj_auto_update(lvl))
      update_obj_subpos_and_height_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    return true;
}

/**
 * Puts a rectangle of new slabs on map. Updates level graphics, things and statistics.
 * @param lvl Pointer to the LEVEL structure.
 * @param startx,starty Top left of the changed rectangle.
 * @param endx,endy Bottom right of the changed rectangle.
 * @param nslab New value for the slab to put in rectangle.
 * @param nown New value for the rectangle owner.
 * @return Returns true on success, false on error.
 */
short user_set_slabown_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nslab,unsigned short nown)
{
    /* Sanity check */
    if ((endx>=lvl->tlsize.x) || (endy>=lvl->tlsize.y) ||
       (startx>endx) || (starty>endy) )
    {
      message_error("Map coordinates out of bounds");
      return false;
    }
    int tile_x,tile_y;
    for (tile_x=startx; tile_x<=endx; tile_x++)
      for (tile_y=starty; tile_y<=endy; tile_y++)
      {
          set_tile_slab(lvl,tile_x,tile_y,nslab);
          set_tile_owner(lvl,tile_x,tile_y,nown);
          inc_info_usr_slbchng_count(lvl);
      }
    if (get_obj_auto_update(lvl))
      update_obj_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_datclm_auto_update(lvl))
      update_datclm_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    if (get_obj_auto_update(lvl))
      update_obj_subpos_and_height_for_square(lvl, startx-1, endx+1, starty-1, endy+1);
    return true;
}
