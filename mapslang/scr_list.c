/******************************************************************************/
// scr_list.c - Another Dungeon Keeper Map Editor.
/******************************************************************************/
// Author:   Tomasz Lis
// Created:  05 Jan 2008

// Purpose:
//   Defines functions for initializing and displaying any
//   of the list screens.
//   This also includes keyboard actions for the screen.

// Comment:
//   None.

//Copying and copyrights:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
/******************************************************************************/

#include "scr_list.h"

#include "libadikted/adikted.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "scr_help.h"
#include "scr_slab.h"
#include "scr_thing.h"
#include "var_utils.h"

/*
 * Initializes variables for the list screen.
 */
short init_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    //Creating and clearing list variable
    workdata->list=(struct LIST_DATA *)malloc(sizeof(struct LIST_DATA));
    if (workdata->list==NULL)
     die("init_list: Cannot allocate memory.");
    return true;
}

/*
 * Deallocates memory for the list screen.
 */
void free_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
  free(workdata->list);
}

/*
 * Covers actions from all screens with numbered list.
 */
short actions_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int ncols=workdata->list->cols;
    int nitems=workdata->list->items;
    int num_rows=(nitems/ncols) + ((nitems%ncols)>0);
    int *pos=&(workdata->list->pos);
    switch (key)
    {
    case KEY_UP:
      (*pos)--;
      break;
    case KEY_DOWN:
      (*pos)++;
      break;
    case KEY_LEFT:
      (*pos)-=(nitems/ncols) + (((nitems)%(ncols))>0);
      break;
    case KEY_RIGHT:
      (*pos)+=(nitems/ncols) + (((nitems)%(ncols))>0);
      break;
    case KEY_PGUP:
      (*pos)-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      (*pos)+=scrmode->rows-1;
      break;
    case KEY_HOME:
      (*pos)=0;
      break;
    case KEY_END:
      (*pos)=nitems-1;
      break;
    default:
      return false;
    }
    if ((*pos) >= nitems)
      (*pos)=nitems-1;
    if ((*pos)<0) *pos=0;
    //Compute the row where selected item is
    int *y=&(workdata->list->y);
    int curr_row=(*pos)%num_rows;
    if ((*y)>curr_row) (*y)=curr_row;
    if ((*y)+scrmode->rows<curr_row+1) (*y)=curr_row-scrmode->rows+1;
    if ((*y)+scrmode->rows > num_rows+1)
      (*y)=num_rows-scrmode->rows+1;
    if (*y<0) *y=0;
    message_log(" actions_list: list position is %d",*pos);
    return true;
}

/*
 * Covers actions from all screens with right panel list.
 */
short actions_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    int ncols=workdata->list->cols;
    int nitems=workdata->list->items;
    int num_rows=(nitems/ncols) + ((nitems%ncols)>0);
    int *pos=&(workdata->list->pos);
    switch (key)
    {
    case KEY_UP:
      (*pos)--;
      break;
    case KEY_DOWN:
      (*pos)++;
      break;
    case KEY_LEFT:
      (*pos)-=(nitems/ncols) + (((nitems)%(ncols))>0);
      break;
    case KEY_RIGHT:
      (*pos)+=(nitems/ncols) + (((nitems)%(ncols))>0);
      break;
    case KEY_PGUP:
      (*pos)-=scrmode->rows-1;
      break;
    case KEY_PGDOWN:
      (*pos)+=scrmode->rows-1;
      break;
    case KEY_HOME:
      (*pos)=0;
      break;
    case KEY_END:
      (*pos)=nitems-1;
      break;
    default:
      return false;
    }
    int scr_rows=scrmode->rows-USRINPUT_ROWS-1;
    if ((*pos) >= nitems)
      (*pos)=nitems-1;
    if ((*pos)<0) (*pos)=0;
    //Compute the row where selected item is
    int *y=&(workdata->list->y);
    int curr_row=(*pos)%num_rows;
    if ((*y)>curr_row) (*y)=curr_row;
    if ((*y)+scr_rows<curr_row+1) (*y)=curr_row-scr_rows+1;
    if ((*y)+scr_rows > num_rows+1)
      (*y)=num_rows-scr_rows+1;
    if ((*y)<0) (*y)=0;
    message_log(" actions_rplist: list position is %d",*pos);
    return true;
}

/*
 * Action function - start any of the the list modes.
 */
