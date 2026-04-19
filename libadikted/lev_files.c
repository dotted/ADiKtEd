/******************************************************************************/
/** @file lev_files.c
 * Functions for loading and writing levels from/to disk.
 * @par Purpose:
 *     Defines functions for loading and writing levels from/to disk.
 *     There are single functions for r/w of specific files, and
 *     functions for loading/writing whole map.
 * @par Comment:
 *     None.
 * @author   Jon Skeet, Tomasz Lis
 * @date     14 Oct 1997 - 27 Jul 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "lev_files.h"

#include <sys/stat.h>
#include <string.h>

#include "globals.h"
#include "arr_utils.h"
#include "memfile.h"
#include "obj_column_def.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "bulcommn.h"
#include "obj_column.h"
#include "lev_data.h"
#include "lev_script.h"
#include "msg_log.h"
#include "lbfileio.h"
#include "lev_script.h"
#include "lev_things.h"

/**
 * Level file load/write function type definition.
 */
typedef short (*mapfile_io_func)(struct LEVEL *lvl,char *fname);

/**
 * Level file load/write function with error message parameter.
 */
typedef short (*mapfile_iomsg_func)(struct LEVEL *lvl,struct MEMORY_FILE *mem,char *err_msg);

/**
 * Returns load error message for specified error code.
 * @param errcode The integer error code.
 * @return Returns load error message text.
 */
char *levfile_error(int errcode)
{
    /*static char str[LINEMSG_SIZE]; */
    static char *const errors[] = {
	"File too small",
	"Bad data",
	"Bad file name",
	"Cant allocate mem",
	"Cant open for writing",
	"Write error",
	"Verification error",
	"Internal error",
	"Map rejected",
    };
    static char *const warnings[] = {
	"Bad items count",
	"Unknown problem",
    };
    /* Note: remember that errors are negative, warnings positive */
    if ((errcode>ERR_FILE_TOOSMLL)&&(errcode<WARN_BAD_COUNT))
    {
        return memfile_error(errcode);
    }
    if (errcode<=ERR_FILE_TOOSMLL)
    {
        errcode = ERR_FILE_TOOSMLL-errcode;
        /*sprintf(str,"Error code %d",errcode); */
        /*return str; */
        const int max_code=(sizeof(errors)/sizeof(*errors) - 1);
        if ((errcode < 0) || (errcode > max_code) )
          errcode = max_code;
        return errors[errcode];
   }
    if (errcode>=WARN_BAD_COUNT)
    {
        errcode = WARN_BAD_COUNT-errcode;
        const int max_code=(sizeof(warnings)/sizeof(*warnings) - 1);
        /*sprintf(str,"Warning code %d",errcode); */
        /*return str; */
        if ((errcode < 0) || (errcode > max_code) )
          errcode = max_code;
        return warnings[errcode];
   }
   return warnings[1];
}

/*
 * Old way of reading various files; not used anymore.
 * This function reads what Jon Skeet named "subtile format".
short load_subtile(unsigned char **dest,
                        char *fname, int length, int x, int y,
                        int linesize, int nlines, int lineoffset,
                        int mbytes, int byteoffset)
{
    struct MEMORY_FILE mem;
    int i, j;
    long addr=0;

    mem = read_file(fname);
    if (mem.errcode!=MFILE_OK)
      return false;
    if ((mem.len<mbytes*nlines*x*y) || (mem.len!=length))
    {
      free(mem.content);
      return false;
    }
    for (i=0; i < y; i++)
    {
      addr += linesize*lineoffset;
      for (j=0; j < x; j++)
          dest[j][i]=mem.content[addr+j*mbytes+byteoffset];
      addr += (nlines-lineoffset)*linesize;
    }
    free(mem.content);
    return true;
}
 */

#if 0
/*
 * Old way of reading various files; not used anymore.
 * This function reads what Jon Skeet named "subtile format".
 * It allocates memory for loaded data.
 */
unsigned char **load_subtile_malloc(char *fname, int length, 
                        int x, int y,
                        int linesize, int nlines, int lineoffset,
                        int mbytes, int byteoffset)
{
    /* Allocating mem */
    unsigned char **dest;
    dest = (unsigned char **)malloc(x*sizeof(char *));
    if (dest==NULL)
    {
        message_error("load_subtile: Out of memory");
        return NULL;
    }
    int i;
    for (i=0; i < x; i++)
    {
      dest[i]=(unsigned char *)malloc(y*sizeof(char));
      if (dest[i]==NULL)
      {
        message_error("load_subtile: Out of memory");
        return NULL;
      }
    }
    /* Loading */
    short result=load_subtile(dest,fname,length,x,y,
        linesize,nlines,lineoffset,mbytes,byteoffset);
    if (result)
      return dest;
    /* Freeing on error */
    for (i=0; i < x; i++)
      free(dest[i]);
    free(dest);
    return NULL;
}
#endif

/**
 * Reads the TNG file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_tng(struct LEVEL *lvl,char *fname)
{
    message_log("  load_tng: started");
    int tng_num;
    int i;
    if (lvl==NULL) return ERR_INTERNAL;
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if (mem->len<SIZEOF_DK_TNG_HEADER)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }
    result=ERR_NONE;
    /*Read the header */
    tng_num = read_int16_le_buf(mem->content);
    /* Check everything's cushty */
    unsigned long expect_size=tng_num*SIZEOF_DK_TNG_REC+SIZEOF_DK_TNG_HEADER;
    if (mem->len != expect_size)
    {
        message_log("  load_tng: File length %d, expected %lu (%d things)",mem->len,expect_size,tng_num);
        /* Fixing the problem */
        if (((lvl->optns.load_redundant_objects&EXLD_THING)==EXLD_THING)||(mem->len < expect_size))
          tng_num=(mem->len-SIZEOF_DK_TNG_HEADER)/SIZEOF_DK_TNG_REC;
        result=WARN_BAD_COUNT;
    }
    /*Read tng entries */
    for (i=0; i < tng_num; i++)
    {
      unsigned char *thing = create_thing_empty();
      int offs=SIZEOF_DK_TNG_REC*i+SIZEOF_DK_TNG_HEADER;
      memcpy(thing, mem->content+offs, SIZEOF_DK_TNG_REC);
      thing_add(lvl,thing);
    }
    if (tng_num != lvl->tng_total_count)
    {
        message_error("Internal error in load_tng: tng_num=%d tng_total=%d", tng_num, lvl->tng_total_count);
        return ERR_INTERNAL;
    }
    memfile_free(&mem);
    return result;
}

/**
 * Reads the CLM file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_clm(struct LEVEL *lvl,char *fname)
{
    message_log("  load_clm: started");
    int i;
    if ((lvl==NULL)||(lvl->clm==NULL)) return ERR_INTERNAL;
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if (mem->len < SIZEOF_DK_CLM_HEADER)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }
    memcpy(lvl->clm_hdr, mem->content+0, SIZEOF_DK_CLM_HEADER);
    int num_clms=read_int32_le_buf(mem->content+0);
    if (mem->len != SIZEOF_DK_CLM_REC*num_clms+SIZEOF_DK_CLM_HEADER)
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    if (num_clms>COLUMN_ENTRIES)
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    for (i=0; i<num_clms; i++)
    {
      int offs=SIZEOF_DK_CLM_REC*i+SIZEOF_DK_CLM_HEADER;
      memcpy(lvl->clm[i], mem->content+offs, SIZEOF_DK_CLM_REC);
    }
    memfile_free(&mem);
    return ERR_NONE;
}

/**
 * Loads the APT file fname, fills LEVEL apt entries.
 * This _must_ be called _after_ tng_* are set up.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_apt(struct LEVEL *lvl,char *fname)
{
    message_log("  load_apt: started");
    int i;
    unsigned char *actnpt;
    if ((lvl==NULL)||(lvl->apt_lookup==NULL)) return ERR_INTERNAL;
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if (mem->len < SIZEOF_DK_APT_HEADER)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }
    result=ERR_NONE;
    long apt_num;
    apt_num = read_int32_le_buf(mem->content+0);
    /* Check everything's cushty */
    unsigned long expect_size=apt_num*SIZEOF_DK_APT_REC+SIZEOF_DK_APT_HEADER;
    if (mem->len != expect_size)
    {
        message_log("  load_apt: File length %d, expected %lu (%d items)",mem->len,expect_size,apt_num);
        /* Fixing the problem */
        if (((lvl->optns.load_redundant_objects&EXLD_ACTNPT)==EXLD_ACTNPT)||(mem->len < expect_size))
          apt_num=(mem->len-SIZEOF_DK_APT_HEADER)/SIZEOF_DK_APT_REC;
        result=WARN_BAD_COUNT;
    }
    for (i=0; i < apt_num; i++)
    {
      actnpt=(unsigned char *)malloc(SIZEOF_DK_APT_REC);
      if (actnpt==NULL)
      {
        message_error("Cannot allocate mem for loading action points");
        return ERR_CANT_MALLOC;
      }
      memcpy (actnpt, mem->content+SIZEOF_DK_APT_REC*i+SIZEOF_DK_APT_HEADER, SIZEOF_DK_APT_REC);
      actnpt_add(lvl,actnpt);
    }
    if (apt_num != lvl->apt_total_count)
    {
        message_error("Internal error in load_apt: apt_num=%d apt_total=%d", apt_num, lvl->apt_total_count);
        return ERR_INTERNAL;
    }
    memfile_free(&mem);
    return result;
}

