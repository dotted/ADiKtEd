/******************************************************************************/
/** @file obj_column_def.c
 * Dungeon Keeper Tools.
 * @par Purpose:
 *     Functions for creating single CLM entries for various purposes.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     17 Dec 2007
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "obj_column_def.h"

#include "lev_data.h"
#include "globals.h"
#include "obj_cube.h"
#include "obj_slabs.h"
#include "msg_log.h"
#include "lbfileio.h"
#include "bulcommn.h"
#include "arr_utils.h"

const char *cube_fullnames[]={
 "empty cube",          "standard earth 1",    "standard earth 2",    "standard earth 3",   /*000 */
 "earth bright top L",  "earth bright top C",  "earth bright top R",  "earth n/water L",
 "earth n/water R",     "earth n/wtr s.top L", "earth n/wtr s.top C", "earth n/wtr s.top R",/*008 */
 "stone w/earth top 1", "stone w/earth top 2", "white stone",         "stone w/earth top 3",
 "unknown cube 0x0010", "unknown cube 0x0011", "unknown cube 0x0012", "unknown cube 0x0013",/*010 */
 "unknown cube 0x0014", "unknown cube 0x0015", "unknown cube 0x0016", "unknown cube 0x0017",
 "unknown cube 0x0018", "unknown cube 0x0019", "unknown cube 0x001a", "unknown cube 0x001b",/*018 */
 "unknown cube 0x001c", "unknown cube 0x001d", "unknown cube 0x001e", "unknown cube 0x001f",
 "unknown cube 0x0020", "unknown cube 0x0021", "unknown cube 0x0022", "unknown cube 0x0023",/*020 */
 "unknown cube 0x0024", "unknown cube 0x0025", "unknown cube 0x0026", "static water",
 "static dark lava",    "static bright lava",  "unknown cube 0x002a", "unknown cube 0x002b",/*028 */
 "unknown cube 0x002c", "unknown cube 0x002d", "unknown cube 0x002e", "unknown cube 0x002f",
 "unknown cube 0x0030", "unknown cube 0x0031", "unknown cube 0x0032", "unknown cube 0x0033",/*030 */
 "unknown cube 0x0034", "unknown cube 0x0035", "unknown cube 0x0036", "unknown cube 0x0037",
 "unknown cube 0x0038", "unknown cube 0x0039", "unknown cube 0x003a", "unknown cube 0x003b",/*038 */
 "unknown cube 0x003c", "unknown cube 0x003d", "unknown cube 0x003e", "unknown cube 0x003f",
 "unknown cube 0x0040", "unknown cube 0x0041", "unknown cube 0x0042", "unknown cube 0x0043",/*040 */
 "unknown cube 0x0044", "unknown cube 0x0045", "unknown cube 0x0046", "unknown cube 0x0047",
 "unknown cube 0x0048", "unknown cube 0x0049", "unknown cube 0x004a", "unknown cube 0x004b",/*048 */
 "unknown cube 0x004c", "unknown cube 0x004d", "unknown cube 0x004e", "unknown cube 0x004f",
 "unknown cube 0x0050", "unknown cube 0x0051", "unknown cube 0x0052", "unknown cube 0x0053",/*050 */
 "woman wall rlf TL",   "woman wall rlf TC",   "woman wall rlf TR",   "woman wall rlf UL",
 "woman wall rlf UC",   "woman wall rlf UR",   "woman wall rlf ML",   "woman wall rlf MC",  /*058 */
 "woman wall rlf MR",   "woman wall rlf BL",   "woman wall rlf BC",   "woman wall rlf BR",
 "twins wall rlf TL",   "twins wall rlf TC",   "twins wall rlf TR",   "twins wall rlf ML",  /*060 */
 "twins wall rlf MC",   "twins wall rlf MR",   "twins wall rlf BL",   "twins wall rlf BC",
 "twins wall rlf BR",   "pair shagn rlf TL",   "pair shagn rlf TC",   "pair shagn rlf TR",  /*068 */
 "pair shagn rlf UL",   "pair shagn rlf UC",   "pair shagn rlf UR",   "pair shagn rlf ML",
 "pair shagn rlf MC",   "pair shagn rlf MR",   "pair shagn rlf BL",   "pair shagn rlf BC",  /*070 */
 "pair shagn rlf BR",   "unknown cube 0x0075", "unknown cube 0x0076", "unknown cube 0x0077",
 "unknown cube 0x0078", "unknown cube 0x0079", "unknown cube 0x007a", "unknown cube 0x007b",/*078 */
 "unknown cube 0x007c", "unknown cube 0x007d", "unknown cube 0x007e", "unknown cube 0x007f",
 "unknown cube 0x0080", "unknown cube 0x0081", "unknown cube 0x0082", "unknown cube 0x0083",/*080 */
 "unknown cube 0x0084", "unknown cube 0x0085", "unknown cube 0x0086", "unknown cube 0x0087",
 "unknown cube 0x0088", "unknown cube 0x0089", "unknown cube 0x008a", "unknown cube 0x008b",/*088 */
 "unknown cube 0x008c", "unknown cube 0x008d", "unknown cube 0x008e", "unknown cube 0x008f",
 "unknown cube 0x0090", "unknown cube 0x0091", "unknown cube 0x0092", "unknown cube 0x0093",/*090 */
 "unknown cube 0x0094", "unknown cube 0x0095", "unknown cube 0x0096", "unknown cube 0x0097",
 "unknown cube 0x0098", "unknown cube 0x0099", "unknown cube 0x009a", "unknown cube 0x009b",/*098 */
 "unknown cube 0x009c", "unknown cube 0x009d", "unknown cube 0x009e", "unknown cube 0x009f",
 "unknown cube 0x00a0", "unknown cube 0x00a1", "unknown cube 0x00a2", "unknown cube 0x00a3",/*0a0 */
 "unknown cube 0x00a4", "unknown cube 0x00a5", "unknown cube 0x00a6", "unknown cube 0x00a7",
 "unknown cube 0x00a8", "unknown cube 0x00a9", "unknown cube 0x00aa", "unknown cube 0x00ab",/*0a8 */
 "unknown cube 0x00ac", "unknown cube 0x00ad", "unknown cube 0x00ae", "unknown cube 0x00af",
 "unknown cube 0x00b0", "unknown cube 0x00b1", "unknown cube 0x00b2", "unknown cube 0x00b3",/*0b0 */
 "unknown cube 0x00b4", "unknown cube 0x00b5", "unknown cube 0x00b6", "unknown cube 0x00b7",
 "unknown cube 0x00b8", "unknown cube 0x00b9", "unknown cube 0x00ba", "unknown cube 0x00bb",/*0b8 */
 "unknown cube 0x00bc", "unknown cube 0x00bd", "unknown cube 0x00be", "unknown cube 0x00bf",
 "unknown cube 0x00c0", "unknown cube 0x00c1", "unknown cube 0x00c2", "unknown cube 0x00c3",/*0c0 */
 "unknown cube 0x00c4", "unknown cube 0x00c5", "unknown cube 0x00c6", "unknown cube 0x00c7",
 "unknown cube 0x00c8", "unknown cube 0x00c9", "unknown cube 0x00ca", "unknown cube 0x00cb",/*0c8 */
 "unknown cube 0x00cc", "unknown cube 0x00cd", "unknown cube 0x00ce", "unknown cube 0x00cf",
 "unknown cube 0x00d0", "unknown cube 0x00d1", "unknown cube 0x00d2", "unknown cube 0x00d3",/*0d0 */
 "unknown cube 0x00d4", "unknown cube 0x00d5", "unknown cube 0x00d6", "unknown cube 0x00d7",
 "unknown cube 0x00d8", "unknown cube 0x00d9", "unknown cube 0x00da", "unknown cube 0x00db",/*0d8 */
 "unknown cube 0x00dc", "unknown cube 0x00dd", "unknown cube 0x00de", "unknown cube 0x00df",
 "unknown cube 0x00e0", "head shape wall TL",  "head shape wall TC",  "head shape wall TR", /*0e0 */
 "head shape wall BL",  "head shape wall BC",  "head shape wall BR",  "tortr floor w/circl",
 "skel.splat wall TL",  "skel.splat wall TC",  "skel.splat wall TR",  "skel.splat wall ML", /*0e8 */
 "skel.splat wall MC",  "skel.splat wall MR",  "skel.splat wall BL",  "skel.splat wall BC",
 "skel.splat wall BR",  "unknown cube 0x00f1", "unknown cube 0x00f2", "unknown cube 0x00f3",/*0f0 */
 "unknown cube 0x00f4", "unknown cube 0x00f5", "unknown cube 0x00f6", "unknown cube 0x00f7",
 "unknown cube 0x00f8", "unknown cube 0x00f9", "unknown cube 0x00fa", "unknown cube 0x00fb",/*0f8 */
 "unknown cube 0x00fc", "unknown cube 0x00fd", "unknown cube 0x00fe", "unknown cube 0x00ff",
 "unknown cube 0x0100", "unknown cube 0x0101", "unknown cube 0x0102", "unknown cube 0x0103",/*100 */
 "unknown cube 0x0104", "unknown cube 0x0105", "unknown cube 0x0106", "unknown cube 0x0107",
 "unknown cube 0x0108", "unknown cube 0x0109", "unknown cube 0x010a", "unknown cube 0x010b",/*108 */
 "unknown cube 0x010c", "unknown cube 0x010d", "unknown cube 0x010e", "unknown cube 0x010f",
 "unknown cube 0x0110", "unknown cube 0x0111", "unknown cube 0x0112", "unknown cube 0x0113",/*110 */
 "unknown cube 0x0114", "unknown cube 0x0115", "unknown cube 0x0116", "unknown cube 0x0117",
 "unknown cube 0x0118", "unknown cube 0x0119", "unknown cube 0x011a", "unknown cube 0x011b",/*118 */
 "unknown cube 0x011c", "unknown cube 0x011d", "unknown cube 0x011e", "unknown cube 0x011f",
 "unknown cube 0x0120", "unknown cube 0x0121", "unknown cube 0x0122", "unknown cube 0x0123",/*120 */
 "unknown cube 0x0124", "unknown cube 0x0125", "unknown cube 0x0126", "unknown cube 0x0127",
 "unknown cube 0x0128", "unknown cube 0x0129", "unknown cube 0x012a", "unknown cube 0x012b",/*128 */
 "unknown cube 0x012c", "unknown cube 0x012d", "unknown cube 0x012e", "unknown cube 0x012f",
 "unknown cube 0x0130", "unknown cube 0x0131", "unknown cube 0x0132", "unknown cube 0x0133",/*130 */
 "unknown cube 0x0134", "unknown cube 0x0135", "unknown cube 0x0136", "unknown cube 0x0137",
 "unknown cube 0x0138", "unknown cube 0x0139", "unknown cube 0x013a", "unknown cube 0x013b",/*138 */
 "unknown cube 0x013c", "unknown cube 0x013d", "unknown cube 0x013e", "unknown cube 0x013f",
 "unknown cube 0x0140", "unknown cube 0x0141", "unknown cube 0x0142", "unknown cube 0x0143",/*140 */
 "unknown cube 0x0144", "unknown cube 0x0145", "unknown cube 0x0146", "unknown cube 0x0147",
 "unknown cube 0x0148", "unknown cube 0x0149", "unknown cube 0x014a", "unknown cube 0x014b",/*148 */
 "unknown cube 0x014c", "unknown cube 0x014d", "unknown cube 0x014e", "unknown cube 0x014f",
 "unknown cube 0x0150", "unknown cube 0x0151", "unknown cube 0x0152", "unknown cube 0x0153",/*150 */
 "unknown cube 0x0154", "unknown cube 0x0155", "unknown cube 0x0156", "unknown cube 0x0157",
 "unknown cube 0x0158", "unknown cube 0x0159", "unknown cube 0x015a", "unknown cube 0x015b",/*158 */
 "unknown cube 0x015c", "unknown cube 0x015d", "unknown cube 0x015e", "unknown cube 0x015f",
 "unknown cube 0x0160", "unknown cube 0x0161", "unknown cube 0x0162", "wooden door 0x0163",/*160 */
 "wooden door 0x0164",  "wooden door 0x0165",  "wooden door 0x0166",  "wooden door 0x0167",
 "wooden door 0x0168",  "wooden door 0x0169",  "wooden door 0x016a",  "wooden door 0x016b",/*168 */
 "braced door 0x016c",  "braced door 0x016d",  "braced door 0x016e",  "braced door 0x016f",
 "braced door 0x0170",  "braced door 0x0171",  "braced door 0x0172",  "braced door 0x0173",/*170 */
 "braced door 0x0174",  "steel door 0x0175",   "steel door 0x0176",   "steel door 0x0177",
 "steel door 0x0178",   "steel door 0x0179",   "steel door 0x017a",   "steel door 0x017b",/*178 */
 "steel door 0x017c",   "steel door 0x017d",   "unknown cube 0x017e", "unknown cube 0x017f",
 "unknown cube 0x0180", "unknown cube 0x0181", "unknown cube 0x0182", "unknown cube 0x0183",/*180 */
 "unknown cube 0x0184", "unknown cube 0x0185", "unknown cube 0x0186", "unknown cube 0x0187",
 "unknown cube 0x0188", "unknown cube 0x0189", "unknown cube 0x018a", "unknown cube 0x018b",/*188 */
 "unknown cube 0x018c", "unknown cube 0x018d", "unknown cube 0x018e", "unknown cube 0x018f",
 "unknown cube 0x0190", "unknown cube 0x0191", "unknown cube 0x0192", "unknown cube 0x0193",/*190 */
 "unknown cube 0x0194", "unknown cube 0x0195", "unknown cube 0x0196", "unknown cube 0x0197",
 "unknown cube 0x0198", "unknown cube 0x0199", "unknown cube 0x019a", "unknown cube 0x019b",/*198 */
 "unknown cube 0x019c", "unknown cube 0x019d", "unknown cube 0x019e", "unknown cube 0x019f",
 "unknown cube 0x01a0", "unknown cube 0x01a1", "unknown cube 0x01a2", "unknown cube 0x01a3",/*1a0 */
 "unknown cube 0x01a4", "unknown cube 0x01a5", "unknown cube 0x01a6", "unknown cube 0x01a7",
 "unknown cube 0x01a8", "unknown cube 0x01a9", "unknown cube 0x01aa", "blue face flag",     /*1a8 */
 "green face flag",     "yellow face flag",    "unowned flag place",  "white face flag",
 "red face flag stone", "lair floor Cntr",     "fence brick wall T",  "fence brick wall B", /*1b0 */
 "wood floor hole R",   "wood floor hole L",   "wood floor hole C",   "wood floor n/lava",
 "wood floor n/water",  "valuable gems 1",     "valuable gems 2",     "valuable gems 3",    /*1b8 */
 "valuable gems 4",     "empty cube 0x01bd",   "empty cube 0x01be",   "empty cube 0x01bf",
 "empty cube 0x01c0",   "empty cube 0x01c1",   "empty cube 0x01c2",   "empty cube 0x01c3",/*1c0 */
 "empty cube 0x01c4",   "empty cube 0x01c5",   "empty cube 0x01c6",   "empty cube 0x01c7",
 "empty cube 0x01c8",   "empty cube 0x01c9",   "empty cube 0x01ca",   "empty cube 0x01cb",/*1c8 */
 "empty cube 0x01cc",   "empty cube 0x01cd",   "empty cube 0x01ce",   "empty cube 0x01cf",
 "empty cube 0x01d0",   "empty cube 0x01d1",   "empty cube 0x01d2",   "empty cube 0x01d3",/*1d0 */
 "empty cube 0x01d4",   "empty cube 0x01d5",   "empty cube 0x01d6",   "empty cube 0x01d7",
 "empty cube 0x01d8",   "empty cube 0x01d9",   "empty cube 0x01da",   "empty cube 0x01db",/*1d8 */
 "empty cube 0x01dc",   "empty cube 0x01dd",   "empty cube 0x01de",   "empty cube 0x01df",
 "empty cube 0x01e0",   "empty cube 0x01e1",   "empty cube 0x01e2",   "empty cube 0x01e3",/*1e0 */
 "empty cube 0x01e4",   "empty cube 0x01e5",   "empty cube 0x01e6",   "empty cube 0x01e7",
 "empty cube 0x01e8",   "empty cube 0x01e9",   "empty cube 0x01ea",   "empty cube 0x01eb",/*1e8 */
 "empty cube 0x01ec",   "empty cube 0x01ed",   "empty cube 0x01ee",   "empty cube 0x01ef",
 "empty cube 0x01f0",   "empty cube 0x01f1",   "empty cube 0x01f2",   "empty cube 0x01f3",/*1f0 */
 "empty cube 0x01f4",   "empty cube 0x01f5",   "empty cube 0x01f6",   "empty cube 0x01f7",
 "empty cube 0x01f8",   "empty cube 0x01f9",   "empty cube 0x01fa",   "empty cube 0x01fb",/*1f8 */
 "empty cube 0x01fc",   "empty cube 0x01fd",   "empty cube 0x01fe",   "empty cube 0x01ff",
     };

