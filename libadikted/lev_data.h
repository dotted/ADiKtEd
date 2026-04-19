/******************************************************************************/
/** @file lev_data.h
 * Struct definitions and LEVEL structure handling routines.
 * @par Purpose:
 *     Header file. Defines exported routines from lev_data.c
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

#ifndef ADIKT_LEVDATA_H
#define ADIKT_LEVDATA_H

#include "globals.h"

/* Map size definitions */

#define MAP_SIZE_DKSTD_X 85
#define MAP_SIZE_DKSTD_Y 85
#define MAX_MAP_SIZE_DKXPAND_X 500
#define MAX_MAP_SIZE_DKXPAND_Y 500
#define MAP_SIZE_H 1
#define MAP_SUBNUM_H 8
#define MAP_SUBNUM_X 3
#define MAP_SUBNUM_Y 3
#define COLUMN_ENTRIES 2048

/**
 * Map format type selection.
 * Do not use values other than MFV_DKSTD and MFV_DKGOLD,
 * unless you know what you're doing!
 */
enum MAP_FORMAT_VERSION {
    MFV_DKSTD           =  0, /* Standard Dungeon Keeper 1 map, for first version of DK */
    MFV_DKGOLD          =  1, /* Dungeon Keeper Gold or Deeper Dungeons map */
    MFV_DKXPAND         =  2, /* KeeperFX or NBKe map */
    MFV_DK2STD          = 10, /* Standard Dungeon Keeper 2 map, for versions up to 1.3 */
    MFV_DK2OFC          = 11, /* Dungeon Keeper 2 v 1.7 map, same as for Official Editor */
     };

enum OBJECT_TYPE_INDEX {
    OBJECT_TYPE_NONE    =  0,
    OBJECT_TYPE_ACTNPT  =  1,
    OBJECT_TYPE_THING   =  2,
    OBJECT_TYPE_STLIGHT =  3,
    OBJECT_TYPE_COLUMN  = 16,
    OBJECT_TYPE_SLAB    = 17,
    OBJECT_TYPE_DATLST  = 18,
     };

enum VERIFY_WARN_FLAGS {
     VWFLAG_NONE              =  0,
     VWFLAG_NOWARN_MANYHEART  =  1,
/*     VWFLAG_NOWARN_           =  2, */
     };

/* Select level verification steps to skip (bit fields) */
enum VERIFY_SKIP_FLAGS {
    VSF_STRUCT      = 0x01,
    VSF_THINGS      = VSF_STRUCT    << 1,
    VSF_SLABS       = VSF_THINGS    << 1,
    VSF_ACTNPNTS    = VSF_SLABS     << 1,
    VSF_COLUMNS     = VSF_ACTNPNTS  << 1,
    VSF_DAT         = VSF_COLUMNS   << 1,
    VSF_TXT         = VSF_DAT       << 1,
    VSF_LOGIC       = VSF_TXT       << 1
};

/*Disk files entries */

#define SIZEOF_DK_TNG_REC 21
#define SIZEOF_DK_CLM_REC 24
#define SIZEOF_DK_APT_REC 8
#define SIZEOF_DK_LGT_REC 20

#define SIZEOF_DK_CLM_HEADER 8
#define SIZEOF_DK_APT_HEADER 4
#define SIZEOF_DK_LGT_HEADER 4
#define SIZEOF_DK_TNG_HEADER 2

/**
 * Direction indices. Indices in the 9-element arrays,
 * like slab columns or surround variable.
 */
enum DIRECTION_INDEX {
    IDIR_CENTR  = 4,
    IDIR_NW     = 0,
    IDIR_NORTH  = 1,
    IDIR_NE     = 2,
    IDIR_EAST   = 5,
    IDIR_SE     = 8,
    IDIR_SOUTH  = 7,
    IDIR_SW     = 6,
    IDIR_WEST   = 3,
     };
/* Now explaining the constants as elements of an array: */
/*       IDIR_NW     IDIR_NORTH    IDIR_NE */
/*       IDIR_WEST   IDIR_CENTR    IDIR_EAST */
/*       IDIR_SW     IDIR_SOUTH    IDIR_SE */

/**
 * Orientation - for graffiti.
 */
enum GRAFFITI_ORIENT {
    ORIENT_NS       = 0x00,
    ORIENT_WE       = 0x01,
    ORIENT_SN       = 0x02,
    ORIENT_EW       = 0x03,
    ORIENT_TNS      = 0x04,
    ORIENT_TWE      = 0x05,
    ORIENT_TSN      = 0x06,
    ORIENT_TEW      = 0x07,
    };