/**
 * Reads the INF file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_inf(struct LEVEL *lvl,char *fname)
{
    message_log("  load_inf: started");
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /*If wrong filesize - pannic */
    if (mem->len != 1)
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    lvl->inf=mem->content[0];
    memfile_free(&mem);
    return ERR_NONE;
}

/**
 * Reads the VSN file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_vsn(struct LEVEL *lvl,char *fname)
{
    message_log("  load_vsn: started");
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /*If wrong filesize - pannic */
    if (mem->len != 1)
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    unsigned char vsn;
    vsn=mem->content[0];
    memfile_free(&mem);
    if ((vsn==1)&&((lvl->format_version==MFV_DKSTD)||(lvl->format_version==MFV_DKGOLD)))
        return ERR_NONE;
    if ((vsn==2)&&((lvl->format_version==MFV_DKXPAND)))
        return ERR_NONE;
    return ERR_FILE_BADDATA;
}

/**
 * Reads the WIB file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_wib(struct LEVEL *lvl,char *fname)
{
    message_log("  load_wib: started");
    /*Loading the file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if ((mem->len!=lvl->subsize.x*lvl->subsize.y))
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    /*Reading WIB entries */
    int sx, sy;
    unsigned long addr;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      addr = sy*lvl->subsize.x;
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          set_subtl_wib(lvl,sx,sy,mem->content[addr+sx]);
      }
    }
    memfile_free(&mem);
    return ERR_NONE;
  /* Old way */
  /*return load_subtile(lvl->wib, fname, 65536, arr_entries_x, arr_entries_y,256, 1, 0, 1, 0); */
}

/**
 * Reads the SLB file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_slb(struct LEVEL *lvl,char *fname)
{
    message_log("  load_slb: started");
    /* Let's get the modification date, in case ADI script is lost */
    struct stat attrib;    /* create a file attribute structure */
    if (stat(fname,&attrib) == 0)  /* get the attributes of file */
    {
        lvl->info.creat_date=attrib.st_mtime;    /* Get the last modified time */
        lvl->info.lastsav_date=lvl->info.creat_date;
        int name_len=strlen(default_map_name)+10;
        char *name_text=malloc(name_len);
        if (name_text!=NULL)
        {
            strftime(name_text,name_len, default_map_name, localtime(&(lvl->info.creat_date)) );
            set_lif_name_text(lvl,name_text);
        }
    }
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if ((mem->len != 2*lvl->tlsize.x*lvl->tlsize.y))
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    /*Loading the entries */
    int i, k;
    unsigned long addr=0;
    for (i=0; i<lvl->tlsize.y; i++)
    {
      addr = 2*lvl->tlsize.x*i;
      for (k=0; k<lvl->tlsize.x; k++)
          set_tile_slab(lvl,k,i,read_int16_le_buf(mem->content+addr+k*2));
    }
    memfile_free(&mem);
    /*message_log("  load_slb: finished"); */
    return ERR_NONE;
    /*The old way - left as an antic */
    /*return load_subtile(, fname, 14450, MAP_SIZE_Y, MAP_SIZE_X,170, 1, 0, 2, 0); */
}

/**
 * Reads the OWN file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_own(struct LEVEL *lvl,char *fname)
{
    message_log("  load_own: started");
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if ((mem->len!=lvl->subsize.x*lvl->subsize.y))
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    /*Reading entries */
    int sx, sy;
    unsigned long addr;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      addr = sy*lvl->subsize.x;
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          set_subtl_owner(lvl,sx,sy,mem->content[addr+sx]);
      }
    }
    memfile_free(&mem);
    return ERR_NONE;
    /*Old way */
    /*return load_subtile(lvl->own, fname, 65536, MAP_SIZE_Y, MAP_SIZE_X,256, 3, 0, 3, 0);  */
}

/**
 * Reads the DAT file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_dat(struct LEVEL *lvl,char *fname)
{
    message_log("  load_dat: started");
    short result;
    const unsigned int line_len=2*lvl->subsize.x;
    /*Loading the file */
    struct MEMORY_FILE *mem;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /*message_log("  load_dat: after memfile_readnew"); */
    if ((mem->len != line_len*lvl->subsize.y))
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    /*Reading DAT entries */
    /*message_log("  load_dat: Reading DAT entries"); */
    int sx, sy;
    unsigned char *addr;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      addr = mem->content+sy*line_len;
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          set_dat_val(lvl,sx,sy,read_int16_le_buf(addr+sx*2));
      }
    }
    /*message_log("  load_dat: Reading entries finished"); */
    memfile_free(&mem);
    /*message_log("  load_dat: Loaded file memory freed"); */
    return ERR_NONE;
}

/**
 * Reads the TXT script file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_txt(struct LEVEL *lvl,char *fname)
{
    message_log("  load_txt: started");
    lvl->script.lines_count=0;
    /*Loading the file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
/*    message_log("  load_txt: file readed"); */
    /*If filesize too small - pannic */
    if (mem->len < 2)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }
    unsigned char *content=mem->content;
    unsigned char *ptr=mem->content;
    unsigned char *ptr_end=mem->content+mem->len;
    int lines_count=0;
/*    message_log("  load_txt: counting lines"); */
    while (ptr>=content)
    {
      ptr=memchr(ptr, 0x0a, (char *)ptr_end-(char *)ptr );
      lines_count++;
      if (ptr!=NULL) ptr++;
    }
    lvl->script.txt=(char **)malloc(lines_count*sizeof(unsigned char *));
    lvl->script.list=(struct DK_SCRIPT_COMMAND **)malloc(lines_count*sizeof(struct DK_SCRIPT_COMMAND *));
    ptr=mem->content;
    int currline;
    currline=0;
/*    message_log("  load_txt: breaking text into %d lines",lines_count); */
    while (currline<lines_count)
    {
      if (ptr>=ptr_end) ptr=ptr_end-1;
      unsigned char *nptr=memchr(ptr, 0x0a, ptr_end-ptr );
      /*Skip control characters (but leave spaces and TABs) */
      while ((ptr<nptr)&&((unsigned char)ptr[0]<0x20)&&((unsigned char)ptr[0]!=0x09)) ptr++;
      if (nptr==NULL)
        nptr=ptr_end;
      int linelen=(char *)nptr-(char *)ptr;
      /*At end, skip control characters and spaces too */
      while ((linelen>0)&&((unsigned char)ptr[linelen-1]<=0x20)) linelen--;
      lvl->script.txt[currline]=(unsigned char *)malloc((linelen+1)*sizeof(unsigned char));
      lvl->script.list[currline]=NULL; /* decompose_script() will allocate memory for it */
      memcpy(lvl->script.txt[currline],ptr,linelen);
      lvl->script.txt[currline][linelen]='\0';
      ptr=nptr+1;
      currline++;
    }
/*    message_log("  load_txt: deleting empty lines"); */
    int nonempty_lines=lines_count-1;
    /* Delete empty lines at end */
    while ((nonempty_lines>=0)&&((lvl->script.txt[nonempty_lines][0])=='\0'))
      nonempty_lines--;
    currline=lines_count-1;
    while (currline>nonempty_lines)
    {
      free(lvl->script.txt[currline]);
      currline--;
    }
    lines_count=nonempty_lines+1;
    lvl->script.txt=(char **)realloc(lvl->script.txt,lines_count*sizeof(unsigned char *));
    lvl->script.list=(struct DK_SCRIPT_COMMAND **)realloc(lvl->script.list,lines_count*sizeof(struct DK_SCRIPT_COMMAND *));
    lvl->script.lines_count=lines_count;
    memfile_free(&mem);
    decompose_script(&(lvl->script),&(lvl->optns.script));
    script_decomposed_to_params(&(lvl->script),&(lvl->optns.script));
    return ERR_NONE;
}