short start_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int lstmode)
{
    workdata->list->prevmode=scrmode->mode;
    workdata->list->pos=0; // selected item position (rel. to screen top)
    //These parameters will be reset to proper values on redraw
    workdata->list->y=0; //the first visible row
    workdata->list->items=8; //number of items in the list
    workdata->list->cols=2; //number of columns in the item list
    workdata->list->val1=0;
    workdata->list->val2=0;
    workdata->list->val3=0;
    workdata->list->ptr=NULL;
    scrmode->mode=lstmode;
    inc_info_usr_mdswtch_count(workdata->lvl);
    message_info("Use arrow keys and page up/down to move, "
      "enter to choose.");
    return true;
}

/*
 * Action function - end any of the the list modes.
 */
void end_list(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (scrmode->mode!=workdata->list->prevmode)
      scrmode->mode=workdata->list->prevmode;
    else
      scrmode->mode=MD_SLB;
    message_release();
}

/*
 * Action function - start any of the right panel list modes.
 */
short start_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int lstmode)
{
    workdata->list->prevmode=scrmode->mode;
    workdata->list->pos=0; // selected item position (rel. to screen top)
    //These parameters will be reset to proper values on redraw
    workdata->list->y=0; //the first visible row
    workdata->list->items=8; //number of items in the list
    workdata->list->cols=1; //number of columns in the item list
    workdata->list->val1=0;
    workdata->list->val2=0;
    workdata->list->val3=0;
    scrmode->mode=lstmode;
    inc_info_usr_mdswtch_count(workdata->lvl);
    message_info("Use arrow keys and page up/down to move, "
      "or enter number with keyboard.");
    return true;
}

/*
 * Action function - end any of the the right panel list modes.
 */
void end_rplist(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    if (scrmode->mode!=workdata->list->prevmode)
      scrmode->mode=workdata->list->prevmode;
    else
      scrmode->mode=MD_SLB;
    message_release();
}

/*
 * Draws screen with a numbered list (as creature types or item types)
 * and key help for given screen.
 */
void draw_numbered_list(char *(*itemstr)(unsigned short),
        struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width)
{
    struct LIST_DATA *list=workdata->list;
    unsigned int line_length=min(scrmode->cols,LINEMSG_SIZE);
    unsigned int entry_width=itm_width+5;
    int scr_rows=scrmode->rows;
    list->cols=max((unsigned int)(line_length/entry_width),1);
    list->items=end_idx-start_idx+1;
    unsigned int num_rows=list->items/list->cols
                        + (((list->items)%(list->cols))>0);
    unsigned int i,k;
    int col;
    for (i=0; i < scr_rows; i++)
    {
      set_cursor_pos(i, 0);
      char it_msg[LINEMSG_SIZE];
      if (i+list->y<num_rows)
        for (k=0;k<list->cols;k++)
        {
          int itm_idx=start_idx+list->y+i+k*num_rows;
          if (itm_idx-start_idx==list->pos)
              col=PRINT_COLOR_BLACK_ON_LGREY;
          else
              col=PRINT_COLOR_LGREY_ON_BLACK;
          if (itm_idx<=end_idx)
            sprintf(it_msg,"%3d: %-*.*s",
                    itm_idx,itm_width,itm_width, itemstr(itm_idx));
          else
            sprintf(it_msg,"");
          screen_setcolor(col);
          screen_printf("%s",it_msg);
        }
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
      set_cursor_pos(i, scrmode->cols);
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
    }
    int scr_row=display_mode_keyhelp(workdata->help,0,scrmode->cols+3,scrmode->rows,
            scrmode->mode,list->pos);
}

/*
 * Draws screen with a list of things, including index of every thing,
 * and key help for given screen.
 */
void draw_thing_list(char *(*itemstr)(unsigned short),thing_subtype_arrayitem index_func,
        struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,
        unsigned int count,unsigned int itm_width)
{
    struct LIST_DATA *list=workdata->list;
    unsigned int line_length=min(scrmode->cols,LINEMSG_SIZE);
    unsigned int entry_width=itm_width+5;
    int scr_rows=scrmode->rows;
    list->cols=max((unsigned int)(line_length/entry_width),1);
    list->items=count;
    unsigned int num_rows=list->items/list->cols
                        + (((list->items)%(list->cols))>0);
    unsigned int i,k;
    int col;
    for (i=0; i < scr_rows; i++)
    {
      set_cursor_pos(i, 0);
      char it_msg[LINEMSG_SIZE];
      if (i+list->y<num_rows)
        for (k=0;k<list->cols;k++)
        {
          int itm_idx=list->y+i+k*num_rows;
          int real_index=index_func(itm_idx);
          if (itm_idx==list->pos)
              col=PRINT_COLOR_BLACK_ON_LGREY;
          else
              col=PRINT_COLOR_LGREY_ON_BLACK;
          if (itm_idx<count)
            sprintf(it_msg,"%3d: %-*.*s",
                    real_index,itm_width,itm_width, itemstr(real_index));
          else
            sprintf(it_msg,"");
          screen_setcolor(col);
          screen_printf("%s",it_msg);
        }
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
      set_cursor_pos(i, scrmode->cols);
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
    }
    int scr_row=display_mode_keyhelp(workdata->help,0,scrmode->cols+3,scrmode->rows,
            scrmode->mode,index_func(list->pos)-1);
}