extern const int idir_subtl_x[];
extern const int idir_subtl_y[];

/**
 * Font - for graffiti.
 */
enum GRAFFITI_FONT {
    GRAFF_FONT_NONE      = 0x00,
    GRAFF_FONT_ADICLSSC  = 0x01,
    GRAFF_FONT_ADISIZE8  = 0x02,
    };

/**
 * Statistics line types. For functions returning statistic texts.
 */
enum STATS_LINE_TYPE {
    STLT_NONE     = 0,
    STLT_HEADER   = 1,
    STLT_MAINITEM = 2,
    STLT_SUBITEM  = 3,
    STLT_EMPTY    = 4,
     };

/**
 * Graffiti structure. Stores a graffiti item.
 */
struct DK_GRAFFITI {
    struct IPOINT_2D tile;
    char *text;
    unsigned short font;
    unsigned short orient;
    int height;
    struct IPOINT_2D fin_tile;
    unsigned short cube;
  };

/**
 * Script player structure. Stores player information retrieved from script.
 */
struct DK_SCRIPT_PLAYER {
    unsigned int max_creatures;
    unsigned long start_gold;
    int computer_player;
    unsigned short *ally;
    unsigned short *creature_avail;
    int *creature_maxlvl; /* Max level; -1 means unlimited */
    unsigned short *room_avail;
    unsigned short *spell_avail;
    unsigned short *trap_avail;
    unsigned short *door_avail;
    unsigned int *trap_amount;
    unsigned int *door_amount;
};

/**
 * Script party member structure.
 * Stores party member information retrieved from script.
 */
struct DK_SCRIPT_PARTYMEMBER {
    short type;
    unsigned short level;
    unsigned long gold;
    unsigned short objctiv;
    unsigned long cntdown;
  };

/**
 * Script party structure.
 * Stores party information retrieved from script.
 */
struct DK_SCRIPT_PARTY {
    char *name;
    struct DK_SCRIPT_PARTYMEMBER *member;
    unsigned short count;
  };

/**
 * Script add event structure.
 * Stores add event information retrieved from script.
 */
struct DK_SCRIPT_ADDEVENT {
    unsigned short owner;
    unsigned short type; /*CREATURE/PARTY/TUNNELLER/TUNNELLER_PARTY */
    struct DK_SCRIPT_PARTYMEMBER leader; /*type<0 means he's not there */
    int actnpt;         /*where the creature/party is added */
    char *partyname;    /*should be NULL for CREATURE and TUNNELLER */
    unsigned short num; /*number of copies of creature/party */
  };

/**
 * Script parameters structure.
 * Stores analyzed script information. Can be created using script,
 * and can be converted back into script.
 */
struct DK_SCRIPT_PARAMETERS {
       /* Single values */
    unsigned long portal_gen_speed;
    short dead_return_to_pool;
    int end_level; /*This should be 0 if all loops are closed properly */
       /* Arrays with indices PLAYER0..PLAYER_UNSET */
    struct DK_SCRIPT_PLAYER *player;
       /* Other constant-size arrays */
    unsigned int *creature_pool;
       /* Dynamic structures */
    struct DK_SCRIPT_ADDEVENT *addevent;
    unsigned int addevent_count;
    struct DK_SCRIPT_PARTY *party;
    unsigned int party_count;
    /*TODO - action points/hero gates code */
    /* All others - unrecognized commands */
    struct DK_SCRIPT_COMMAND **rest;
    int rest_count;
  };

/**
 * Complete level script structure.
 * Stores both raw and analyzed script data.
 */
struct DK_SCRIPT {
    struct DK_SCRIPT_COMMAND **list;
    struct DK_SCRIPT_PARAMETERS par;
    char **txt;  /* The whole script stored as txt */
    int lines_count;
};

/**
 * Level statistics structure.
 * Stats are re-computed on every load and other operations.
 */
struct LEVSTATS {
    /*Things strats */
    int creatures_count;
    int effectgenrts_count;
    int traps_count;
    int doors_count;
    int items_count;
    /*Items stats */
    int hero_gates_count;
    int dn_hearts_count;
    int things_count[THING_CATEGR_COUNT];
    /*Various stats */
    int room_things_count;
    /*Stats on objects adding/removal */
    int things_removed;
    int things_added;
    /* Number of save operations in this session */
    int saves_count;
    /* Number of unsaved changes */
    int unsaved_changes;
  };

/**
 * Level information structure.
 * Info are not re-computed on load, unless the ADI script or LIF file is missing.
 */