/**
 * Loads the LGT file fname, fills LEVEL light entries.
 * This _must_ be called _after_ tng_* are set up.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_lgt(struct LEVEL *lvl,char *fname)
{
    message_log("  load_lgt: started");
    unsigned char *stlight;
    if ((lvl==NULL)||(lvl->lgt_lookup==NULL))
      return ERR_INTERNAL;
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if (mem->len < SIZEOF_DK_LGT_HEADER)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }

    result=ERR_NONE;
    lvl->lgt_total_count=0;
    long lgt_num;
    lgt_num = read_int32_le_buf(mem->content+0);
    unsigned long expect_size=lgt_num*SIZEOF_DK_LGT_REC+SIZEOF_DK_LGT_HEADER;
    /* Check everything's cushty */
    if (mem->len != expect_size)
    {
        message_log("  load_lgt: File length %d, expected %lu (%d items)",mem->len,expect_size,lgt_num);
        /* Fixing the problem */
        if (((lvl->optns.load_redundant_objects&EXLD_STLGHT)==EXLD_STLGHT)||(mem->len < expect_size))
          lgt_num=(mem->len-SIZEOF_DK_LGT_HEADER)/SIZEOF_DK_LGT_REC;
        result=WARN_BAD_COUNT;
    }
    int i;
    for (i=0; i<lgt_num; i++)
    {
      stlight=(unsigned char *)malloc(SIZEOF_DK_LGT_REC);
      if (stlight==NULL)
      {
        message_error("Cannot allocate mem for loading static lights");
        return ERR_CANT_MALLOC;
      }
      memcpy (stlight, mem->content+SIZEOF_DK_LGT_REC*i+SIZEOF_DK_LGT_HEADER, SIZEOF_DK_LGT_REC);
      stlight_add(lvl,stlight);
    }
    if (lgt_num != lvl->lgt_total_count)
    {
        message_error("Internal error in load_lgt: lgt_num=%d lgt_total=%d", lgt_num, lvl->lgt_total_count);
        return ERR_INTERNAL;
    }
    memfile_free(&mem);
    return result;
}

/**
 * Reads the WLB file into LEVEL structure.
 * WLB seems to be unused by the game, but are always written by BF editor.
 * DK loads this file when starting a level.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_wlb(struct LEVEL *lvl,char *fname)
{
    message_log("  load_wlb: started");
    /*Reading file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /*If wrong filesize - don't load */
    if (mem->len != lvl->tlsize.x*lvl->tlsize.y)
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    int i,j;
    for (i=0;i<lvl->tlsize.y;i++)
      for (j=0;j<lvl->tlsize.x;j++)
      {
        int mempos=i*lvl->tlsize.x+j;
        lvl->wlb[j][i]=mem->content[mempos];
      }
    memfile_free(&mem);
    return ERR_NONE;
}

/**
 * Reads the SLX file into LEVEL structure.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_slx(struct LEVEL *lvl,char *fname)
{
    message_log("  load_slx: started");
    short result;
    FILE *F = fopen(fname, "rb");
    if (F == NULL)
        return ERR_FILE_BADDATA;
    if (1 != fread(lvl->slx_data, sizeof(lvl->slx_data), 1, F))
        return ERR_FILE_BADDATA;
    fclose(F);
    return ERR_NONE;
}

/**
 * Loads the FLG file.
 * These seems to have small priority, but DK loads it when starting a level.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname File name to load from.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_flg(struct LEVEL *lvl,char *fname)
{
    message_log("  load_flg: started");
    const unsigned int line_len=2*lvl->subsize.x;
    /*Loading the file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
        return result;
    /* Checking file size */
    if ((mem->len!=line_len*lvl->subsize.y))
    { memfile_free(&mem); return ERR_FILE_BADDATA; }
    /*Reading entries */
    int sx, sy;
    unsigned long addr;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      addr = sy*line_len;
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          set_subtl_flg(lvl,sx,sy,read_int16_le_buf(mem->content+addr+sx*2));
      }
    }
    memfile_free(&mem);
    return ERR_NONE;
}

/**
 * Loads the LIF file.
 * LIFs contain text name of the level (and level number, which is ignored).
 * @param lvl Pointer to the LEVEL structure.
 * @param fname File name to load from.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_lif(struct LEVEL *lvl,char *fname)
{
    message_log("  load_lif: started");
    short result;
    /* Load the file lines */
    char **lines=NULL;
    int lines_count=0;
    result=load_text_file(&lines,&lines_count,fname);
    if (result!=ERR_NONE)
    {
      return result;
    }
    /* Search for nonempty line */
    char *line_data=NULL;
    int currline;
    for (currline=0;currline<lines_count;currline++)
    {
        free(line_data);
        line_data=strdup_trim(lines[currline]);
        if ((line_data!=NULL)&&(line_data[0]!='\0')) break;
    }
    free_text_file(&lines,&lines_count);    
    /* Now we have the line we want in line_data. */
    char *namepos=NULL;
    if ((line_data!=NULL)&&(line_data[0]!='\0'))
        namepos=(char *)strchr(line_data,',');
    if ((namepos==NULL)||(strlen(namepos)<2))
    { free(line_data); return ERR_FILE_BADDATA; }
    /* Getting level name */
    char *str=strdup_trim(namepos+1);
    if (str!=NULL)
        set_lif_name_text(lvl,str);
    free(line_data);
    return ERR_NONE;
}

/**
 * Loads ADiKtEd script (.ADI file) from MEMORY_FILE executes all commands.
 * @param lvl Pointer to the LEVEL structure.
 * @param mem Source file data.
 * @param err_msg Error message text.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short script_load_and_execute(struct LEVEL *lvl,struct MEMORY_FILE *mem,char *err_msg)
{
    message_log(" script_load_and_execute: started");
    sprintf(err_msg,"No error");
    short result;
    /* Checking file size */
    if (mem->len < 2)
    {
        result=ERR_FILE_TOOSMLL;
        strncpy(err_msg,levfile_error(result),LINEMSG_SIZE);
        return result;
    }
    unsigned char *content=mem->content;
    unsigned char *ptr=mem->content;
    unsigned char *ptr_end=mem->content+mem->len;
    /* Counting lines */
    int lines_count=0;
    while (ptr>=content)
    {
      ptr=memchr(ptr, 0x0a, (char *)ptr_end-(char *)ptr );
      lines_count++;
      if (ptr!=NULL) ptr++;
    }
    ptr=mem->content;
    int currline=0;
    result=ERR_NONE;
    char *line;
    while (currline<lines_count)
    {
      if (ptr>=ptr_end) ptr=ptr_end-1;
      unsigned char *nptr=memchr(ptr, 0x0a, ptr_end-ptr );
      /*Skip control characters (but leave spaces and TABs) */
      while ((ptr<nptr)&&((unsigned char)ptr[0]<0x20)&&((unsigned char)ptr[0]!=0x09)) ptr++;
      if (nptr==NULL)
        nptr=ptr_end;
      int linelen=(char *)nptr-(char *)ptr;
      /*At end, skip control characters and spaces too */
      while ((linelen>0)&&((unsigned char)ptr[linelen-1]<=0x20)) linelen--;

      line=(unsigned char *)malloc((linelen+1)*sizeof(unsigned char *));
      memcpy(line,ptr,linelen);
      line[linelen]='\0';
      short res=execute_script_line(lvl,line,err_msg);
      if (!res)
      {
        sprintf(err_msg+strlen(err_msg),", line %d",currline+1);
        result=ERR_FILE_BADDATA;
      }
      free(line);
      ptr=nptr+1;
      currline++;
    }
    return result;
}

/**
 * Loads ADiKtEd script (.ADI file) and executes all commands.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Source file name.
 * @param err_msg Error message text.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short script_load_and_execute_file(struct LEVEL *lvl,char *fname,char *err_msg)
{
    message_log(" script_load_and_execute_file: started");
    /*Loading the file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
    {
        strncpy(err_msg,memfile_error(result),LINEMSG_SIZE);
        return result;
    }
    result=script_load_and_execute(lvl,mem,err_msg);
    memfile_free(&mem);
    return result;
}

/**
 * Loads any text file as lines. If input structure is not empty,
 * appends the loaded data at end of it.
 * @param lines Pointer to the text lines array.
 * @param lines_count Amount of lines in the array.
 * @param fname Source file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_text_file(char ***lines,int *lines_count,char *fname)
{
    message_log(" load_text_file: started");
    /*Loading the file */
    struct MEMORY_FILE *mem;
    short result;
    result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
    if (result != MFILE_OK)
    { return result; }
/*    message_log("  load_text_file: file readed"); */
    /*If filesize too small - pannic */
    if (mem->len < 2)
    { memfile_free(&mem); return ERR_FILE_TOOSMLL; }
    unsigned char *content=mem->content;
    unsigned char *ptr=mem->content;
    unsigned char *ptr_end=mem->content+mem->len;
/*    message_log("  load_text_file: counting lines"); */
    while (ptr>=content)
    {
      ptr=memchr(ptr, 0x0a, (char *)ptr_end-(char *)ptr );
      (*lines_count)++;
      if (ptr!=NULL) ptr++;
    }
    (*lines)=(char **)realloc((*lines),(*lines_count)*sizeof(unsigned char *));
    ptr=mem->content;
    int currline;
    currline=0;