/*
 * Draws right panel with a list.
 */
void draw_rpanel_list(char *(*itemstr)(unsigned short),
        struct SCRMODE_DATA *scrmode,struct MAPMODE_DATA *mapmode,struct LIST_DATA *list,
        unsigned int start_idx,unsigned int end_idx,unsigned int itm_width)
{
    unsigned int line_length=min(scrmode->keycols,LINEMSG_SIZE);
    unsigned int entry_width=itm_width+1;
    int scr_rows=scrmode->rows-USRINPUT_ROWS-1;
    list->cols=max((unsigned int)(line_length/entry_width),1);
    list->items=end_idx-start_idx+1;
    unsigned int num_rows=list->items/list->cols
                        + (((list->items)%(list->cols))>0);
    unsigned int i,k;
    int col;
    for (i=0; i < scr_rows; i++)
    {
      set_cursor_pos(i, scrmode->cols+2);
      char it_msg[LINEMSG_SIZE];
      if (i+list->y<num_rows)
        for (k=0;k<list->cols;k++)
        {
          int itm_idx=start_idx+list->y+i+k*num_rows;
          if (itm_idx-start_idx==list->pos)
              col=PRINT_COLOR_BLACK_ON_LGREY;
          else
              col=PRINT_COLOR_LGREY_ON_BLACK;
          if (itm_idx<=end_idx)
            sprintf(it_msg," %-*.*s",
                    itm_width,itm_width, itemstr(itm_idx));
          else
            sprintf(it_msg,"");
          screen_setcolor(col);
          screen_printf("%s",it_msg);
        }
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
      set_cursor_pos(i, scrmode->cols);
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
    }
}

/*
 * Covers actions from the creature screen.
 */