struct LEVINFO {
    /* Adikted commands executed by user */
    unsigned long usr_cmds_count;
    unsigned long usr_mdswtch_count;
    unsigned long usr_slbchng_count;
    unsigned long usr_creatobj_count;
    /* Map creation date */
    time_t creat_date;
    time_t lastsav_date;
    /* Map version */
    int ver_major;
    int ver_minor;
    int ver_rel;
    /*text name of the level, and other text data */
    char *name_text;
    char *desc_text;
    char *author_text;
    char *editor_text; /* name of the person who last edited the level */
  };

/**
 * The main Level data structure.
 * Stores all elements of Dungeon Keeper level, including data for
 * ADiKted's special functions.
 */
struct LEVEL {
    /* Map version, from MAP_FORMAT_VERSION enumeration */
    short format_version;
    /* Level size, in tiles */
    struct UPOINT_2D tlsize;
    /* Level size, in subtiles */
    struct UPOINT_2D subsize;
    /*map file name (for loading) */
    char *fname;
    /*map file name (for saving) */
    char *savfname;
    /*Slab file - tile type definitions, size MAP_SIZE_Y x MAP_SIZE_X */
    unsigned short **slb;
    /*Owners file - subtile owner index, size arr_entries_y+1 x arr_entries_x+1 */
    unsigned char **own;
    /*Vibration file - subtile animation indices, size arr_entries_y+1 x arr_entries_x+1 */
    unsigned char **wib;
    /*WLB file - some additional info about water and lava tiles, */
    /* size MAP_SIZE_Y x MAP_SIZE_X, not always present */
    unsigned char **wlb;
    /*Flag file - size arr_entries_y+1 x arr_entries_x+1 */
    unsigned short **flg;
    /*Column file - constant-size array of entries used for displaying tiles, */
    /* size COLUMN_ENTRIES x SIZEOF_DK_CLM_REC */
    unsigned char **clm;
    /*How many DAT entries points at every column */
    unsigned int *clm_utilize;
    /*Column file header */
    unsigned char *clm_hdr;
    /*Texture information file - one byte file, identifies texture pack index */
    unsigned char inf;
    /*Script text - a text file containing level parameters as editable script; */
    /* number of lines and file size totally variable */
    struct DK_SCRIPT script;

    /*our objects - apt, tng and lgt */

    unsigned char ****apt_lookup; /* Index to action points, by subtile */
    unsigned short **apt_subnums; /* Number of action points in a subtile */
    unsigned int apt_total_count; /* Total number of action points */

    unsigned char ****tng_lookup; /* Index to things, by subtile */
    unsigned short **tng_subnums; /* Number of things in a subtile */
    unsigned int tng_total_count; /* Number of things in total */

    /*Light file - contains static light definitions */
    unsigned char ****lgt_lookup; /* Index to static light, by subtile */
    unsigned short **lgt_subnums; /* Number of static lights in a subtile */
    unsigned int lgt_total_count; /* Total number of static lights */

    unsigned short **tng_apt_lgt_nums;    /* Number of all objects in a tile */

    /* DAT file contains indices of columns for each subtile */
    /* Its content stores a graphic for whole map */
    /* Size arr_entries_y+1 x arr_entries_x+1 (there is single rock column at end) */
    unsigned short **dat;

    /* Elements that are not part of DK levels, but are importand for Adikted */
    /* Level statistics */
    struct LEVSTATS stats;
    /* Level information */
    struct LEVINFO info;
    /* Options, which affects level graphic generation, and other stuff */
    struct LEVOPTIONS optns;
    /* Custom columns definition */
    /* There can be only one custom column on each subtile. */
    /* The lookup array size is arr_entries_y+1 x arr_entries_x+1 */
    struct DK_CUSTOM_CLM ***cust_clm_lookup; /* Index to cust.columns, by subtile */
    unsigned int cust_clm_count;
    struct DK_GRAFFITI **graffiti;
    unsigned int graffiti_count;

    unsigned char slx_data[MAX_MAP_SIZE_DKXPAND_X * MAX_MAP_SIZE_DKXPAND_Y];
  };

extern const char default_map_name[];

/* creates object for storing map */
DLLIMPORT short level_init(struct LEVEL **lvl_ptr,short map_version,struct UPOINT_3D *lvl_size);
/* frees object for storing map */
DLLIMPORT short level_deinit(struct LEVEL **lvl_ptr);
DLLIMPORT short level_set_options(struct LEVEL *lvl,struct LEVOPTIONS *optns);
DLLIMPORT struct LEVOPTIONS *level_get_options(struct LEVEL *lvl);
DLLIMPORT short level_set_mapdraw_options(struct LEVEL *lvl,struct MAPDRAW_OPTIONS *mdrwopts);
DLLIMPORT struct MAPDRAW_OPTIONS *level_get_mapdraw_options(struct LEVEL *lvl);