const unsigned short wib_columns_animate[]={
      CUBE_ANI_WATER, /*water */
      CUBE_ANI_LAVADK, CUBE_ANI_LAVABR, /*lava */
};

const unsigned short animated_cubes[]={
      CUBE_ANI_WATER, /*water */
      CUBE_ANI_LAVADK, CUBE_ANI_LAVABR, /*lava */
      0x0201, 0x0202, 0x0203, /*unknown - are used in template (?!) */
};

/*
 * Creates empty COLUMN_REC structure, sets default values inside
*/
struct COLUMN_REC *create_column_rec(void)
{
    struct COLUMN_REC *clm_rec=(struct COLUMN_REC *)malloc(sizeof(struct COLUMN_REC));
    if (clm_rec==NULL)
    {
        message_error("Cannot allocate memory for clm_rec");
        return NULL;
    }
    fill_column_rec(clm_rec, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0);
    return clm_rec;
}

/*
 * Creates empty permanent COLUMN_REC structure, sets default values inside
*/
struct COLUMN_REC *create_column_recp(void)
{
    struct COLUMN_REC *clm_rec=(struct COLUMN_REC *)malloc(sizeof(struct COLUMN_REC));
    if (clm_rec==NULL)
    {
        message_error("create_column_recp: Cannot allocate memory for clm_rec");
        return NULL;
    }
    fill_column_rec(clm_rec, 0, 1, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0);
    return clm_rec;
}