void actions_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_O:
          workdata->list->val2=get_owner_next(workdata->list->val2);
          break;
        case KEY_S:
        case KEY_SHIFT_S:
          if (workdata->list->val1<9)
            workdata->list->val1++;
          else
            message_error("Max level reached");
          break;
        case KEY_X:
        case KEY_SHIFT_X:
          if (workdata->list->val1>0)
            workdata->list->val1--;
          else
            message_error("Min level reached");
          break;
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_CRTR](scrmode,workdata);
          message_info("Adding creature cancelled");
          break;
        case KEY_ENTER:
        {
          struct IPOINT_2D subpos;
          get_map_subtile_pos(workdata->mapmode,&subpos);
          unsigned char *thing;
          thing=tng_makecreature(scrmode,workdata,subpos.x,subpos.y,workdata->list->pos+1);
          set_thing_subtile_h(thing,1);
          set_thing_level(thing,workdata->list->val1);
          set_thing_owner(thing,workdata->list->val2);
          mdend[MD_CRTR](scrmode,workdata);
        }; break;
        default:
          message_info("Unrecognized creature key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from the item type screen.
 */
void actions_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_CITM](scrmode,workdata);
          message_info("Adding Item cancelled");
          break;
        case KEY_ENTER:
        {
          struct IPOINT_2D subpos;
          get_map_subtile_pos(workdata->mapmode,&subpos);
          unsigned char *thing;
          thing=tng_makeitem(scrmode,workdata,subpos.x,subpos.y,workdata->list->pos+1);
          mdend[MD_CITM](scrmode,workdata);
        };break;
        default:
          message_info("Unrecognized add item key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from texture selection screen.
 */
void actions_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_TXTR](scrmode,workdata);
          message_info("Texture change cancelled");
          break;
        case KEY_ENTER:
          set_lvl_inf(workdata->lvl,workdata->list->pos);
          mdend[MD_TXTR](scrmode,workdata);
          message_info("Texture changed");
          break;
        default:
          message_info("Unrecognized texture key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from custom columns screen.
 */
void actions_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    struct DK_CUSTOM_CLM *cclm_recs[9];
    struct COLUMN_REC *clm_recs[9];
    int tx=(workdata->mapmode->map.x+workdata->mapmode->screen.x);
    int ty=(workdata->mapmode->map.y+workdata->mapmode->screen.y);
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_CCLM](scrmode,workdata);
          message_info("Customization cancelled");
          break;
        case KEY_ENTER:
          {
            int i,k;
            for (k=0;k<3;k++)
              for (i=0;i<3;i++)
              {
                cclm_recs[k*3+i]=create_cust_col();
                clm_recs[k*3+i]=cclm_recs[k*3+i]->rec;
              }
            //Retrieving parameters from LEVEL structure - the slab and its surrounding
            unsigned char *surr_slb=(unsigned char *)malloc(9*sizeof(unsigned char));
            unsigned char *surr_own=(unsigned char *)malloc(9*sizeof(unsigned char));
            unsigned char **surr_tng=(unsigned char **)malloc(9*sizeof(unsigned char *));
            get_slab_surround(surr_slb,surr_own,surr_tng,workdata->lvl,tx,ty);
            fill_custom_column_data(workdata->list->pos,clm_recs,surr_slb,surr_own,surr_tng);
            for (k=0;k<3;k++)
              for (i=0;i<3;i++)
              {
                cust_col_add_or_update(workdata->lvl,tx*3+i,ty*3+k,cclm_recs[k*3+i]);
              }
            mdend[MD_CCLM](scrmode,workdata);
            message_info("Custom columns set");
          };break;
        default:
          message_info("Unrecognized custom columns key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from slab list screen.
 */
void actions_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_SLBL](scrmode,workdata);
          message_info("Slab change cancelled");
          break;
        case KEY_ENTER:
          slb_place_room(workdata,workdata->list->pos);
          mdend[MD_SLBL](scrmode,workdata);
          message_info("New slab placed");
          break;
        default:
          message_info("Unrecognized slab list key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Covers actions from object search screen.
 */
void actions_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_SRCH](scrmode,workdata);
          message_info("Object search cancelled");
          break;
        case KEY_ENTER:
          mdend[MD_SRCH](scrmode,workdata);
          if (workdata->list->pos==0)
          {
            clear_highlight(workdata->mapmode);
          }
          int tx=-1;
          int ty=-1;
          int num_found=0;
          unsigned char *obj;
          while (ty<(long)workdata->lvl->tlsize.y)
          {
            obj=find_next_object_on_map(workdata->lvl,&tx,&ty,workdata->list->pos);
            if (obj!=NULL)
            {
              set_tile_highlight(workdata->mapmode,tx,ty,PRINT_COLOR_LRED_ON_YELLOW);
              num_found++;
            } else
            { break; }
          } 
          message_info("Matching objects found: %d",num_found);
          break;
        default:
          message_info("Unrecognized object search key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start texture selection mode.
 */
short start_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdtextr: starting");
    short result;
    result=start_list(scrmode,workdata,MD_TXTR);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->pos=get_lvl_inf(workdata->lvl);
    message_log(" start_mdtextr: completed");
    return result;
}

/*
 * Action function - start slab list mode.
 */
short start_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdslbl: starting");
    int tx=workdata->mapmode->map.x+workdata->mapmode->screen.x;
    int ty=workdata->mapmode->map.y+workdata->mapmode->screen.y;
    short result;
    result=start_list(scrmode,workdata,MD_SLBL);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->pos=get_tile_slab(workdata->lvl,tx,ty);
    message_log(" start_mdslbl: completed");
    return result;
}

void draw_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_creature_subtype_fullname,scrmode,workdata,
        1,CREATR_SUBTP_FLOAT,16);
    int scr_row=scrmode->rows-4;
    int scr_col1=scrmode->cols+3;
    // Display more info about the creature
    if (scr_row > 0)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf_toeol("Creature parameters");
      set_cursor_pos(scr_row++, scr_col1);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Level (s/x): ");
      screen_setcolor(PRINT_COLOR_LGREEN_ON_BLACK);
      screen_printf("%d", workdata->list->val1+1);
      set_cursor_pos(scr_row++, scr_col1);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner   (o): ");
      screen_setcolor(get_screen_color_owned(workdata->list->val2,false,false,false));
      screen_printf(get_owner_type_fullname(workdata->list->val2));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    }
    show_no_cursor();
}

void draw_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_item_subtype_fullname,scrmode,workdata,
        1,ITEM_SUBTYPE_SPELLARMG,18);
    show_no_cursor();
}

void draw_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_texture_fullname,scrmode,workdata,
        0,INF_MAX_INDEX,14);
    show_no_cursor();
}