DLLIMPORT short level_clear(struct LEVEL *lvl);
short level_clear_tng(struct LEVEL *lvl);
short level_clear_apt(struct LEVEL *lvl);
short level_clear_lgt(struct LEVEL *lvl);
short level_clear_datclm(struct LEVEL *lvl);
short level_clear_other(struct LEVEL *lvl);
short level_clear_stats(struct LEVEL *lvl);
short level_clear_info(struct LEVEL *lvl);
short level_clear_script(struct LEVEL *lvl);
DLLIMPORT short level_clear_script_param(struct DK_SCRIPT_PARAMETERS *par);
DLLIMPORT short level_clear_options(struct LEVOPTIONS *optns);

DLLIMPORT short level_free(struct LEVEL *lvl);
short level_free_tng(struct LEVEL *lvl);
DLLIMPORT short level_free_script_param(struct DK_SCRIPT_PARAMETERS *par);
DLLIMPORT short free_text_file(char ***lines,int *lines_count);

DLLIMPORT short level_verify(struct LEVEL *lvl, char *actn_name,struct IPOINT_2D *errpt);
DLLIMPORT short level_verify_control(struct LEVEL *lvl, char *actn_name, unsigned long skip_step_flags, struct IPOINT_2D *errpt);
DLLIMPORT short level_verify_struct(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);
short actnpts_verify(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);
DLLIMPORT short level_verify_logic(struct LEVEL *lvl, char *err_msg,struct IPOINT_2D *errpt);
DLLIMPORT void start_new_map(struct LEVEL *lvl);
DLLIMPORT void generate_random_map(struct LEVEL *lvl);
DLLIMPORT void generate_slab_bkgnd_default(struct LEVEL *lvl,unsigned short def_slab);
DLLIMPORT void generate_slab_bkgnd_random(struct LEVEL *lvl);
DLLIMPORT short level_generate_random_extension(struct LEVEL *lvl,char *ret_msg);
DLLIMPORT struct DK_SCRIPT_PARAMETERS *level_get_script_param(struct LEVEL *lvl);

DLLIMPORT void free_map(struct LEVEL *lvl);

DLLIMPORT char *get_thing(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT int thing_add(struct LEVEL *lvl,unsigned char *thing);
DLLIMPORT void thing_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT void thing_drop(struct LEVEL *lvl,unsigned int sx, unsigned int sy, unsigned int num);
DLLIMPORT unsigned int get_thing_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy);

DLLIMPORT char *get_actnpt(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT int actnpt_add(struct LEVEL *lvl,unsigned char *actnpt);
DLLIMPORT void actnpt_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT unsigned int get_actnpt_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy);

DLLIMPORT char *get_stlight(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT int stlight_add(struct LEVEL *lvl,unsigned char *stlight);
DLLIMPORT void stlight_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int num);
DLLIMPORT unsigned int get_stlight_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy);

DLLIMPORT short get_object_type(const struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned int z);
DLLIMPORT unsigned char *get_object(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z);
DLLIMPORT void object_del(struct LEVEL *lvl,unsigned int sx,unsigned int sy,unsigned int z);
DLLIMPORT unsigned int get_object_subnums(const struct LEVEL *lvl,unsigned int sx,unsigned int sy);
DLLIMPORT unsigned int get_object_tilnums(const struct LEVEL *lvl,unsigned int tx,unsigned int ty);
DLLIMPORT int get_object_subtl_last(const struct LEVEL *lvl,unsigned int sx,unsigned int sy,short obj_type);
DLLIMPORT void update_object_owners(struct LEVEL *lvl);

DLLIMPORT unsigned int get_lgt_total_count(struct LEVEL *lvl);
DLLIMPORT unsigned int get_apt_total_count(struct LEVEL *lvl);
DLLIMPORT unsigned int get_tng_total_count(struct LEVEL *lvl);
DLLIMPORT short get_datclm_auto_update(struct LEVEL *lvl);
DLLIMPORT short switch_datclm_auto_update(struct LEVEL *lvl);
DLLIMPORT short set_datclm_auto_update(struct LEVEL *lvl,short val);
DLLIMPORT short get_obj_auto_update(struct LEVEL *lvl);
DLLIMPORT short switch_obj_auto_update(struct LEVEL *lvl);
DLLIMPORT short set_obj_auto_update(struct LEVEL *lvl,short val);