/*
 * Frees memory of given COLUMN_REC entry,.
 */
void free_column_rec(struct COLUMN_REC *clm_rec)
{
    free(clm_rec);
}

void clear_clm_entry(unsigned char *clmentry)
{
    if (clmentry==NULL) return;
    memset(clmentry,'\0',SIZEOF_DK_CLM_REC);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=1, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_simp(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use,1, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using permanent=0, lintel=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_sim(struct COLUMN_REC *clm_rec, unsigned int use, int base,
        int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  fill_column_rec_autosh(clm_rec, use, 0, 0, base, 
                 c0, c1, c2, c3, c4, c5, c6, c7);
}

/*
 * Fills a given COLUMN_REC entry, using orientation=0,
 * and automatically computing correct height and solid params.
 */
void fill_column_rec_autosh(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int base, 
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
{
    fill_column_rec(clm_rec, use, permanent, lintel, 0, 0, base, 0,
               c0, c1, c2, c3, c4, c5, c6, c7);
    clm_rec->solid=compute_clm_rec_solid(clm_rec);
    clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Fills a given COLUMN_REC entry, requires all parameters stored in CLM entry.
 */
void fill_column_rec(struct COLUMN_REC *clm_rec, const unsigned int use,
        const int permanent, const int lintel, const int height,
        const unsigned int solid, const int base, const int orientation,
        const int c0, const int c1, const int c2, const int c3,
        const int c4, const int c5, const int c6, const int c7)
{
    clm_rec->use=use;
    clm_rec->permanent=permanent;
    clm_rec->lintel=lintel;
    clm_rec->height=height;
    clm_rec->solid=solid;
    clm_rec->base=base;
    clm_rec->orientation=orientation;
    clm_rec->c[0]=c0;
    clm_rec->c[1]=c1;
    clm_rec->c[2]=c2;
    clm_rec->c[3]=c3;
    clm_rec->c[4]=c4;
    clm_rec->c[5]=c5;
    clm_rec->c[6]=c6;
    clm_rec->c[7]=c7;
}

/*
 * Sets a CLM entry; c1 is the lowest square, c8 - the top one.
 */
void set_clm_entry(unsigned char *clmentry, struct COLUMN_REC *clm_rec)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    unsigned char *x=clmentry;
    x[0]=clm_rec->use&255;
    x[1]=(clm_rec->use>>8)&255;
    x[2]=(clm_rec->permanent&1)+((clm_rec->lintel&7)<<1)+((clm_rec->height&15)<<4);
    x[3]=clm_rec->solid&255;
    x[4]=(clm_rec->solid>>8)&255;
    x[5]=clm_rec->base&255;
    x[6]=(clm_rec->base>>8)&255;
    x[7]=clm_rec->orientation&255;
    x[8]=clm_rec->c[0]&255;
    x[9]=(clm_rec->c[0]>>8)&255;
    x[10]=clm_rec->c[1]&255;
    x[11]=(clm_rec->c[1]>>8)&255;
    x[12]=clm_rec->c[2]&255;
    x[13]=(clm_rec->c[2]>>8)&255;
    x[14]=clm_rec->c[3]&255;
    x[15]=(clm_rec->c[3]>>8)&255;
    x[16]=clm_rec->c[4]&255;
    x[17]=(clm_rec->c[4]>>8)&255;
    x[18]=clm_rec->c[5]&255;
    x[19]=(clm_rec->c[5]>>8)&255;
    x[20]=clm_rec->c[6]&255;
    x[21]=(clm_rec->c[6]>>8)&255;
    x[22]=clm_rec->c[7]&255;
    x[23]=(clm_rec->c[7]>>8)&255;
}

void get_clm_entry(struct COLUMN_REC *clm_rec, const unsigned char *clmentry)
{
    if ((clmentry==NULL)||(clm_rec==NULL)) return;
    clm_rec->use=clmentry[0]+(clmentry[1]<<8);
    clm_rec->permanent=clmentry[2]&1;
    clm_rec->lintel=(clmentry[2]>>1)&7;
    clm_rec->height=(clmentry[2]>>4)&15;
    clm_rec->solid=clmentry[3]+(clmentry[4]<<8);
    clm_rec->base=clmentry[5]+(clmentry[6]<<8);
    clm_rec->orientation=clmentry[7];
    clm_rec->c[0]=clmentry[8]+(clmentry[9]<<8);
    clm_rec->c[1]=clmentry[10]+(clmentry[11]<<8);
    clm_rec->c[2]=clmentry[12]+(clmentry[13]<<8);
    clm_rec->c[3]=clmentry[14]+(clmentry[15]<<8);
    clm_rec->c[4]=clmentry[16]+(clmentry[17]<<8);
    clm_rec->c[5]=clmentry[18]+(clmentry[19]<<8);
    clm_rec->c[6]=clmentry[20]+(clmentry[21]<<8);
    clm_rec->c[7]=clmentry[22]+(clmentry[23]<<8);
}

/*
 * Determines if the two CLM entries looks identically - COLUMN_REC version
 */
short compare_column_recs(struct COLUMN_REC *clm_rec1, struct COLUMN_REC *clm_rec2)
{
  if (clm_rec1->height != clm_rec2->height)
    return false;
  if (clm_rec1->solid != clm_rec2->solid)
    return false;
  if (clm_rec1->lintel != clm_rec2->lintel)
    return false;
  /*unsigned short height=clm_rec1->height;*/
  unsigned int solid=clm_rec1->solid;
  if (clm_rec1->base!=clm_rec2->base)
    return false;
  if ((solid&1))
    if (clm_rec1->c[0]!=clm_rec2->c[0])
      return false;
  if ((solid&2))
    if (clm_rec1->c[1]!=clm_rec2->c[1])
      return false;
  if ((solid&4))
    if (clm_rec1->c[2]!=clm_rec2->c[2])
      return false;
  if ((solid&8))
    if (clm_rec1->c[3]!=clm_rec2->c[3])
      return false;
  if ((solid&16))
    if (clm_rec1->c[4]!=clm_rec2->c[4])
      return false;
  if ((solid&32))
    if (clm_rec1->c[5]!=clm_rec2->c[5])
      return false;
  if ((solid&64))
    if (clm_rec1->c[6]!=clm_rec2->c[6])
      return false;
  if ((solid&128))
    if (clm_rec1->c[7]!=clm_rec2->c[7])
      return false;
  return true;
}

/*
 * Determines if the two CLM entries looks identically - basic version
 */
short compare_column_entries(const unsigned char *clmentry1, const unsigned char *clmentry2)
{
   /*comparing 'lintel' and 'height', but not 'permanent' */
    if ((clmentry1[2]&0x0fe)!=(clmentry2[2]&0x0fe))
        return false;
    /* Comparing 'solid' and 'base' */
    if (memcmp(clmentry1+3,clmentry2+3,4)!=0)
        return false;
    /* Comparing cube values */
    if (memcmp(clmentry1+8,clmentry2+8,15)!=0)
        return false;
    return true;
}

unsigned short compute_clm_rec_height(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    /*First test - 3 free bottom columns makes height at the bottom. */
    if ((clm_rec->c[0]==0) && (clm_rec->c[1]==0) && (clm_rec->c[2]==0))
      return 0;
    if ((clm_rec->c[1]==0) && (clm_rec->c[2]==0) && (clm_rec->c[3]==0))
      return 1;
    if ((clm_rec->c[2]==0) && (clm_rec->c[3]==0) && (clm_rec->c[4]==0))
      return 2;
    if ((clm_rec->c[3]==0) && (clm_rec->c[4]==0) && (clm_rec->c[5]==0))
      return 3;
    /*Door when they are opening/open */
    if ((clm_rec->c[0]==0x094) && (clm_rec->c[1]==0x0))
      return 1;
    /*Strange iron door from map 67 (that's probably wrong entry - but the warn message for it annoys me) */
    if ((clm_rec->base==0x0b1) && (clm_rec->c[0]==0x0) && (clm_rec->c[1]==0x17c))
      return 0;
    if ((clm_rec->c[7]!=0))
      return 8;
    if ((clm_rec->c[6]!=0))
      return 7;
    if ((clm_rec->c[5]!=0))
      return 6;
    if ((clm_rec->c[4]!=0))
      return 5;
    if ((clm_rec->c[3]!=0))
      return 4;
    if ((clm_rec->c[2]!=0))
      return 3;
    if ((clm_rec->c[1]!=0))
      return 2;
    if ((clm_rec->c[0]!=0))
      return 1;
    return 0;
}

unsigned short compute_clm_rec_solid(const struct COLUMN_REC *clm_rec)
{
    if (clm_rec==NULL) return 0;
    unsigned short solid=0;
    if ((clm_rec->c[7]!=0))
      solid|=128;
    if ((clm_rec->c[6]!=0))
      solid|=64;
    if ((clm_rec->c[5]!=0))
      solid|=32;
    if ((clm_rec->c[4]!=0))
      solid|=16;
    if ((clm_rec->c[3]!=0))
      solid|=8;
    if ((clm_rec->c[2]!=0))
      solid|=4;
    if ((clm_rec->c[1]!=0))
      solid|=2;
    if ((clm_rec->c[0]!=0))
      solid|=1;
    return solid;
}

unsigned int get_clm_entry_use(const unsigned char *clmentry)
{
    return (unsigned int)clmentry[0]+(clmentry[1]<<8);
}

/*
 * Increases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_inc(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use<65535) use++;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Decreases USE parameter of the column entry.
 * Returns the new value.
 */
unsigned int clm_entry_use_dec(unsigned char *clmentry)
{
    unsigned int use=clmentry[0]+(clmentry[1]<<8);
    if (use>0) use--;
    clmentry[0]=use&255;
    clmentry[1]=(use>>8)&255;
    return use;
}

/*
 * Returns the value of "permanent".
 */
short get_clm_entry_permanent(const unsigned char *clmentry)
{
    if (clmentry==NULL) return 0;
    short permanent=clmentry[2]&1;
    return permanent;
}

void set_clm_entry_permanent(unsigned char *clmentry,short nperm)
{
    if (clmentry==NULL) return;
    unsigned char val;
    val=clmentry[2] & 0xfe; /*clear permanent bit */
    clmentry[2]=(val)|(nperm!=0);
}

unsigned short get_clm_entry_height(const unsigned char *clmentry)
{
    return (unsigned short)(clmentry[2]>>4)&15;
}

unsigned short get_clm_entry_solid(const unsigned char *clmentry)
{
    return read_int16_le_buf(clmentry+3);
}

unsigned short get_clm_entry_base(const unsigned char *clmentry)
{
    return read_int16_le_buf(clmentry+5);
}

/*
 * Returns most top nonzero cube entry.
 * If there are no nonzero cubes, returns 0.
 */
unsigned short get_clm_entry_topcube(const unsigned char *clmentry)
{
    unsigned short cube=0;
    int i=7;
    while ((cube==0)&&(i>=0))
    {
          cube = read_int16_le_buf(clmentry+(i<<1)+8);
          i--;
    }
    return cube;
}

short clm_rec_copy(struct COLUMN_REC *dest_rec,const struct COLUMN_REC *src_rec)
{
    memcpy(dest_rec,src_rec,sizeof(struct COLUMN_REC));
    return true;
}

/*
 * Verifies values in the CLM entry.
 */
short clm_verify_entry(const unsigned char *clmentry, char *err_msg)
{
  struct COLUMN_REC *clm_rec;
  clm_rec=create_column_rec();
  get_clm_entry(clm_rec, clmentry);
  int i;
  for (i=0;i<9;i++)
  {
    unsigned int cube;
    if (i==0)
      cube=clm_rec->base;
    else
      cube=clm_rec->c[i-1];
    if ((cube>CUBE_MAX_INDEX)&&(!is_animated_cube(cube)))
    {
        sprintf(err_msg,"Cube entry %d too large (%u>%d)",i,cube,CUBE_MAX_INDEX);
        return VERIF_WARN;
    }
  }
  unsigned int prop_solid=compute_clm_rec_solid(clm_rec);
  if (clm_rec->solid != prop_solid)
  {
        sprintf(err_msg,"Solid property should be %u",prop_solid);
        return VERIF_WARN;
  }
  unsigned int prop_height=compute_clm_rec_height(clm_rec);
  if (clm_rec->height != prop_height)
  {
        sprintf(err_msg,"Height property should be %u",prop_height);
        return VERIF_WARN;
  }
  free_column_rec(clm_rec);
  return VERIF_OK;
}

/*
 * Returns if the column should be animated (contains water or lava, or similar)
 */
short cube_wib_animate(unsigned int cube)
{
    int array_count=sizeof(wib_columns_animate)/sizeof(unsigned short);
    int idx=arr_ushort_pos(wib_columns_animate,cube,array_count);
    if (idx>=0) return true;
    return false;
}

/*
 * Returns if the column is one of the special, animated and not defined in CUBES.DAT
 */
short is_animated_cube(unsigned int cube)
{
    int array_count=sizeof(animated_cubes)/sizeof(unsigned short);
    int idx=arr_ushort_pos(animated_cubes,cube,array_count);
    if (idx>=0) return true;
    return false;
}

/*
 * Returns a WIB file entry for given column; requires three additional columns:
 *          one from the north, one from the west and from NW corner.
 */
unsigned short column_wib_entry(struct COLUMN_REC *clm_rec,
    struct COLUMN_REC *clm_rec_n,struct COLUMN_REC *clm_rec_w,struct COLUMN_REC *clm_rec_nw)
{
    int i;
    /* Preparing info if siblings are animated */
    short anim_north=false;
    if (cube_wib_animate(clm_rec_n->base))
      anim_north=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_n->solid&(1<<i))&&(cube_wib_animate(clm_rec_n->c[i])))
        anim_north=true;
    }
    short anim_west=false;
    if (cube_wib_animate(clm_rec_w->base))
      anim_west=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_w->solid&(1<<i))&&(cube_wib_animate(clm_rec_w->c[i])))
        anim_west=true;
    }
    short anim_nw=false;
    if (cube_wib_animate(clm_rec_nw->base))
      anim_nw=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec_nw->solid&(1<<i))&&(cube_wib_animate(clm_rec_nw->c[i])))
        anim_nw=true;
    }
    /*And if this one alone should animate */
    short anim_this=false;
    if (cube_wib_animate(clm_rec->base))
      anim_this=true;
    for (i=0;i<8;i++)
    {
      if ((clm_rec->solid&(1<<i))&&(cube_wib_animate(clm_rec->c[i])))
        anim_this=true;
    }
    /*Getting to decision */
    if (anim_this&&anim_west&&anim_north&&anim_nw)
      return COLUMN_WIB_ANIMATE;
    return COLUMN_WIB_SKEW;
}