void draw_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_custom_column_fullname,scrmode,workdata,
        0,CUST_CLM_GEN_MAX_INDEX,20);
    show_no_cursor();
}

void draw_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_slab_fullname,scrmode,workdata,
        0,SLAB_TYPE_PURPLE_PATH,17);
    show_no_cursor();
}

void draw_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_effectgen_subtype_fullname,scrmode,workdata,
        1,EFCTGEN_SUBTP_DRYICE,15);
    int scr_row=scrmode->rows-4;
    int scr_col1=scrmode->cols+3;
    // Display more info about the creature
    if (scr_row > 0)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf_toeol("Effect Generator params");
      set_cursor_pos(scr_row++, scr_col1);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner   (o): ");
      screen_setcolor(get_screen_color_owned(workdata->list->val2,false,false,false));
      screen_printf(get_owner_type_fullname(workdata->list->val2));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    }
    show_no_cursor();
}

void actions_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_CEFC](scrmode,workdata);
          message_info("Effect creation cancelled");
          break;
        case KEY_ENTER:
        {
          struct IPOINT_2D subpos;
          get_map_subtile_pos(workdata->mapmode,&subpos);
          unsigned char *thing;
          thing=tng_makeeffectgen(scrmode,workdata,subpos.x,subpos.y,workdata->list->pos+1);
          mdend[MD_CEFC](scrmode,workdata);
        };break;
        default:
          message_info("Unrecognized add effect key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_trap_subtype_fullname,scrmode,workdata,
        1,TRAP_SUBTYPE_DUMMY7,14);
    int scr_row=scrmode->rows-4;
    int scr_col1=scrmode->cols+3;
    // Display more info about the creature
    if (scr_row > 0)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(scr_row++, scr_col1);
      screen_printf_toeol("Trap parameters");
      set_cursor_pos(scr_row++, scr_col1);
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf("Owner   (o): ");
      screen_setcolor(get_screen_color_owned(workdata->list->val2,false,false,false));
      screen_printf(get_owner_type_fullname(workdata->list->val2));
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    }
    show_no_cursor();
}

void actions_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_CTRP](scrmode,workdata);
          message_info("Trap creation cancelled");
          break;
        case KEY_ENTER:
        {
          struct IPOINT_2D subpos;
          get_map_subtile_pos(workdata->mapmode,&subpos);
          unsigned char *thing;
          thing=tng_maketrap(scrmode,workdata,subpos.x,subpos.y,workdata->list->pos+1);
          mdend[MD_CTRP](scrmode,workdata);
        };break;
        default:
          message_info("Unrecognized add trap key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_thing_list(get_item_subtype_fullname,get_thing_subtypes_arritem_func(workdata->list->val1),
        scrmode,workdata,get_thing_subtypes_count(workdata->list->val1),18);
//    draw_numbered_list(get_item_subtype_fullname,scrmode,mapmode,1,ITEM_SUBTYPE_SPELLARMG,18);
    show_no_cursor();
}

void actions_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_EITM](scrmode,workdata);
          message_info("Item edit cancelled");
          break;
        case KEY_ENTER:
        {
          thing_subtype_arrayitem index_func;
          index_func=get_thing_subtypes_arritem_func(workdata->list->val1);
          int real_index=-1;
          if (index_func!=NULL)
              real_index=index_func(workdata->list->pos);
          if (real_index>=0)
            set_thing_subtype(workdata->list->ptr,real_index);
          mdend[MD_EITM](scrmode,workdata);
          if (real_index<0)
          {
              message_error("Internal error: Can't get subtype for selected item");
              break;
          }
          message_info("Item subtype and properties changed");
        }; break;
        default:
          message_info("Unrecognized edit item key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_crcrtr(scrmode,workdata);
}

void actions_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_O:
          workdata->list->val2=get_owner_next(workdata->list->val2);
          break;
        case KEY_S:
        case KEY_SHIFT_S:
          if (workdata->list->val1<9)
            workdata->list->val1++;
          else
            message_error("Max level reached");
          break;
        case KEY_X:
        case KEY_SHIFT_X:
          if (workdata->list->val1>0)
            workdata->list->val1--;
          else
            message_error("Min level reached");
          break;
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_ECRT](scrmode,workdata);
          message_info("Creature edit cancelled");
          break;
        case KEY_ENTER:
          set_thing_subtype(workdata->list->ptr,workdata->list->pos+1);
          set_thing_level(workdata->list->ptr,workdata->list->val1);
          set_thing_owner(workdata->list->ptr,workdata->list->val2);
          mdend[MD_ECRT](scrmode,workdata);
          message_info("Creature properties changed");
          break;
        default:
          message_info("Unrecognized edit creature key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_crefct(scrmode,workdata);
}