DLLIMPORT short get_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
DLLIMPORT void set_subtl_wib(struct LEVEL *lvl, unsigned int sx, unsigned int sy, short nval);

DLLIMPORT short get_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty);
DLLIMPORT void set_tile_wlb(struct LEVEL *lvl, unsigned int tx, unsigned int ty, short nval);

DLLIMPORT unsigned char get_tile_owner(const struct LEVEL *lvl, unsigned int tx, unsigned int ty);
DLLIMPORT void set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned char nval);
DLLIMPORT unsigned char get_subtl_owner(const struct LEVEL *lvl, unsigned int sx, unsigned int sy);
DLLIMPORT void set_subtl_owner(struct LEVEL *lvl, unsigned int sx, unsigned int sy, unsigned char nval);
DLLIMPORT short user_set_tile_owner(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nown);
DLLIMPORT short user_set_owner_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nown);

DLLIMPORT unsigned short get_tile_slab(const struct LEVEL *lvl, unsigned int tx, unsigned int ty);
DLLIMPORT void set_tile_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval);
DLLIMPORT short user_set_slab(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nslab);
DLLIMPORT short user_set_slab_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nslab);
DLLIMPORT short user_set_slabown_rect(struct LEVEL *lvl, unsigned int startx, unsigned int endx,
    unsigned int starty, unsigned int endy, unsigned short nslab,unsigned short nown);

DLLIMPORT unsigned int get_dat_val(const struct LEVEL *lvl, const unsigned int sx, const unsigned int sy);
DLLIMPORT void set_dat_val(struct LEVEL *lvl, int sx, int sy, unsigned int d);

DLLIMPORT unsigned short get_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy);
DLLIMPORT void set_subtl_flg(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned short nval);

DLLIMPORT short set_lvl_fname(struct LEVEL *lvl,char *fname);
DLLIMPORT short format_lvl_fname(struct LEVEL *lvl,char *namefmt);
DLLIMPORT char *get_lvl_fname(struct LEVEL *lvl);
DLLIMPORT short set_lvl_savfname(struct LEVEL *lvl,char *fname);
DLLIMPORT short format_lvl_savfname(struct LEVEL *lvl,char *namefmt);
DLLIMPORT char *get_lvl_savfname(struct LEVEL *lvl);
DLLIMPORT short set_levels_path(struct LEVEL *lvl,char *lvpath);
DLLIMPORT char *get_levels_path(struct LEVEL *lvl);
DLLIMPORT short set_data_path(struct LEVEL *lvl,char *datpath);
DLLIMPORT char *get_data_path(struct LEVEL *lvl);

DLLIMPORT char *get_lif_name_text(struct LEVEL *lvl);
DLLIMPORT short set_lif_name_text(struct LEVEL *lvl,char *name);

DLLIMPORT struct DK_SCRIPT *get_lvl_script(struct LEVEL *lvl);
DLLIMPORT struct LEVSTATS *get_lvl_stats(struct LEVEL *lvl);

DLLIMPORT unsigned long inc_info_usr_mdswtch_count(struct LEVEL *lvl);
DLLIMPORT unsigned long inc_info_usr_cmds_count(struct LEVEL *lvl);
DLLIMPORT unsigned long inc_info_usr_slbchng_count(struct LEVEL *lvl);
DLLIMPORT unsigned long inc_info_usr_creatobj_count(struct LEVEL *lvl);

DLLIMPORT unsigned int inc_info_ver_major(struct LEVEL *lvl);
DLLIMPORT unsigned int inc_info_ver_minor(struct LEVEL *lvl);
DLLIMPORT unsigned int inc_info_ver_rel(struct LEVEL *lvl);

DLLIMPORT void update_level_stats(struct LEVEL *lvl);
DLLIMPORT void update_things_stats(struct LEVEL *lvl);
DLLIMPORT void update_thing_stats(struct LEVEL *lvl,const unsigned char *thing,short change);
DLLIMPORT short get_level_objstats_textln(struct LEVEL *lvl,char *stat_buf,const int line_num);

DLLIMPORT unsigned char get_lvl_inf(struct LEVEL *lvl);
DLLIMPORT short set_lvl_inf(struct LEVEL *lvl,unsigned char ninf);
DLLIMPORT short get_lvl_format_version(struct LEVEL *lvl);

DLLIMPORT short get_slx_tileset(const struct LEVEL *lvl, unsigned int tx, unsigned int ty);
DLLIMPORT void set_slx_tileset(struct LEVEL *lvl, unsigned int tx, unsigned int ty, unsigned short nval);
#endif /* ADIKT_LEVDATA_H */