/*
 * Returns cube type name as text
 */
char *get_cube_fullname(unsigned short idx)
{
    int types_count=sizeof(cube_fullnames)/sizeof(char *);
    if (idx<types_count)
      return (char *)cube_fullnames[idx];
    else
      return "unknown(?!)";
}

void place_column_doorwood_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x169;
  clm_rec->c[pos+1]=0x166;
  clm_rec->c[pos+2]=0x163;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorwood_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x16b;
  clm_rec->c[pos+1]=0x168;
  clm_rec->c[pos+2]=0x165;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x172;
  clm_rec->c[pos+1]=0x16f;
  clm_rec->c[pos+2]=0x16c;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x173;
  clm_rec->c[pos+1]=0x170;
  clm_rec->c[pos+2]=0x16d;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doorbrace_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x174;
  clm_rec->c[pos+1]=0x171;
  clm_rec->c[pos+2]=0x16e;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17b;
  clm_rec->c[pos+1]=0x178;
  clm_rec->c[pos+2]=0x175;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17c;
  clm_rec->c[pos+1]=0x179;
  clm_rec->c[pos+2]=0x176;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dooriron_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x17d;
  clm_rec->c[pos+1]=0x17a;
  clm_rec->c[pos+2]=0x177;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12e;
  clm_rec->c[pos+1]=0x12b;
  clm_rec->c[pos+2]=0x128;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a0; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19a; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19d; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a0; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a3; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12d;
  clm_rec->c[pos+1]=0x12a;
  clm_rec->c[pos+2]=0x127;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[0]=0x0c0; clm_rec->c[pos+3]=0x0a1; break;
  case PLAYER1:    clm_rec->c[0]=0x0c1; clm_rec->c[pos+3]=0x19b; break;
  case PLAYER2:    clm_rec->c[0]=0x0c2; clm_rec->c[pos+3]=0x19e; break;
  case PLAYER3:    clm_rec->c[0]=0x0c3; clm_rec->c[pos+3]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[0]=0x0c7; clm_rec->c[pos+3]=0x1a4; break;
  default:         clm_rec->c[0]=0x0c6; clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_doormagic_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=1;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x12c;
  clm_rec->c[pos+1]=0x129;
  clm_rec->c[pos+2]=0x126;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[pos+3]=0x0a2; break;
  case PLAYER1:    clm_rec->c[pos+3]=0x19c; break;
  case PLAYER2:    clm_rec->c[pos+3]=0x19f; break;
  case PLAYER3:    clm_rec->c[pos+3]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[pos+3]=0x1a5; break;
  default:         clm_rec->c[pos+3]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_rock_gndlev(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, CUBE_PATH_SMLSTONES,
         CUBE_ROCK_ANY, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

void fill_column_rockground(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_simp(clm_rec, 0, CUBE_PATH_SMLSTONES,
         CUBE_ROCK3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

void fill_column_rock(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rockground(clm_rec,owner);
     place_column_rock(clm_rec,owner);
}

/*
 * Rock column - should be placed on rock ground.
 */
void place_column_rock(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_ROCK3;
  clm_rec->c[pos+1]=CUBE_ROCK2;
  clm_rec->c[pos+2]=CUBE_ROCK2;
  clm_rec->c[pos+3]=CUBE_ROCK1;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Ground for earth, gold and any wall to put on top
 */
void fill_column_earthground(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

/*
 * Ground for any wall to put on top; near water version
 */
void fill_column_wallground_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x075, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

/*
 * Ground for any wall to put on top; near lava version
 */
void fill_column_wallground_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x076, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

void fill_column_gold(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_EARTHGRNSDCTR, CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), 0, 0, 0);
}

void fill_column_gold_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x026, CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), CUBE_GOLD1 +rnd(3), 0, 0, 0);
}