void actions_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_O:
          workdata->list->val2=get_owner_next(workdata->list->val2);
          break;
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_EFCT](scrmode,workdata);
          message_info("Effect Generator edit cancelled");
          break;
        case KEY_ENTER:
          set_thing_subtype(workdata->list->ptr,workdata->list->pos+1);
          set_thing_owner(workdata->list->ptr,workdata->list->val2);
          mdend[MD_EFCT](scrmode,workdata);
          message_info("Effect Generator properties changed");
          break;
        default:
          message_info("Unrecognized edit effect key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_crtrap(scrmode,workdata);
}

void actions_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    if (!actions_list(scrmode,workdata,key))
    {
      switch (key)
      {
        case KEY_O:
          workdata->list->val2=get_owner_next(workdata->list->val2);
          break;
        case KEY_TAB:
        case KEY_DEL:
        case KEY_ESCAPE:
          mdend[MD_ETRP](scrmode,workdata);
          message_info("Trap edit cancelled");
          break;
        case KEY_ENTER:
          set_thing_subtype(workdata->list->ptr,workdata->list->pos+1);
          set_thing_owner(workdata->list->ptr,workdata->list->val2);
          mdend[MD_ETRP](scrmode,workdata);
          message_info("Trap properties changed");
          break;
        default:
          message_info("Unrecognized edit trap key code: %d",key);
          speaker_beep();
      }
    }
}

void draw_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    draw_numbered_list(get_search_objtype_name,scrmode,workdata,
        0,get_search_objtype_count()-1,18);
    show_no_cursor();
}

short start_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_crcrtr: starting");
    short result;
    result=start_list(scrmode,workdata,MD_CRTR);
    scrmode->usrinput_type=SI_NONE;
    // creature level
    workdata->list->val1=0;
    // owning player
    workdata->list->val2=get_tile_owner(workdata->lvl,workdata->mapmode->map.x+workdata->mapmode->screen.x,workdata->mapmode->map.y+workdata->mapmode->screen.y);
//    list->pos=
    message_log(" start_crcrtr: completed");
    return result;
}

void end_crcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_critem: starting");
    short result;
    result=start_list(scrmode,workdata,MD_CITM);
    scrmode->usrinput_type=SI_NONE;
//    list->pos=
    message_log(" start_critem: completed");
    return result;
}

void end_critem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_crefct: starting");
    short result;
    result=start_list(scrmode,workdata,MD_CEFC);
    scrmode->usrinput_type=SI_NONE;
//    list->pos=
    message_log(" start_crefct: completed");
    return result;
}

void end_crefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_crtrap: starting");
    short result;
    result=start_list(scrmode,workdata,MD_CTRP);
    scrmode->usrinput_type=SI_NONE;
//    list->pos=
    message_log(" start_crtrap: completed");
    return result;
}

void end_crtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_editem: starting");
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    unsigned char *thing;
    thing=get_object(workdata->lvl,subpos.x,subpos.y,get_visited_obj_idx(workdata));
    // Note: we're not checking if the object is a thing - it have to be thing
    // but let's make sure it is an editable item
    int arr_idx=get_thing_subtypes_arridx(thing);
    if (arr_idx<0)
    {
      message_error("Can't find types list containing selected item.");
      return false;
    }
    unsigned short stype_idx=get_thing_subtype(thing);
    short result;
    result=start_list(scrmode,workdata,MD_EITM);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->val1=arr_idx;
    int npos=get_thing_subtypes_arritmidx(arr_idx,stype_idx);
    if (npos>=0)
      workdata->list->pos=npos;
    workdata->list->ptr=thing;
    message_log(" start_editem: completed");
    return result;
}

void end_editem(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_edefct: starting");
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    unsigned char *thing;
    thing=get_object(workdata->lvl,subpos.x,subpos.y,get_visited_obj_idx(workdata));
    // Note: we're not checking if the object is a thing - it have to be thing
    // but let's make sure it is an effect
    if (!is_effectgen(thing))
    {
      message_error("Can't edit effect - wrong object type");
      return false;
    }
    short result;
    result=start_list(scrmode,workdata,MD_EFCT);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->pos=get_thing_subtype(thing)-1;
    // owning player
    workdata->list->val2=get_thing_owner(thing);
    // The pointer which is changed at end
    workdata->list->ptr=thing;
    message_log(" start_edefct: completed");
    return result;
}