/*    message_log("  load_text_file: breaking text into %d lines",(*lines_count)); */
    while (currline<(*lines_count))
    {
      if (ptr>=ptr_end) ptr=ptr_end-1;
      unsigned char *nptr=memchr(ptr, 0x0a, ptr_end-ptr );
      /*Skip control characters (but leave spaces and TABs) */
      while ((ptr<nptr)&&((unsigned char)ptr[0]<0x20)&&((unsigned char)ptr[0]!=0x09)) ptr++;
      if (nptr==NULL)
        nptr=ptr_end;
      int linelen=(char *)nptr-(char *)ptr;
      /*At end, skip control characters and spaces too */
      while ((linelen>0)&&((unsigned char)ptr[linelen-1]<=0x20)) linelen--;
      (*lines)[currline]=(unsigned char *)malloc((linelen+1)*sizeof(unsigned char));
      memcpy((*lines)[currline],ptr,linelen);
      (*lines)[currline][linelen]='\0';
      ptr=nptr+1;
      currline++;
    }
/*    message_log("  load_text_file: deleting empty lines"); */
    int nonempty_lines=(*lines_count)-1;
    /* Delete empty lines at end */
    while ((nonempty_lines>=0) && (((*lines)[nonempty_lines][0])=='\0'))
      nonempty_lines--;
    currline=(*lines_count)-1;
    while (currline>nonempty_lines)
    {
      free((*lines)[currline]);
      currline--;
    }
    (*lines_count)=nonempty_lines+1;
    (*lines)=(char **)realloc((*lines),(*lines_count)*sizeof(unsigned char *));
    memfile_free(&mem);
    return ERR_NONE;
}