void fill_column_gold_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_EARTHBTMLAVA, 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0x034+rnd(3), 0, 0, 0);
}

void fill_column_earth(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_EARTHGRNSDCTR, 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x026, 0x001+rnd(3), 0x001+rnd(3), 0x001+rnd(3), 0x005, 0, 0, 0);
}

void fill_column_earth_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_EARTHBTMLAVA, 0x030, 0x030, 0x030, 0x030, 0, 0, 0);
}

void fill_column_wall_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     /*fill_column_rec_sim(clm_rec, 0, CUBE_PATH3, */
     /*    CUBE_PATH1+rnd(5), 0x00a, 0x001+rnd(3), 0x001+rnd(3), 0x004, 0, 0, 0); */
     /* Simplified - why wasting CLM entires if only top is visible anyway? */
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, CUBE_EARTHGRNSDCTR, CUBE_EARTHSTD2, CUBE_EARTHSTD2, CUBE_EARTHBRGTOPDKL, 0, 0, 0);
     /*The "top" is owner-dependent */
     if (owner<PLAYER_UNSET)
         clm_rec->c[4]=0x043+owner;
}

/*
 * Column of grey stones, larger than brick.
 */
void fill_column_wall_cobblestones(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_earthground(clm_rec,owner);
    place_column_wall_cobblestones(clm_rec,owner);
}