void end_edefct(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_edtrap: starting");
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    unsigned char *thing;
    thing=get_object(workdata->lvl,subpos.x,subpos.y,get_visited_obj_idx(workdata));
    // Note: we're not checking if the object is a thing - it have to be thing
    // but let's make sure it is a trap
    if (!is_trap(thing))
    {
      message_error("Can't edit trap - wrong object type");
      return false;
    }
    short result;
    result=start_list(scrmode,workdata,MD_ETRP);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->pos=get_thing_subtype(thing)-1;
    // owning player
    workdata->list->val2=get_thing_owner(thing);
    // The pointer which is changed at end
    workdata->list->ptr=thing;
    message_log(" start_edtrap: completed");
    return result;
}

void end_edtrap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdcclm: starting");
    short result;
    result=start_list(scrmode,workdata,MD_CCLM);
    scrmode->usrinput_type=SI_NONE;
//    list->pos=
    message_log(" start_mdcclm: completed");
    return result;
}

void end_mdtextr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

void end_mdcclm(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

void end_mdslbl(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_ecrtr: starting");
    struct IPOINT_2D subpos;
    get_map_subtile_pos(workdata->mapmode,&subpos);
    unsigned char *thing;
    thing=get_object(workdata->lvl,subpos.x,subpos.y,get_visited_obj_idx(workdata));
    // Note: we're not checking if the object is a thing - it have to be thing
    // but let's make sure it is a creature
    if (!is_creature(thing))
    {
      message_error("Can't edit creature - wrong object type");
      return false;
    }
    short result;
    result=start_list(scrmode,workdata,MD_ECRT);
    scrmode->usrinput_type=SI_NONE;
    workdata->list->pos=get_thing_subtype(thing)-1;
    // creature level
    workdata->list->val1=get_thing_level(thing);
    // owning player
    workdata->list->val2=get_thing_owner(thing);
    // The pointer which is changed at end
    workdata->list->ptr=thing;
    message_log(" start_ecrtr: completed");
    return result;
}

void end_edcrtr(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

short start_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdsrch: starting");
    short result;
    result=start_list(scrmode,workdata,MD_SRCH);
    scrmode->usrinput_type=SI_NONE;
//    list->pos=
    message_log(" start_mdsrch: completed");
    return result;
}

void end_mdsrch(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_list(scrmode,workdata);
}

char *get_listview_map_fname(unsigned short pos_idx)
{
  static char map_fname[LINEMSG_SIZE];
    if (pos_idx<1)
    {
      sprintf (map_fname, "(%s)","user typed");
    } else
    {
      sprintf (map_fname, "map%.5d", pos_idx);
    }
    return map_fname;
}

short start_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdlmap: starting");
    short result;
    result=start_rplist(scrmode,workdata,MD_LMAP);
    scrmode->usrinput_type=SI_LDMAP;
    scrmode->usrinput[0]='\0';
    scrmode->usrinput_pos=0;
    workdata->list->pos=0;
    level_free(workdata->mapmode->preview);
    level_clear(workdata->mapmode->preview);
    message_log(" start_mdlmap: completed");
    return result;
}

void end_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_rplist(scrmode,workdata);
    scrmode->usrinput_type=SI_NONE;
}

void draw_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct LEVEL *draw_lvl;
    if ((workdata->mapmode->level_preview&LPREV_LOAD) == LPREV_LOAD)
        draw_lvl=workdata->mapmode->preview;
    else
        draw_lvl=workdata->lvl;
    draw_map_area(scrmode,workdata->mapmode,draw_lvl,true,true,false);
    draw_rpanel_list(get_listview_map_fname,scrmode,workdata->mapmode,
        workdata->list,0,9999,12);
    display_rpanel_bottom(scrmode,workdata);
}