/**
 * Writes the SLB file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_slb(struct LEVEL *lvl,char *fname)
{
    message_log(" write_slb: starting");
    FILE *fp;
    int i, k;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    for (k=0; k < lvl->tlsize.y; k++)
    {
      for (i=0; i < lvl->tlsize.x; i++)
      {
          write_int16_le_file(fp,get_tile_slab(lvl,i,k));
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the OWN file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_own(struct LEVEL *lvl,char *fname)
{
    message_log(" write_own: starting");
    /*Opening */
    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    /*Writing data */
    int sx,sy;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          fputc (get_subtl_owner(lvl,sx,sy), fp);
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the DAT file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_dat(struct LEVEL *lvl,char *fname)
{
    message_log(" write_dat: starting");
    /*const unsigned int line_len=2*lvl->subsize.x;*/

    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    /*Writing data */
    int sx,sy;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          write_int16_le_file(fp,get_dat_val(lvl,sx,sy));
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the FLG file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_flg(struct LEVEL *lvl,char *fname)
{
    message_log(" write_flg: starting");
    /*const unsigned int line_len=2*lvl->subsize.x;*/

    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    /*Writing data */
    int sx,sy;
    for (sy=0; sy<lvl->subsize.y; sy++)
    {
      for (sx=0; sx<lvl->subsize.x; sx++)
      {
          write_int16_le_file(fp,get_subtl_flg(lvl,sx,sy));
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the CLM file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_clm(struct LEVEL *lvl,char *fname)
{
    message_log(" write_clm: starting");
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    write_int32_le_buf(lvl->clm_hdr+0,COLUMN_ENTRIES);
    fwrite(lvl->clm_hdr, SIZEOF_DK_CLM_HEADER, 1, fp);
    for (i=0; i<COLUMN_ENTRIES; i++)
      fwrite(lvl->clm[i], SIZEOF_DK_CLM_REC, 1, fp);
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the WIB file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_wib(struct LEVEL *lvl,char *fname)
{
    message_log(" write_wib: starting");

    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    int i, j;
    for (i=0; i < lvl->subsize.y; i++)
    {
      for (j=0; j<lvl->subsize.x; j++)
          fputc (get_subtl_wib(lvl,j,i), fp);
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the APT file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_apt(struct LEVEL *lvl,char *fname)
{
    message_log(" write_apt: starting");
    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;

    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    write_int32_le_file (fp,lvl->apt_total_count);
    int cy, cx, k;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=get_actnpt_subnums(lvl,cx,cy);
          for (k=0; k<num_subs; k++)
          {
                char *actnpt=get_actnpt(lvl,cx,cy,k);
                fwrite (actnpt, SIZEOF_DK_APT_REC, 1, fp);
          }
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the TNG file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_tng(struct LEVEL *lvl,char *fname)
{
    message_log(" write_tng: starting");
    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;

    FILE *fp;
    int cx, cy, k;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    /*Header */
    write_int16_le_file(fp, lvl->tng_total_count);
    /*Entries */
    for (cy=0; cy < arr_entries_y; cy++)
      for (cx=0; cx < arr_entries_x; cx++)
          for (k=0; k < get_thing_subnums(lvl,cx,cy); k++)
                fwrite (get_thing(lvl,cx,cy,k), SIZEOF_DK_TNG_REC, 1, fp);
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the INF file from LEVEL structure into disk.
 * One byte file - the easy one.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_inf(struct LEVEL *lvl,char *fname)
{
    message_log(" write_inf: starting");
    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    fputc ((lvl->inf) & 255, fp);
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the VSN file from LEVEL structure into disk.
 * One byte file - the easy one.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_vsn(struct LEVEL *lvl,char *fname)
{
    message_log(" write_vsn: starting");
    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    unsigned char vsn;
    switch (lvl->format_version)
    {
    case MFV_DKSTD:
    case MFV_DKGOLD:
         vsn=1;
         break;
    case MFV_DKXPAND:
         vsn=2;
         break;
    default:
         vsn=0;
         break;
    }
    fputc (vsn, fp);
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the TXT script file from LEVEL structure into disk.
 * @see write_text_file
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_txt(struct LEVEL *lvl,char *fname)
{
    message_log(" write_txt: starting");
    return write_text_file(lvl->script.txt,lvl->script.lines_count,fname);
}

/**
 * Writes the LGT file from LEVEL structure into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_lgt(struct LEVEL *lvl,char *fname)
{
    message_log(" write_lgt: starting");
    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;

    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    write_int32_le_file (fp,lvl->lgt_total_count);
    int cy, cx, k;
    for (cy=0; cy<arr_entries_y; cy++)
    {
      for (cx=0; cx<arr_entries_x; cx++)
      {
          int num_subs=get_stlight_subnums(lvl,cx,cy);
          for (k=0; k<num_subs; k++)
          {
                char *stlight=get_stlight(lvl,cx,cy,k);
                fwrite (stlight, SIZEOF_DK_LGT_REC, 1, fp);
          }
      }
    }
    fclose (fp);
    return ERR_NONE;
}

/**
 * Writes the WLB file from LEVEL structure into disk.
 * Water-Lava Block files are used to define what is under a bridge.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_wlb(struct LEVEL *lvl,char *fname)
{
    message_log(" write_wlb: starting");
    FILE *fp;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    int i, j;
    for (i=0; i < lvl->tlsize.y; i++)
    {
      for (j=0; j < lvl->tlsize.x; j++)
          fputc(lvl->wlb[j][i], fp);
    }
    fclose(fp);
    return ERR_NONE;
}

/**
 * Wrapper around 'strrchr()'. Tries to find last occurrence of 'item' in 'buffer.
 * @return Returns pointer to last 'item' in 'buffer' on success, otherwise 'buffer'.
 */
static char* find_last( char *buffer, const char item )
{
    char *found = strrchr( buffer, item );
    if ( found != NULL ) {
        return found;
    }
    return buffer;
}

/**
 * Writes the LIF file from LEVEL structure into disk.
 * LIFs are used to save text name of the level.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_lif(struct LEVEL *lvl,char *fname)
{
  message_log(" write_lif: starting");
  /*Acquiring map number */
  long lvl_num;
  char *fname_num = fname;

  /* Separating filename from directory subpath */
  fname_num = find_last( fname_num, '/' );                 /* Linux path separator */
  fname_num = find_last( fname_num, '\\' );                /* Windows path separator */

  while ( ((*fname_num)!='\0') && (!isdigit(*fname_num)) ) fname_num++;
  lvl_num=atol(fname_num);
  /*Creating text lines */
  char **lines=(char **)malloc(2*sizeof(char *));
  int lines_count=0;
  lines[lines_count]=malloc(LINEMSG_SIZE+1);
  sprintf(lines[lines_count],"%02lu, %s",lvl_num,get_lif_name_text(lvl));
  lines_count++;
  /*Writing data */
  short result;
  result=write_text_file(lines,lines_count,fname);
  text_file_free(lines,lines_count);
  return result;
}

/**
 * Writes the LOF file from LEVEL structure into disk.
 * LOFs are used to save text name of the level and some extra data like mapsize etc
 * only in extended format.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_lof(struct LEVEL *lvl,char *fname)
{
    message_log(" write_lof: starting");

    /*Creating text lines */
    char **lines=(char **)malloc(13*sizeof(char *));
    int lines_count=0;
    for (size_t i = 0; i < 13; i++)
    {
        lines[i]=malloc(LINEMSG_SIZE+1);
    }
    
    sprintf(lines[lines_count],"; KeeperFX Level Overview File (LOF)");
    lines_count++;
    char* nameText = get_lif_name_text(lvl);
    sprintf(lines[lines_count],"NAME_TEXT = %s",nameText);
    lines_count++;
    char* nameId = "";
    sprintf(lines[lines_count],"NAME_ID = %s",nameId);
    lines_count++;
    char* kind = "FREE";
    sprintf(lines[lines_count],"KIND = %s",kind);
    lines_count++;
    char* ensignPos = "";
    sprintf(lines[lines_count],"ENSIGN_POS = %s",ensignPos);
    lines_count++;
    char* ensignZoom = "";
    sprintf(lines[lines_count],"ENSIGN_ZOOM = %s",ensignZoom);
    lines_count++;
    char* Players = "";
    sprintf(lines[lines_count],"PLAYERS = %s",Players);
    lines_count++;
    char* Options = "";
    sprintf(lines[lines_count],"OPTIONS = %s",Options);
    lines_count++;
    char* Speech = "";
    sprintf(lines[lines_count],"SPEECH = %s",Speech);
    lines_count++;
    char* landView = "";
    sprintf(lines[lines_count],"LAND_VIEW = %s",landView);
    lines_count++;
    char* author = lvl->info.author_text;
    if (author==NULL) author="";
    sprintf(lines[lines_count],"AUTHOR = %s",author);
    lines_count++;
    char* description = lvl->info.desc_text;
    if (description==NULL) description="";
    sprintf(lines[lines_count],"DESCRIPTION = %s",description);
    lines_count++;
    sprintf(lines[lines_count],"MAPSIZE = %d %d",lvl->tlsize.x,lvl->tlsize.y);
    lines_count++;

    short result;
    result=write_text_file(lines,lines_count,fname);
    text_file_free(lines,lines_count);
    return result;

}

/**
 * Writes the ADI script file from LEVEL structure into disk.
 * Creates the file data first it - is not stored directly in LEVEL.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_adi_script(struct LEVEL *lvl,char *fname)
{
  message_log(" write_adi_script: starting");
  /*Creating text lines */
  char **lines=NULL;
  int lines_count=0;
  add_stats_to_script(&lines,&lines_count,lvl);
  add_graffiti_to_script(&lines,&lines_count,lvl);
  add_custom_clms_to_script(&lines,&lines_count,lvl);
  short result;
  result=write_text_file(lines,lines_count,fname);
  text_file_free(lines,lines_count);
  return result;
}

/**
 * Writes the NFO level information file into disk.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_nfo(struct LEVEL *lvl,char *fname)
{
  message_log(" write_nfo: starting");
  /*Creating text lines */
  char **lines=NULL;
  int lines_count=0;
    char *line;
    char *tmp1;
    int plr_items[PLAYERS_COUNT];
    int i,k,imax;
    line=(char *)malloc(LINEMSG_SIZE*sizeof(char));

    sprintf(line,"Name: %s %d.%d.%d",get_lif_name_text(lvl),
        lvl->info.ver_major,lvl->info.ver_minor,lvl->info.ver_rel);
    text_file_linecp_add(&lines,&lines_count,line);
    if ((lvl->info.author_text!=NULL)&&(lvl->info.author_text[0]!='\0'))
        tmp1=lvl->info.author_text;
    else
        tmp1="Anonymous";
    sprintf(line,"Author: %s",tmp1);
    if ((lvl->info.editor_text!=NULL)&&(lvl->info.editor_text[0]!='\0'))
        sprintf(line+strlen(line),", Editor: %s",lvl->info.editor_text);
    strcat(line,", Created on ");
    strftime(line+strlen(line),LINEMSG_SIZE/2, "%d %b %Y",
        gmtime(&lvl->info.creat_date) );
    text_file_linecp_add(&lines,&lines_count,line);
    strcpy(line,"Keepers: ");
    /*Clearing array for storing players heart count */
    for (i=0; i < PLAYERS_COUNT; i++)
      plr_items[i]=0;
    owned_things_count(plr_items,lvl,THING_TYPE_ITEM,ITEM_SUBTYPE_DNHEART);
    k=0;
    for (i=0; i < PLAYERS_COUNT; i++)
      if (plr_items[i]>0)
      {
          if (k) strcat(line,", ");
          strcat(line,get_owner_type_colorname(i));
          k=1;
      }
    text_file_linecp_add(&lines,&lines_count,line);
    strcpy(line,"Pool: ");
    imax=creatures_cmd_arrsize();
    k=0;
    for (i=0; i < imax; i++)
      if (lvl->script.par.creature_pool[i]>0)
      {
          if (k) strcat(line,", ");
          strcat(line,get_creature_subtype_fullname(i));
          k=1;
          if (strlen(line)>50)
          {
            text_file_linecp_add(&lines,&lines_count,line);
            strcpy(line,"  ");
            k=0;
          }
      }
    if (strlen(line)>2)
      text_file_linecp_add(&lines,&lines_count,line);
    strcpy(line,"Objects on map:");
    text_file_linecp_add(&lines,&lines_count,line);
    sprintf(line,"Creatures: %d, Traps: %d, Doors: %d, Items: %d",
        lvl->stats.creatures_count,lvl->stats.traps_count,
        lvl->stats.doors_count,lvl->stats.items_count);
    text_file_linecp_add(&lines,&lines_count,line);
    sprintf(line,"Dungeon hearts: %d, Hero gates: %d, Special Boxes: %d",
        lvl->stats.dn_hearts_count,lvl->stats.hero_gates_count,
        lvl->stats.things_count[THING_CATEGR_SPECIALBOX]);
    text_file_linecp_add(&lines,&lines_count,line);
    strcpy(line,"Description:");
    text_file_linecp_add(&lines,&lines_count,line);
    if ((lvl->info.desc_text!=NULL)&&(lvl->info.desc_text[0]!='\0'))
        tmp1=lvl->info.desc_text;
    else
        tmp1="No description.";
    imax=strlen(tmp1);
    do {
      i=50;
      strcpy(line,"  ");
      strncpy(line+2,tmp1,i);
      line[i+2]='\0';
      k=(strrchr(line,' ')-line);
      if (k>(i>>1))
        {i=k;line[i]='\0';}
         text_file_linecp_add(&lines,&lines_count,line);
      i--;
      tmp1+=i;
      imax-=i;
    } while (imax>i);
    if (imax>0)
    {
      strcpy(line,"  ");
      strcpy(line+2,tmp1);
      text_file_linecp_add(&lines,&lines_count,line);
    }
    free(line);
  short result;
  result=write_text_file(lines,lines_count,fname);
  text_file_free(lines,lines_count);
  return result;
}

/**
 * Saves any text file.
 * @param lines Pointer to the lines array.
 * @param lines_count Lines count.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_text_file(char **lines,int lines_count,char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "wb");
    if (fp==NULL)
      return ERR_CANT_OPENWR;
    int last_line=lines_count-1;
    for (i=0;i<last_line;i++)
    {
      fputs(lines[i],fp);
      fputs("\r\n",fp);
    }
    if (last_line>=0)
    {
      fputs(lines[last_line],fp);
      if (lines[last_line][0] != '\0')
        fputs("\r\n",fp);
    }
    fclose(fp);
    return ERR_NONE;
}

/**
 * Writes extended info
 * SLX consist of extended slab flags (i.e. tileset)
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short write_slx(struct LEVEL *lvl,char *fname)
{
  message_log(" write_slx: starting");

  FILE* fp = fopen(fname, "wb");
  if (fp==NULL)
    return ERR_CANT_OPENWR;
  fwrite(lvl->slx_data, lvl->tlsize.x * lvl->tlsize.y, 1, fp);
  fclose(fp);
  return ERR_NONE;
}

/**
 * Saves any map file, showing error/warning message if it is required.
 * @param lvl Pointer to the LEVEL structure.
 * @param fext Extension of destination file name.
 * @param write_file The writing function.
 * @param saved_files Saved files counter. Incremented if save is successful.
 * @param result Result value. Set to error code if error occures, otherwise left unchanged.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short save_mapfile(struct LEVEL *lvl,char *mfname,char *fext,mapfile_io_func write_file,int *saved_files,short *result)
{
  short file_result;
  char *fname;
  fname = (char *)malloc(strlen(mfname)+5);
  if (fname==NULL)
  {
      message_error("save_mapfile: Out of memory");
      file_result=ERR_CANT_MALLOC;
      (*result)=file_result;
      return file_result;
  }
  sprintf (fname, "%s.%s", mfname,fext);
  file_result=write_file(lvl,fname);
  if (file_result==ERR_NONE)
  {
      (*saved_files)++;
  } else
  if (file_result<ERR_NONE)
  {
      message_error("Error: %s when saving \"%s\"",levfile_error(file_result), fname);
      (*result)=file_result;
  } else
  if (file_result>ERR_NONE)
  {
      char *ifname;
      ifname=prepare_short_fname(fname,24);
      message_info_force("Warning: %s when saving \"%s\"",levfile_error(file_result), ifname);
      free(ifname);
      (*saved_files)++;
      if ((*result)>=ERR_NONE)
          (*result)=file_result;
  }
  free(fname);
  return file_result;
}

/**
 * Saves the whole map. Includes all files editable in ADiKtEd.
 * On failure, tries to save at least some of the files.
 * Does not perform an update before saving - to do this, use
 * user_save_map() instead.
 * @see user_save_map
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, last error code on failure.
 */
short save_dk1_map(struct LEVEL *lvl)
{
    message_log(" save_dk1_map: started");

    short result=ERR_NONE;
    int saved_files=0;
    int total_files=0;
    save_mapfile(lvl,lvl->savfname,"slb",write_slb,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"own",write_own,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"dat",write_dat,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"clm",write_clm,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"tng",write_tng,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"apt",write_apt,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"wib",write_wib,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"inf",write_inf,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"txt",write_txt,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"lgt",write_lgt,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"wlb",write_wlb,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"flg",write_flg,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"lif",write_lif,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"vsn",write_vsn,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"adi",write_adi_script,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"slx",write_slx,&saved_files,&result);
    total_files++;

    if ((result==ERR_NONE)||(strlen(lvl->fname)<1))
    {
      strncpy(lvl->fname,lvl->savfname,DISKPATH_SIZE);
      lvl->fname[DISKPATH_SIZE-1]=0;
    }
    lvl->stats.saves_count++;
    message_log(" save_dk1_map: properly saved %d out of %d map files",saved_files,total_files);
    return result;
}

short save_nfo_file(struct LEVEL *lvl)
{
    message_log(" save_dk1_map: started");
    int saved_files=0;
    short result=ERR_NONE;
    return save_mapfile(lvl,lvl->fname,"nfo",write_nfo,&saved_files,&result);
}

/**
 * Saves the whole DK Extended map. Includes all files editable in ADiKtEd.
 * On failure, tries to save at least some of the files.
 * DK Extended map files won't load in standard Dungeon Keeper on DD.
 * Does not perform an update before saving - to do this, use
 * user_save_map() instead.
 * @see user_save_map
 * @see save_dk1_map
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, last error code on failure.
 */
short save_dke_map(struct LEVEL *lvl)
{
    message_log(" save_dke_map: started");

    short result=ERR_NONE;
    int saved_files=0;
    int total_files=0;

    save_mapfile(lvl,lvl->savfname,"slb",write_slb,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"own",write_own,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"dat",write_dat,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"clm",write_clm,&saved_files,&result);
    total_files++;
    //TODO replace with tngfx
    save_mapfile(lvl,lvl->savfname,"tng",write_tng,&saved_files,&result);
    total_files++;
    //TODO replace with aptfx
    save_mapfile(lvl,lvl->savfname,"apt",write_apt,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"wib",write_wib,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"inf",write_inf,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"txt",write_txt,&saved_files,&result);
    total_files++;
    //TODO replace with lgtfx
    save_mapfile(lvl,lvl->savfname,"lgt",write_lgt,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"wlb",write_wlb,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"flg",write_flg,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"lof",write_lof,&saved_files,&result);
    total_files++;

    save_mapfile(lvl,lvl->savfname,"adi",write_adi_script,&saved_files,&result);
    total_files++;
    save_mapfile(lvl,lvl->savfname,"slx",write_slx,&saved_files,&result);
    total_files++;

    if ((result==ERR_NONE)||(strlen(lvl->fname)<1))
    {
      strncpy(lvl->fname,lvl->savfname,DISKPATH_SIZE);
      lvl->fname[DISKPATH_SIZE-1]=0;
    }
    lvl->stats.saves_count++;
    message_log(" save_dke_map: properly saved %d out of %d map files",saved_files,total_files);
    return result;
}

/**
 * Saves the whole map. Includes all files editable in ADiKtEd.
 * On failure, tries to save at least some of the files.
 * Makes updates required for saving before it starts.
 * Also, fails if the map verification will return serious error.
 * Should be used instead of save_dk1_map(), as it prepares the map
 * before the saving operation is performed.
 * @see save_dk1_map
 * @param lvl Pointer to the LEVEL structure.
 * @param prior_save Informs how to increase level version before saving.
 *     Prior saves get greater version increase than minor saves.
 * @return Returns ERR_NONE on success, last error code on failure.
 */
short user_save_map(struct LEVEL *lvl,short prior_save)
{
  short result;
  struct IPOINT_2D errpt={-1,-1};
  if (level_verify(lvl,"save",&errpt)==VERIF_ERROR)
    return ERR_VERIF;

  /*Once there was an CLM/DAT/TNG update function here, */
  /* but the new way is to minimize file changes - so it's been removed */
  update_slab_owners(lvl);
  struct LEVSTATS *stats=get_lvl_stats(lvl);
  if (stats->saves_count==0)
  {
      if (prior_save)
        inc_info_ver_major(lvl);
      else
        inc_info_ver_minor(lvl);
  } else
  {
      if (prior_save)
        inc_info_ver_minor(lvl);
      else
        inc_info_ver_rel(lvl);
  }
  switch (lvl->format_version)
  {
  case MFV_DKSTD:
  case MFV_DKGOLD:
      result=save_dk1_map(lvl);
      break;
  case MFV_DKXPAND:
      result=save_dke_map(lvl);
      break;
  default:
      message_error("Error: Save not supported for this map format version");
      result=ERR_INTERNAL;
      break;
  }
  return result;
}

/**
 * Loads any map file, showing error/warning message if it is required.
 * @param lvl Pointer to the LEVEL structure.
 * @param fext Extension of destination file name.
 * @param load_file The loading function.
 * @param loaded_files Saved files counter. Incremented if load is successful.
 * @param result Result value. Set to error code if error occures, otherwise left unchanged.
 * @param flags Load flags. Allows to ignore some errors.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_mapfile(struct LEVEL *lvl,char *fext,mapfile_io_func load_file,
    int *loaded_files,short *result,short flags)
{
  short file_result;
  char *fname;
  message_log("load_mapfile: loading %s file",fext);
  fname = (char *)malloc(strlen(lvl->fname)+strlen(fext)+3);
  if (fname==NULL)
  {
      file_result=ERR_CANT_MALLOC;
      if (flags&LFF_IGNORE_INTERNAL)
      {
          message_log("load_mapfile: Out of memory");
      } else
      {
          message_error("load_mapfile: Out of memory");
          (*result)=file_result;
      }
      return file_result;
  }
  sprintf(fname, "%s.%s", lvl->fname, fext);
  file_result=load_file(lvl,fname);
  /*message_log("load_mapfile: Load function execution finished"); */
  if (file_result==ERR_NONE)
  {
      (*loaded_files)++;
  } else
  if (file_result<ERR_NONE)
  {
      if (flags&LFF_IGNORE_CANNOT_LOAD)
      {
          if (((*result)==ERR_NONE)&&(!(flags&LFF_DONT_EVEN_WARN)))
            message_info_force("Warning: %s when loading \"%s\"",levfile_error(file_result), fname);
      } else
      {
          message_error("Error: %s when loading \"%s\"",levfile_error(file_result), fname);
          (*result)=file_result;
      }
  } else
  if (file_result>ERR_NONE)
  {
      char *ifname;
      ifname=prepare_short_fname(fname,24);
      if (flags&LFF_IGNORE_CANNOT_LOAD)
      {
          if ((*result)==ERR_NONE)
            message_info_force("Warning: %s when loading \"%s\"",levfile_error(file_result), ifname);
      } else
      {
          message_info_force("Warning: %s when loading \"%s\"",levfile_error(file_result), ifname);
          if ((*result)>=ERR_NONE)
              (*result)=file_result;
      }
      free(ifname);
      (*loaded_files)++;
  }
  free(fname);
  return file_result;
}

/**
 * Loads map file which loading function uses additional error message parameter.
 * Shows error/warning message if it is required.
 * @param lvl Pointer to the LEVEL structure.
 * @param fext Extension of destination file name.
 * @param load_file The loading function.
 * @param loaded_files Saved files counter. Incremented if load is successful.
 * @param result Result value. Set to error code if error occures, otherwise left unchanged.
 * @param flags Load flags. Allows to ignore some errors.
 * @return Returns ERR_NONE on success, error code on failure.
 */
short load_mapfile_msg(struct LEVEL *lvl,char *fext,mapfile_iomsg_func load_file,
    int *loaded_files,short *result,short flags)
{
  short file_result;
  char *fname;
  char *err_msg;
  message_log("load_mapfile_msg: loading %s file",fext);
  fname = (char *)malloc(strlen(lvl->fname)+strlen(fext)+3);
  err_msg=(char *)malloc(LINEMSG_SIZE);
  if ((fname==NULL)||(err_msg==NULL))
  {
      file_result=ERR_CANT_MALLOC;
      if (flags&LFF_IGNORE_INTERNAL)
      {
          message_log("load_mapfile_msg: Out of memory");
      } else
      {
          message_error("load_mapfile_msg: Out of memory");
          (*result)=file_result;
      }
      free(fname);
      free(err_msg);
      return file_result;
  }
  sprintf(fname, "%s.%s", lvl->fname, fext);
  err_msg[0]='\0';
  /*Loading the file */
  struct MEMORY_FILE *mem;
  file_result = memfile_readnew(&mem,fname,MAX_FILE_SIZE);
  if (file_result != MFILE_OK)
  {
      if (flags&LFF_IGNORE_CANNOT_LOAD)
      {
          if (flags&LFF_DONT_EVEN_WARN)
            message_log(" load_mapfile_msg: %s when reading \"%s\"",memfile_error(file_result), fname);
          else
            message_info_force("Warning: %s when reading \"%s\"",memfile_error(file_result), fname);
      } else
      {
          message_error("Error: %s when reading \"%s\"",memfile_error(file_result), fname);
          (*result)=file_result;
      }
      free(fname);
      free(err_msg);
      return file_result;
  }
  file_result=load_file(lvl,mem,err_msg);
  memfile_free(&mem);
  /*message_log("load_mapfile: Load function execution finished"); */
  if (file_result==ERR_NONE)
  {
      (*loaded_files)++;
  } else
  if (file_result<ERR_NONE)
  {
      /* Special condition for ADI file loading. */
      /* Ignore ADI load errors other than "bad data" - ADI file don't have to exist */
      if ((load_file==script_load_and_execute)&&(file_result==ERR_FILE_BADDATA))
      {
        char *warn_fname;
        warn_fname=prepare_short_fname(fname,24);
        message_info_force("ADI script warning: %s in \"%s\" (ignored)", err_msg, warn_fname);
        free(warn_fname);
      } else
      if (flags&LFF_IGNORE_CANNOT_LOAD)
      {
          if (((*result)==ERR_NONE)&&(!(flags&LFF_DONT_EVEN_WARN)))
            message_info_force("Warning: %s when loading \"%s\"",err_msg, fname);
      } else
      {
          message_error("Error: %s when loading \"%s\"",err_msg, fname);
          (*result)=file_result;
      }
  } else
  if (file_result>ERR_NONE)
  {
      char *warn_fname;
      warn_fname=prepare_short_fname(fname,24);
      if (flags&LFF_IGNORE_CANNOT_LOAD)
      {
          if ((*result)==ERR_NONE)
            message_info_force("Warning: %s when loading \"%s\"",err_msg, warn_fname);
      } else
      {
          message_info_force("Warning: %s when loading \"%s\"",err_msg, warn_fname);
          if ((*result)>=ERR_NONE)
              (*result)=file_result;
      }
      free(warn_fname);
      (*loaded_files)++;
  }
  free(err_msg);
  free(fname);
  return file_result;
}

/**
 * Loads the whole map. Tries to open all files editable in ADiKtEd.
 * Accepts failure for less importand files. Makes no stats update.
 * If you want a complete loading function, use user_load_map() instead.
 * @see user_load_map
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, negative error code on error.
 */
short load_dk1_map(struct LEVEL *lvl)
{
  message_log(" load_dk1_map: started");
  short result=ERR_NONE;
  level_free(lvl);
  level_clear(lvl);
  if ((lvl->fname==NULL)||(strlen(lvl->fname)<1))
  {
    result=ERR_FILE_BADNAME;
    message_error("Error: Can't load - bad map name");
    return result;
  }
  int loaded_files=0;
  /*int total_files=0;
  short file_result;*/
  /* Crucial files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"slb",load_slb,&loaded_files,&result,LFF_IGNORE_NONE);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"own",load_own,&loaded_files,&result,LFF_IGNORE_NONE);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"tng",load_tng,&loaded_files,&result,LFF_IGNORE_NONE);
  /* Less importand files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"dat",load_dat,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"apt",load_apt,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"lgt",load_lgt,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"clm",load_clm,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"wib",load_wib,&loaded_files,&result,LFF_IGNORE_ALL);

  /* Least importand files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"txt",load_txt,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"inf",load_inf,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"wlb",load_wlb,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"flg",load_flg,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"lif",load_lif,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"vsn",load_vsn,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"slx",load_slx,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile_msg(lvl,"adi",script_load_and_execute,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);

  if (result<ERR_NONE)
  {
      message_log(" load_dk1_map: failed");
      return result;
  }
  if ((result>=ERR_NONE)&&(strlen(lvl->savfname)<1))
  {
      strncpy(lvl->savfname,lvl->fname,DISKPATH_SIZE);
      lvl->savfname[DISKPATH_SIZE-1]=0;
  }
  message_log(" load_dk1_map: finished");
  return result;
}

/**
 * Loads the whole map. Tries to open all files editable in ADiKtEd.
 * Accepts failure for less importand files. Makes no stats update.
 * If you want a complete loading function, use user_load_map() instead.
 * DK Extended map files won't work in standard Dungeon Keeper on DD.
 * @see user_load_map
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, negative error code on error.
 */
short load_dke_map(struct LEVEL *lvl)
{
  message_log(" load_dke_map: started");
  short result=ERR_NONE;
  level_free(lvl);
  level_clear(lvl);
  if ((lvl->fname==NULL)||(strlen(lvl->fname)<1))
  {
    result=ERR_FILE_BADNAME;
    message_error("Error: Can't load - bad map name");
    return result;
  }
  int loaded_files=0;
  /*int total_files=0;
  short file_result;*/

  message_error("Error: Load not supported for extended map format");
  result=ERR_INTERNAL;
#if 0
  /* Crucial files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"slb",load_slb,&loaded_files,&result,LFF_IGNORE_NONE);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"own",load_own,&loaded_files,&result,LFF_IGNORE_NONE);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"tngfx",load_tngfx,&loaded_files,&result,LFF_IGNORE_NONE);
  /* Less importand files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"dat",load_dat,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"aptfx",load_aptfx,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"lgtfx",load_lgtfx,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"clm",load_clm,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"wib",load_wib,&loaded_files,&result,LFF_IGNORE_ALL);

  /* Least importand files */
  if (result>=ERR_NONE)
      load_mapfile(lvl,"txt",load_txt,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"inf",load_inf,&loaded_files,&result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"wlb",load_wlb,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"flg",load_flg,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"lof",load_lof,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
#endif
  if (result>=ERR_NONE)
      load_mapfile(lvl,"vsn",load_vsn,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile(lvl,"slx",load_slx,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result>=ERR_NONE)
      load_mapfile_msg(lvl,"adi",script_load_and_execute,&loaded_files,&result,LFF_IGNORE_WITHOUT_WARN);
  if (result<ERR_NONE)
  {
      message_log(" load_dke_map: failed");
      return result;
  }
  if ((result>=ERR_NONE)&&(strlen(lvl->savfname)<1))
  {
      strncpy(lvl->savfname,lvl->fname,DISKPATH_SIZE);
      lvl->savfname[DISKPATH_SIZE-1]=0;
  }
  message_log(" load_dke_map: finished");
  return result;
}

/**
 * Loads the whole map. Tries to open all files editable in Adikted.
 * Accepts failure for less importand files.
 * @param lvl Pointer to the LEVEL structure.
 * @param new_on_error If nonzero, the function will automatically start new map on load error.
 * @return Returns ERR_NONE on success, on error starts new map and returns negative error code.
 */
short user_load_map(struct LEVEL *lvl,short new_on_error)
{
  short result;
  switch (lvl->format_version)
  {
  case MFV_DKSTD:
  case MFV_DKGOLD:
      result=load_dk1_map(lvl);
      break;
  case MFV_DKXPAND:
      result=load_dke_map(lvl);
      break;
  default:
      message_error("Error: Load not supported for this map format version");
      result=ERR_INTERNAL;
      break;
  }
  if ((result<ERR_NONE)&&(new_on_error))
  {
    free_map(lvl);
    start_new_map(lvl);
  }
  update_level_stats(lvl);
  return result;
}

/**
 * Loads the map preview. Tries to open only files needed for Slab mode preview.
 * If you want a complete loading function, use user_load_map() instead.
 * @see user_load_map
 * @param lvl Pointer to the LEVEL structure.
 * @return Returns ERR_NONE on success, or error code on failure.
 *    If an error occures, the LEVEL may contain partially loaded data.
 */
short load_map_preview(struct LEVEL *lvl)
{
  short result=ERR_NONE;
  level_free(lvl);
  level_clear(lvl);
  if ((lvl->fname==NULL)||(strlen(lvl->fname)<1))
  {
    result=ERR_FILE_BADNAME;
    message_error("load_map_preview: Bad file name");
    return result;
  }
  int loaded_files=0;
  short global_result=ERR_NONE;
  if (result>=ERR_NONE)
      result=load_mapfile(lvl,"slb",load_slb,&loaded_files,&global_result,LFF_IGNORE_ALL);
  if (result>=ERR_NONE)
      result=load_mapfile(lvl,"own",load_own,&loaded_files,&global_result,LFF_IGNORE_ALL);
  if (strlen(lvl->savfname)<1)
  {
      strncpy(lvl->savfname,lvl->fname,DISKPATH_SIZE);
      lvl->savfname[DISKPATH_SIZE-1]=0;
  }
  update_level_stats(lvl);
  return result;
}

/**
 * Utility function for reverse engineering the CLM format.
 * Used in rework mode.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns true on success, false otherwise.
 */
short write_def_clm_source(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i;
    fp = fopen (fname, "w");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }

    /*Preparing array bounds */
    /*const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;*/

    unsigned char *clmentry;
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    for (i=0; i<COLUMN_ENTRIES; i++)
      {
        clmentry = (unsigned char *)(lvl->clm[i]);
        get_clm_entry(clm_rec, clmentry);
        /*int csum=read_int16_le_buf(clmentry+2);*/
/*        clm_rec->c[0]+clm_rec->c[1]+clm_rec->c[2]+clm_rec->c[3]+
            clm_rec->c[4]+clm_rec->c[5]+clm_rec->c[6]+clm_rec->c[7]+
            clm_rec->base;*/
/*        if ((lvl->clm_utilize[i])!=(clm_rec->use)) */
        {
/*          char binstr0[9];
          char binstr1[9];
          char binstr2[9];
          char binstr3[9];
          char binstr4[9];
          char binstr5[9];
          for (k=0;k<8;k++)
            binstr0[k]='0'+((clmentry[0]&(1<<k))!=0);
          binstr0[8]=0;
          for (k=0;k<8;k++)
            binstr1[k]='0'+((clmentry[1]&(1<<k))!=0);
          binstr1[8]=0;
          for (k=0;k<8;k++)
            binstr2[k]='0'+((clmentry[2]&(1<<k))!=0);
          binstr2[8]=0;
          for (k=0;k<8;k++)
            binstr3[k]='0'+((clmentry[3]&(1<<k))!=0);
          binstr3[8]=0;
          for (k=0;k<8;k++)
            binstr4[k]='0'+((clmentry[4]&(1<<k))!=0);
          binstr4[8]=0;
          for (k=0;k<8;k++)
            binstr5[k]='0'+((use_cntr[i]&(1<<k))!=0);
          binstr5[8]=0;
          fprintf(fp,"%4d   %s %s(%5d) %s %s %s (%s=%d)\n",i,binstr0,binstr1,clm_rec->use,binstr2,binstr3,binstr4,binstr5,use_cntr[i]);
*/
          fprintf(fp,"COLUMN(%4d,%5d,%2d,%2d,%2d, 0x%02x, 0x%03x,%2d,",
           i,(unsigned short)(clm_rec->use), clm_rec->permanent, clm_rec->lintel,
           clm_rec->height, clm_rec->solid, clm_rec->base, clm_rec->orientation);
            fprintf(fp,"    0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x) u=%d u0=%d\n",
              clm_rec->c[0],clm_rec->c[1],clm_rec->c[2],clm_rec->c[3],
              clm_rec->c[4],clm_rec->c[5],clm_rec->c[6],clm_rec->c[7],
              lvl->clm_utilize[i],(unsigned short)(clm_rec->use-(lvl->clm_utilize[i])));
/*          fprintf(fp,"  set_clm_ent_idx(lvl,%4d,%5d,%2d,%2d,%2d, 0x%02x, 0x%03x,%2d,\n",
           i,(unsigned short)(clm_rec->use-(lvl->clm_utilize[i])), clm_rec->permanent, clm_rec->lintel,
           clm_rec->height, clm_rec->solid, clm_rec->base, clm_rec->orientation);
        if ((clm_rec->c[5]==0)&&(clm_rec->c[6]==0)&&(clm_rec->c[7]==0))
            fprintf(fp,"    0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%x, 0x%x, 0x%x);\n",
              clm_rec->c[0],clm_rec->c[1],clm_rec->c[2],clm_rec->c[3],
              clm_rec->c[4],clm_rec->c[5],clm_rec->c[6],clm_rec->c[7]);
          else
            fprintf(fp,"    0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x, 0x%03x);\n",
              clm_rec->c[0],clm_rec->c[1],clm_rec->c[2],clm_rec->c[3],
              clm_rec->c[4],clm_rec->c[5],clm_rec->c[6],clm_rec->c[7]);
*/
        }
      }
    free_column_rec(clm_rec);
    fclose (fp);
    return true;
}

/**
 * Utility function for reverse engineering the TNG format.
 * Used in rework mode.
 * @param lvl Pointer to the LEVEL structure.
 * @param fname Destination file name.
 * @return Returns true on success, false otherwise.
 */
short write_def_tng_source(struct LEVEL *lvl,char *fname)
{
    FILE *fp;
    int i,k;
    fp = fopen (fname, "w");
    if (!fp)
    {
      message_error("Can't open \"%s\" for writing", fname);
      return false;
    }

    /*Preparing array bounds */
    const int arr_entries_x=lvl->tlsize.x*MAP_SUBNUM_X;
    const int arr_entries_y=lvl->tlsize.y*MAP_SUBNUM_Y;
    int cx,cy;
    for (cy=0; cy < arr_entries_y; cy++)
      for (cx=0; cx < arr_entries_x; cx++)
          for (k=0; k < get_thing_subnums(lvl,cx,cy); k++)
          {
            unsigned char *thing=get_thing(lvl,cx,cy,k);
            int spos_x=get_thing_subtile_x(thing);
            int spos_y=get_thing_subtile_y(thing);
            int sen_tl=get_thing_sensitile(thing);
            unsigned short tngtype=get_thing_type(thing);
/*            if (tngtype!=THING_TYPE_ITEM) continue; */
            if ( (sen_tl!=((spos_x/MAP_SUBNUM_Y-1)+(spos_y/MAP_SUBNUM_Y-1)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y-1)+(spos_y/MAP_SUBNUM_Y+0)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y-1)+(spos_y/MAP_SUBNUM_Y+1)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+0)+(spos_y/MAP_SUBNUM_Y-1)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+0)+(spos_y/MAP_SUBNUM_Y+0)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+0)+(spos_y/MAP_SUBNUM_Y+1)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+1)+(spos_y/MAP_SUBNUM_Y-1)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+1)+(spos_y/MAP_SUBNUM_Y+0)*lvl->tlsize.x)) &&
                 (sen_tl!=((spos_x/MAP_SUBNUM_Y+1)+(spos_y/MAP_SUBNUM_Y+1)*lvl->tlsize.x)) )
            {
              int tl_x=spos_x/MAP_SUBNUM_X;
              int tl_y=spos_y/MAP_SUBNUM_Y;
              fprintf(fp,"stl %3d,%3d tl %2d,%2d", spos_x, spos_y,
                  tl_x, tl_y);
              fprintf(fp," s %d,%d", spos_x-tl_x*MAP_SUBNUM_X, spos_y-tl_y*MAP_SUBNUM_Y);
              fprintf(fp," stlpos %3d,%3d",
              get_thing_subtpos_x(thing), get_thing_subtpos_y(thing));
              fprintf(fp," alt %3d altstl %d",
                get_thing_subtpos_h(thing),get_thing_subtile_h(thing));
              fprintf(fp," typ %5s",get_thing_type_shortname(tngtype));
              switch (tngtype)
              {
              case THING_TYPE_DOOR:
                fprintf(fp," knd %s",get_door_subtype_fullname(get_thing_subtype(thing)));
                break;
              case THING_TYPE_TRAP:
                fprintf(fp," knd %s",get_trap_subtype_fullname(get_thing_subtype(thing)));
                break;
              case THING_TYPE_ITEM:
                fprintf(fp," knd %s",get_item_subtype_fullname(get_thing_subtype(thing)));
                break;
              }
              fprintf(fp,"\n");
              for (i=0; i < SIZEOF_DK_TNG_REC; i++)
              {
                  fprintf(fp,"  %02X", (unsigned int)thing[i]);
              }
              fprintf(fp,"\n");
              for (i=0; i < SIZEOF_DK_TNG_REC; i++)
              {
                  fprintf(fp," %3d", (unsigned int)thing[i]);
              }
              fprintf(fp,"\n\n");
            }
          }
    fclose (fp);
    return true;
}