/*
 * Column of grey stones with marking only on one of them.
 */
void place_column_wall_cobblestones(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x098;
  clm_rec->c[pos+1]=0x097;
  clm_rec->c[pos+2]=0x096;
  clm_rec->c[pos+3]=0x095;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

/*
 * Column of grey stones with some markings on.
 */
void place_column_wall_cobblestones_mk(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x03e;
  clm_rec->c[pos+1]=0x039;
  clm_rec->c[pos+2]=0x03a;
  clm_rec->c[pos+3]=0x041;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x053;
  clm_rec->c[pos+1]=0x050;
  clm_rec->c[pos+2]=0x050;
  clm_rec->c[pos+3]=0x04e;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_redsmbrick_dkbtm(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x052;
  clm_rec->c[pos+1]=0x048+rnd(3);
  clm_rec->c[pos+2]=0x048+rnd(3);
  clm_rec->c[pos+3]=0x04d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_redsmbrick_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x051;
  clm_rec->c[pos+1]=0x04f;
  clm_rec->c[pos+2]=0x04f;
  clm_rec->c[pos+3]=0x04c;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_redsmbrick(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=clm_rec->height;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x048+rnd(3);
  clm_rec->c[pos+1]=0x048+rnd(3);
  clm_rec->c[pos+2]=0x048+rnd(3);
  clm_rec->c[pos+3]=0x04d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_wall_redsmbrick_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x053, 0x050, 0x050, 0x04e, 0, 0, 0);
}

void fill_column_wall_redsmbrick_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x052, 0x048+rnd(3), 0x048+rnd(3), 0x04d, 0, 0, 0);
}

void fill_column_wall_redsmbrick_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x051, 0x04f, 0x04f, 0x04c, 0, 0, 0);
}

void fill_column_wall_redsmbrick_a_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x075, 0x048+rnd(3), 0x048+rnd(3), 0x048+rnd(3), 0x04d, 0, 0, 0);
}

void fill_column_wall_redsmbrick_a_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x076, 0x048+rnd(3), 0x048+rnd(3), 0x048+rnd(3), 0x04d, 0, 0, 0);
}

void fill_column_wall_redsmbrick_c_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x075, 0x04f, 0x04f, 0x04f, 0x04c, 0, 0, 0);
}

void fill_column_wall_redsmbrick_c_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x076, 0x04f, 0x04f, 0x04f, 0x04c, 0, 0, 0);
}

void fill_column_wall_drapebrick_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    /*we could use fill_column_wall_redsmbrick_a to get this better, */
    /* but this way we're not losing CLM entries */
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a0; break;
    case PLAYER1:    clm_rec->c[4]=0x19a; break;
    case PLAYER2:    clm_rec->c[4]=0x19d; break;
    case PLAYER3:    clm_rec->c[4]=0x1a0; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a3; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
}

void fill_column_wall_drapebrick_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    /*we could use fill_column_wall_redsmbrick_a to get this better, */
    /* but this way we're not losing CLM entries */
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a1; break;
  case PLAYER1:    clm_rec->c[4]=0x19b; break;
  case PLAYER2:    clm_rec->c[4]=0x19e; break;
  case PLAYER3:    clm_rec->c[4]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a4; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
}

void fill_column_wall_drapebrick_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1, 0x052, 0x049, 0x049, 0x04d, 0, 0, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[4]=0x0a2; break;
    case PLAYER1:    clm_rec->c[4]=0x19c; break;
    case PLAYER2:    clm_rec->c[4]=0x19f; break;
    case PLAYER3:    clm_rec->c[4]=0x1a2; break;
    case PLAYER_GOOD:clm_rec->c[4]=0x1a5; break;
    default:         clm_rec->c[4]=0x04d; break;
    }
}