void actions_mdlmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    short load_preview;
    load_preview=false;
    short text_changed;
    if (string_get_actions(scrmode,key,&text_changed))
    {
      if ((text_changed)&&(scrmode->usrinput[0]!='\0'))
      {
        actions_rplist(scrmode,workdata,KEY_HOME);
      }
      if (text_changed)
        load_preview=true;
    } else
    if (actions_rplist(scrmode,workdata,key))
    {
      if (workdata->list->pos>0)
      {
        char *mapname=get_listview_map_fname(workdata->list->pos);
        strncpy(scrmode->usrinput,mapname,LINEMSG_SIZE);
        scrmode->usrinput_pos=strlen(scrmode->usrinput);
      }
      load_preview=true;
    } else
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          mdend[MD_LMAP](scrmode,workdata);
          message_info("Map loading cancelled");
          break;
        case KEY_ENTER:
          {
            popup_show("Loading map","Reading map files. Please wait...");
            short fname_ok=format_lvl_fname(workdata->lvl,scrmode->usrinput);
            mdend[MD_LMAP](scrmode,workdata);
            if (!fname_ok)
            {
              message_error("Wrong file name - can't load map");
              break;
            }
            free_map(workdata->lvl);
            format_lvl_savfname(workdata->lvl,"");
            user_load_map(workdata->lvl,true);
            clear_highlight(workdata->mapmode);
            change_mode(scrmode,workdata,scrmode->mode);
            message_info("Map \"%s\" loaded", get_lvl_fname(workdata->lvl));
          };break;
        default:
          message_info("Unrecognized \"%s\" key code: %d",longmodenames[MD_LMAP],key);
          speaker_beep();
      }
    }
    if (load_preview)
    {
      format_lvl_fname(workdata->mapmode->preview,scrmode->usrinput);
      if ((workdata->mapmode->level_preview&LPREV_LOAD) == LPREV_LOAD)
      {
        if (load_map_preview(workdata->mapmode->preview) == ERR_NONE)
          message_info("Map \"%s\" preview loaded",scrmode->usrinput);
      }
    }
}

short start_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    message_log(" start_mdsmap: starting");
    short result;
    result=start_rplist(scrmode,workdata,MD_SMAP);
    scrmode->usrinput_type=SI_SVMAP;
    scrmode->usrinput[0]='\0';
    scrmode->usrinput_pos=0;
    workdata->list->pos=0;
    level_free(workdata->mapmode->preview);
    level_clear(workdata->mapmode->preview);
    message_log(" start_mdsmap: completed");
    return result;
}

void end_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    end_rplist(scrmode,workdata);
    scrmode->usrinput_type=SI_NONE;
}

void draw_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata)
{
    struct LEVEL *draw_lvl;
    if ((workdata->mapmode->level_preview&LPREV_SAVE) == LPREV_SAVE)
        draw_lvl=workdata->mapmode->preview;
    else
        draw_lvl=workdata->lvl;
    draw_map_area(scrmode,workdata->mapmode,draw_lvl,true,true,false);
    draw_rpanel_list(get_listview_map_fname,scrmode,workdata->mapmode,
        workdata->list,0,9999,12);
    display_rpanel_bottom(scrmode,workdata);
}

void actions_mdsmap(struct SCRMODE_DATA *scrmode,struct WORKMODE_DATA *workdata,int key)
{
    message_release();
    short load_preview;
    load_preview=false;
    short text_changed;
    if (string_get_actions(scrmode,key,&text_changed))
    {
      if ((text_changed)&&(scrmode->usrinput[0]!='\0'))
      {
        actions_rplist(scrmode,workdata,KEY_HOME);
      }
      if (text_changed)
        load_preview=true;
    } else
    if (actions_rplist(scrmode,workdata,key))
    {
      if (workdata->list->pos>0)
      {
        char *mapname=get_listview_map_fname(workdata->list->pos);
        strncpy(scrmode->usrinput,mapname,LINEMSG_SIZE);
        scrmode->usrinput_pos=strlen(scrmode->usrinput);
      }
      load_preview=true;
    } else
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          mdend[MD_SMAP](scrmode,workdata);
          message_info("Map saving cancelled");
          break;
        case KEY_ENTER:
          {

            popup_show("Saving map","Writing map files. Please wait...");
            short fname_ok=format_lvl_savfname(workdata->lvl,scrmode->usrinput);
            mdend[MD_LMAP](scrmode,workdata);
            if (!fname_ok)
            {
              message_error("Map saving cancelled");
              break;
            }
            user_save_map(workdata->lvl,1);
            message_info("Map \"%s\" saved", get_lvl_savfname(workdata->lvl));
          };break;
        default:
          message_info("Unrecognized \"%s\" key code: %d",longmodenames[MD_SMAP],key);
          speaker_beep();
      }
    }
    if (load_preview)
    {
      format_lvl_fname(workdata->mapmode->preview,scrmode->usrinput);
      if ((workdata->mapmode->level_preview&LPREV_SAVE) == LPREV_SAVE)
      {
        if (load_map_preview(workdata->mapmode->preview) == ERR_NONE)
          message_info("Map \"%s\" preview loaded",scrmode->usrinput);
      }
    }
}
