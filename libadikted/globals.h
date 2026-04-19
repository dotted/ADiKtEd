/******************************************************************************/
/** @file globals.h
 * Global configuration file.
 * @par Purpose:
 *     Header file with global definitions for Another Dungeon
 *     Keeper Map Editor.
 * @par Comment:
 *     Defines basic includes and definitions, used in whole program.
 * @author   Jon Skeet, Tomasz Lis
 * @date     08 sep 1998 - 22 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef ADIKT_GLOBALS_H
#define ADIKT_GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#if !defined(PROJECT_TARGETS_WINDOWS) && !defined(MSDOS)
#include <strings.h>
#endif

#if defined(unix) && !defined(GO32)
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(MSDOS)
#include <dos.h>
#include <process.h>
#endif

#if !defined(stricmp)
#if defined(PROJECT_TARGETS_WINDOWS)
#define stricmp _stricmp
#else
#define stricmp strcasecmp
#endif
#endif

#if defined(BUILD_DLL)
# define DLLIMPORT __declspec (dllexport)
#elif defined(BUILD_SINGLE)
# define DLLIMPORT 
#else /* Not defined BUILD_DLL */
# define DLLIMPORT
//# define DLLIMPORT __declspec (dllimport)
#endif

#if defined(USE_FASTCALL)
# define FASTCALL __fastcall
#else
# define FASTCALL
#endif

/* Basic Definitions */

#if defined(unix) && !defined (GO32)
#define SEPARATOR "/"
#else
#define SEPARATOR "\\"
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

/* Buffer sizes */

#define DISKPATH_SIZE   256
#define LINEMSG_SIZE    160
#define READ_BUFSIZE    256

#define PROGRAM_NAME "ADiKtEd"

/* Return values for verification functions */
#define VERIF_ERROR   0
#define VERIF_OK      1
#define VERIF_WARN    2

/* Return values for all other functions */
#define ERR_NONE           0
/**
 *  Note: error codes -1..-79 are reserved standard C library errors with sign reverted.
 * these are defined in errno.h
 */
#define ERR_BASE_RNC      -90


struct IPOINT_2D {
    int x;
    int y;
};

struct IPOINT_3D {
    int x;
    int y;
    int z;
};

struct UPOINT_2D {
    unsigned int x;
    unsigned int y;
};

struct UPOINT_3D {
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

struct USPOINT_2D {
    unsigned short x;
    unsigned short y;
};

struct IRECT_2D {
    int l;
    int r;
    int t;
    int b;
};

struct SCRIPT_OPTIONS {
    int level_spaces;
};

struct MAPDRAW_OPTIONS {
    short rescale;
    short bmfonts;
    short tngflags;
    char *data_path;
};

struct VERIFY_OPTIONS {
    /* Level size, in tiles */
    struct UPOINT_2D tlsize;
    /* Level size, in subtiles */
    struct UPOINT_2D subsize;
    /* Verification error message buffer */
    char err_msg[LINEMSG_SIZE];
};

struct LEVOPTIONS {
    /* Level graphic generation options */
    short unaffected_gems;
    short unaffected_rock;
    short fill_reinforced_corner;
    short frail_columns;
    /* True means DAT/CLM/WIB are updated automatically */
    short datclm_auto_update;
    /* True means TNG/LGT/APTs are updated automatically */
    short obj_auto_update;
    /* File handling variables */
    char *levels_path;
    char *data_path;
    /**
     * True means that APT/TNG will load all objects if file size of TNG/APT
     * is larger than it should
     */
    short load_redundant_objects;
    /* Flags used for level verification */
    unsigned int verify_warn_flags;
    /* Map picture generation options */
    struct MAPDRAW_OPTIONS picture;
    /* Level script options */
    struct SCRIPT_OPTIONS script;
  };

/* Thing categorization helpers */
#define THING_CATEGR_COUNT 0x16
typedef short (*is_thing_subtype)(const unsigned char *thing);
typedef short (*is_item_subtype)(const unsigned char stype_idx);
typedef int (*thing_subtype_arrayindex)(const unsigned char stype_idx);
typedef unsigned char (*thing_subtype_switch)(const unsigned char stype_idx);
typedef unsigned char (*thing_subtype_arrayitem)(const int arr_itm);

#endif /* ADIKT_GLOBALS_H */