void fill_column_wall_twinsbrick_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x195, CUBE_BRICK_RTWINSBL, CUBE_BRICK_RTWINSML, CUBE_BRICK_RTWINSTL, 0, 0, 0);
}

void fill_column_wall_twinsbrick_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x196, CUBE_BRICK_RTWINSBC, CUBE_BRICK_RTWINSMC, CUBE_BRICK_RTWINSTC, 0, 0, 0);
}

void fill_column_wall_twinsbrick_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x197, CUBE_BRICK_RTWINSBR, CUBE_BRICK_RTWINSMR, CUBE_BRICK_RTWINSTR, 0, 0, 0);
}

void fill_column_wall_womanbrick_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBL, CUBE_BRICK_RWOMANML,
         CUBE_BRICK_RWOMANUL, CUBE_BRICK_RWOMANTL, 0, 0, 0);
}

void fill_column_wall_womanbrick_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBC, CUBE_BRICK_RWOMANMC,
         CUBE_BRICK_RWOMANUC, CUBE_BRICK_RWOMANTC, 0, 0, 0);
}

void fill_column_wall_womanbrick_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_RWOMANBR, CUBE_BRICK_RWOMANMR,
         CUBE_BRICK_RWOMANUR, CUBE_BRICK_RWOMANTR, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBL, CUBE_BRICK_PAIRSHML,
         CUBE_BRICK_PAIRSHUL, CUBE_BRICK_PAIRSHTL, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBC, CUBE_BRICK_PAIRSHMC,
         CUBE_BRICK_PAIRSHUC, CUBE_BRICK_PAIRSHTC, 0, 0, 0);
}

void fill_column_wall_pairshrbrick_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), CUBE_BRICK_PAIRSHBR, CUBE_BRICK_PAIRSHMR,
         CUBE_BRICK_PAIRSHUR, CUBE_BRICK_PAIRSHTR, 0, 0, 0);
}

void fill_column_path(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0cf,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_purple_path(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 1,
         CUBE_EMPTY1, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

/*
 * Claimed ground (not claimed wall)
 */
void fill_column_claimedgnd_centr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x0c6, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
     /*The "top" is owner-dependent */
     if (owner==PLAYER_GOOD)
         clm_rec->c[0]=0x0c7;
     else
     if (owner<PLAYER_UNSET)
         clm_rec->c[0]=0x0c0+owner;
}

void fill_column_claimedgnd_surr(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x07e +rnd(3), 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearwater(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x094, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_claimedgnd_nearlava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0ce,
         0x093, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_lava(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{ /*Simplicity of this one scares me.. */
     fill_column_rec_sim(clm_rec, 0, CUBE_ANI_LAVA_ANY,
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_water(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_ANI_WATER,
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_gem(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, CUBE_GEMS_ANY, CUBE_GEMS_ANY, CUBE_GEMS_ANY, 0, 0, 0);
}

void fill_column_library_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x0b9, 0x0b8, 0x0b7, 0x0b6, 0, 0, 0);
}

void place_column_library_bookcase_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x14b;
  clm_rec->c[2]=0x14d;
  clm_rec->c[3]=0x14b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x0b1;
  clm_rec->c[2]=0x0b4;
  clm_rec->c[3]=0x0b1;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_library_bookcase_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  clm_rec->base=CUBE_PATH_SMOOTH3;
  clm_rec->c[1]=0x14c;
  clm_rec->c[2]=0x14e;
  clm_rec->c[3]=0x14c;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>1) pos=1;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=0x122;
  clm_rec->c[pos+2]=0x120;
  clm_rec->c[pos+3]=0x11d;
  clm_rec->c[pos+4]=0x120;
  clm_rec->c[pos+5]=0x11e;
  clm_rec->c[pos+6]=0x11d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_temple_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x177,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x123, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_temple_corner(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_temple_edge(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  clm_rec->c[pos]=0x122;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_dungheart_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x09f , 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_dungheart_pillar(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        0x080 , 0x052, 0x048, 0x049, 0x04a, 0x049, 0x17f, 0x1aa);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[7]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[7]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[7]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[7]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[7]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[7]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_dungheart_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x052, 0x09f, 0x0, 0x0, 0, 0, 0);
}


void place_column_univ_stair(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+1]=clm_rec->c[pos+0];
  clm_rec->c[pos+0]=CUBE_PATH_SMOOTH1 +rnd(5);
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_dungheart_raise(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x052;
  clm_rec->c[pos+1]=0x04d;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_portal_edge(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x138, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_portal_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMLSTONES,
        CUBE_PATH_SMOOTH1 +rnd(5), 0x00a, 0x00f, 0x00d, 0x00d, 0x00c, 0x00e, 0x0);
}

void fill_column_portal_step(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_LRGSTONES +rnd(2),
         0x00a, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00e, 0);
}

void fill_column_portal_inside_cntr(struct COLUMN_REC *clm_rec, unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x089+4, 0);
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[6]=CUBE_EYEDROCKRED; break;
    case PLAYER1:    clm_rec->c[6]=CUBE_EYEDROCKBLUE; break;
    case PLAYER2:    clm_rec->c[6]=CUBE_EYEDROCKGREEN; break;
    case PLAYER3:    clm_rec->c[6]=CUBE_EYEDROCKYELLOW; break;
    case PLAYER_GOOD:clm_rec->c[6]=CUBE_EYEDROCKWHITE; break;
    default:         clm_rec->c[6]=CUBE_EYEDROCKBLINK; break;
    }
}

void fill_column_hatchery_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3 +rnd(5),
         0x0, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_edge(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x15a, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_hatchery_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x15b, 0x158, 0x158, 0x158, 0x158, 0x159, 0, 0);
}

void fill_column_lair_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         CUBE_LAIR_INSIDE, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_graveyard_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_LRGSTONES,
         0x142, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_barracks_raise(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height;
  if (pos>7) pos=7;
  if (pos<1) pos=1;
  clm_rec->c[pos-1]=0x183;
  clm_rec->c[pos+0]=0x194;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void fill_column_barracks_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
         0x181, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_barracks_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        CUBE_PATH_SMOOTH2, 0x184, 0x184, 0x18f, 0x18e, 0x184, 0x0, 0x0);
}

void fill_column_training_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x13e,
         0x0fc, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_training_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x13e,
        0x0fc, 0x0fb, 0x0fa, 0x0f9, 0x0f9, 0x0f1, 0x0, 0x0);
}

void fill_column_treasure_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x06a,
         0x09f, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_treasure_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x06a,
        0x09f, 0x03e, 0x039, 0x03a, 0x039, 0x041, 0x0, 0x0);
}

void fill_column_workshop_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x15a,
         0x107, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_workshop_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, CUBE_PATH_SMOOTH3,
        CUBE_PATH_LRGSTONES, 0x10e, 0x10d, 0x10c, 0x10b, 0x0, 0x0, 0x0);
}


void fill_column_scavenger_floor(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x119, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_scavenger_pillar(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
    fill_column_rec_sim(clm_rec, 0, 0x0f8,
        CUBE_PATH_SMOOTH1, 0x118, 0x117, 0x116, 0x115, 0x0, 0x0, 0x0);
}

void fill_column_scavenger_inside_cntr(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0f8,
         0x11c, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_prison_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x118,
         0x0c8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR1, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR2, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_guardpost_floor_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x0b0 +rnd(3),
         0x07e +rnd(3), CUBE_WOOD_FLOOR3, 0x0, 0x0, 0x0, 0, 0, 0);
}

void fill_column_bridge_inside(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
     fill_column_rec_sim(clm_rec, 0, 0x1f3,
         0x1b8, 0x0, 0x0, 0x0, 0x0, 0, 0, 0);
}

void place_column_wall_drape_a(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[pos+0]=0x0a0; break;
    case PLAYER1:    clm_rec->c[pos+0]=0x19a; break;
    case PLAYER2:    clm_rec->c[pos+0]=0x19d; break;
    case PLAYER3:    clm_rec->c[pos+0]=0x1a0; break;
    case PLAYER_GOOD:clm_rec->c[pos+0]=0x1a3; break;
    default:         clm_rec->c[pos+0]=0x04d; break;
    }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_drape_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a1; break;
  case PLAYER1:    clm_rec->c[4]=0x19b; break;
  case PLAYER2:    clm_rec->c[4]=0x19e; break;
  case PLAYER3:    clm_rec->c[4]=0x1a1; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a4; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_drape_c(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-1;
  if (pos<0) pos=0;
  if (pos>7) pos=7;
  switch (owner)
  {
  case PLAYER0:    clm_rec->c[4]=0x0a2; break;
  case PLAYER1:    clm_rec->c[4]=0x19c; break;
  case PLAYER2:    clm_rec->c[4]=0x19f; break;
  case PLAYER3:    clm_rec->c[4]=0x1a2; break;
  case PLAYER_GOOD:clm_rec->c[4]=0x1a5; break;
  default:         clm_rec->c[4]=0x04d; break;
  }
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_portal_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x098;
  clm_rec->c[pos+1]=0x097;
  clm_rec->c[pos+2]=0x096;
  clm_rec->c[pos+3]=0x095;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_portal_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x04b;
  clm_rec->c[pos+1]=0x04b;
  clm_rec->c[pos+2]=0x04b;
  clm_rec->c[pos+3]=0x04b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09c;
  clm_rec->c[pos+1]=0x099;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09d;
  clm_rec->c[pos+1]=0x09a;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_treasure_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=0x09e;
  clm_rec->c[pos+1]=0x09b;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x053;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBL;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x0fb;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBC;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_training_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x051;
  clm_rec->c[pos+1]=CUBE_BRICK_CRSWRDBR;
  clm_rec->c[pos+2]=CUBE_BRICK_CRSWRDTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBL;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBC;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_library_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_MGMIRRBR;
  clm_rec->c[pos+1]=CUBE_BRICK_MGMIRRTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBL;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBC;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_scavenger_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_GODEYEBR;
  clm_rec->c[pos+1]=CUBE_BRICK_GODEYETR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_STONEWALL2;
  clm_rec->c[pos+1]=CUBE_STONEWALL1;
  clm_rec->c[pos+2]=0x050;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=0x098;
  clm_rec->c[pos+1]=0x097;
  clm_rec->c[pos+2]=0x096;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_graveyard_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_STONEWALL2;
  clm_rec->c[pos+1]=CUBE_STONEWALL1;
  clm_rec->c[pos+2]=0x04f;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_lair_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2L;
  clm_rec->c[pos+3]=CUBE_BRICK_SKLSTW1L;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_lair_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2C;
  clm_rec->c[pos+1]=CUBE_BRICK_SKULSTM;
  clm_rec->c[pos+2]=CUBE_BRICK_SKULSTT;
  clm_rec->c[pos+3]=CUBE_BRICK_SKLSTW1C;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_lair_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2R;
  clm_rec->c[pos+3]=CUBE_BRICK_SKLSTW1R;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_laircrnr_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2L;
  clm_rec->c[pos+3]=0x0a0;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_laircrnr_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2C;
  clm_rec->c[pos+1]=CUBE_BRICK_SKULSTM;
  clm_rec->c[pos+2]=CUBE_BRICK_SKULSTT;
  clm_rec->c[pos+3]=0x0a1;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_laircrnr_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=CUBE_BRICK_SKLSTW2R;
  clm_rec->c[pos+3]=0x0a2;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBL;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBC;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_workshop_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_FURNCEBR;
  clm_rec->c[pos+1]=CUBE_BRICK_FURNCETR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBL;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBC;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_prison_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>6) pos=6;
  clm_rec->c[pos+0]=CUBE_BRICK_RDHEADBR;
  clm_rec->c[pos+1]=CUBE_BRICK_RDHEADTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBL;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELML;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBC;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELMC;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_torture_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_SPSKELBR;
  clm_rec->c[pos+1]=CUBE_BRICK_SPSKELMR;
  clm_rec->c[pos+2]=CUBE_BRICK_SPSKELTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBL;
  clm_rec->c[pos+1]=CUBE_BRICK_COOPERML;
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_b(struct COLUMN_REC *clm_rec, unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBC;
    switch (owner)
    {
    case PLAYER0:    clm_rec->c[pos+1]=CUBE_BRICK_COOPRMCR; break;
    case PLAYER1:    clm_rec->c[pos+1]=0x1ab; break;
    case PLAYER2:    clm_rec->c[pos+1]=0x1ac; break;
    case PLAYER3:    clm_rec->c[pos+1]=0x1ad; break;
    case PLAYER_GOOD:clm_rec->c[pos+1]=0x1af; break;
    default:         clm_rec->c[pos+1]=0x0ae; break;
    }
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_barracks_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-3;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_COOPERBR;
  clm_rec->c[pos+1]=CUBE_BRICK_COOPERMR;
  clm_rec->c[pos+2]=CUBE_BRICK_COOPERTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBL;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPML;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBC;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPMC;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_temple_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>4) pos=4;
  clm_rec->c[pos+0]=0x122;
  clm_rec->c[pos+1]=CUBE_BRICK_HORNRPBR;
  clm_rec->c[pos+2]=CUBE_BRICK_HORNRPMR;
  clm_rec->c[pos+3]=CUBE_BRICK_HORNRPTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_a(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBL;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNML;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTL;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_b(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBC;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNMC;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTC;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}

void place_column_wall_hatchery_c(struct COLUMN_REC *clm_rec, __attribute__((unused)) unsigned char owner)
{
  int pos=clm_rec->height-4;
  if (pos<0) pos=0;
  if (pos>5) pos=5;
  clm_rec->c[pos+0]=CUBE_BRICK_CHICKNBR;
  clm_rec->c[pos+1]=CUBE_BRICK_CHICKNMR;
  clm_rec->c[pos+2]=CUBE_BRICK_CHICKNTR;
  clm_rec->solid=compute_clm_rec_solid(clm_rec);
  clm_rec->height=compute_clm_rec_height(clm_rec);
}
