/*
 * scr_actn.c
 *
 * Defines functions for initializing and displaying all the level screens,
 * and also for actions - reading keyboard input and executing commands.
 *
 */

#include "scr_actn.h"

#include "globals.h"
#include "var_utils.h"
#include "graffiti.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_thing.h"
#include "scr_help.h"
#include "scr_slab.h"
#include "scr_clm.h"
#include "scr_list.h"
#include "scr_txted.h"
#include "obj_slabs.h"
#include "obj_things.h"
#include "obj_column.h"
#include "lev_data.h"

static void (*actions [MODES_COUNT])(int)={
     actions_mdslab, actions_mdtng, actions_crtre, actions_itemt,
     actions_help, actions_mdclm, actions_scrpt, actions_mdtextr,
     actions_mdcclm, actions_mdcube, actions_mdslbl};

// Drawing functions for modes. You can fill new entries
// with "draw_mdempty" till you create proper draw function.
static void (*mddraw [MODES_COUNT])()={
     draw_mdslab, draw_mdtng, draw_crtre, draw_itemt,
     draw_help, draw_mdclm, draw_scrpt, draw_mdtextr,
     draw_mdcclm, draw_mdcube, draw_mdslbl};

static void (*mdend [MODES_COUNT])()={
     end_mdslab, end_mdtng, end_list, end_list,
     end_help, end_mdclm, end_scrpt, end_list,
     end_list, end_list, end_list};

// Max. 5 chars mode names
const char *modenames[MODES_COUNT]={
     "Slab", "Thing", "Crtr", "Item",
     "Help", "Clmn", "Scrpt", "Textr",
     "CClm", "CCube", "SlbLs",};
// longer mode names
const char *longmodenames[MODES_COUNT]={
     "slab", "thing", "add creature","add item",
     "help", "column", "script", "texture",
     "cust.column","cust.cubes","slab list"};

struct SCRMODE_DATA *scrmode;
struct MAPMODE_DATA *mapmode;

//Automated commands - allow sending multiple commands to the program.
//Used by command line parameters
unsigned int *automated_commands;

// indicates if the main program loop should end
short finished;
// I/O enable variables
short screen_enabled;
short input_enabled;

/*
 * Initializes basic structures and dynamic variables, which may be needed
 * on parameters reading process. Only the basic structures are inited here.
 */
void init_levscr_basics(void)
{
    // variable that informs if main program loop should finish
    finished=false;
    // random num gen seed selection
    srand(time(0));
    //Basic configuration variables
    screen_enabled=true;
    input_enabled=true;
    //Creating and clearing screen mode info variable
    scrmode=(struct SCRMODE_DATA *)malloc(sizeof(struct SCRMODE_DATA));
    if (scrmode==NULL)
     die("init_levscr: Cannot allocate memory.");
    clear_scrmode(scrmode);
    mapmode=(struct MAPMODE_DATA *)malloc(sizeof(struct MAPMODE_DATA));
    if (mapmode==NULL)
     die("init_levscr: Cannot allocate memory.");
    clear_mapmode(mapmode);
    int i;
    automated_commands=malloc(READ_BUFSIZE*sizeof(unsigned int));
    for (i=0; i < READ_BUFSIZE; i++)
      automated_commands[i]=0;
}

/*
 * Initializes the screen to work with Adikted.
 */
void init_levscr_modes(void)
{
    // initilaize screen support library
    if (screen_enabled)
      screen_init();
    // and keyboard input
    if (input_enabled)
      input_init();
    // init modes - read help file
    init_help();
    // init modes - create list structures
    init_list();
    // init modes - create text editor structures
    init_scrpt();
    // init modes - create column structures
    init_mdclm();
    // init modes - create thing structures
    init_mdtng();
    // initialize slab mode
    //(this includes slbkey required for all screens containing map)
    init_mdslab();
}

void clear_scrmode(struct SCRMODE_DATA *scrmode)
{
    scrmode->keycols=40;
    scrmode->mode=MD_SLB;
    //This will disable drawing until variables are updated
    scrmode->rows=0;
    scrmode->cols=0;
    scrmode->clipbrd=NULL;
    scrmode->clip_count=0;
}

void clear_mapmode(struct MAPMODE_DATA *mapmode)
{
    mapmode->mark=false;
    mapmode->paintmode=false;
    mapmode->paintown=false;
    mapmode->paintroom=0;
    mapmode->screenx=0;
    mapmode->screeny=0;
    mapmode->mapx=0;
    mapmode->mapy=0;
    mapmode->subtl_x=1;
    mapmode->subtl_y=1;
    mapmode->panel_mode=PV_MODE;
}

void free_levscr(void)
{
    // free modes
    free_help();
    free_list();
    free_scrpt();
    free_mdclm();
    free_mdtng();
    free_mdslab();
    // Main screen variables
    free(automated_commands);
    free(mapmode);
    free(scrmode);
    // Shutting down screen support
    screen_done();
}

/*
 * Draw the whole screen; Draws bottom lines and calls proper function
 * to draw rest of the screen.
 */
void draw_levscr(struct LEVEL *lvl)
{
    if (mapmode->mark)
      mark_check();
    set_cursor_visibility(0);
    int all_rows=get_screen_rows();
    int all_cols=get_screen_cols();
    scrmode->rows = all_rows-2;
    scrmode->cols = all_cols-scrmode->keycols;
    //If we shouldn't draw on screen - just exit.
    if (!screen_enabled) return;
    // If we don't have much room at all, just forget it!
    if (scrmode->cols < 0) return;

    set_cursor_pos(scrmode->rows, 0);
    screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
    // Hope there's enough room for it :)
    if (all_cols>70)
      screen_printf_toeol("Dungeon Keeper Map Editor    %5.5s mode  %s", 
                 modenames[scrmode->mode], mode_status (scrmode->mode));

    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    set_cursor_pos(all_rows-1, 0);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    screen_printf_toeol(message_get());
    mddraw[scrmode->mode%MODES_COUNT]();
    set_cursor_pos(all_rows-1, all_cols-1);
    screen_refresh();
}

/*
 * Draws screen for new modes which do not have real drawing function yet.
 */
void draw_mdempty()
{
    int tx,ty;
    tx=mapmode->mapx+mapmode->screenx;
    ty=mapmode->mapy+mapmode->screeny;
    draw_map_area(lvl,true,false,false);
    int scr_row=0;
    int scr_col=scrmode->cols+3;
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("This mode is unfinished.");
    set_cursor_pos(scr_row++, scr_col);
    screen_printf("No drawing function yet.");
    scr_row++;
    scr_row=display_mode_keyhelp(scr_row,scr_col,scrmode->mode);
}

/*
 * Draws the right panel with contents set by panel_mode.
 */
void draw_forced_panel(struct LEVEL *lvl, short panel_mode)
{
  int i;
  int scr_row=0;
  int scr_col=scrmode->cols+2;
  switch (panel_mode)
  {
  case PV_COMPS:
    for (i=0; i<help->compassrows; i++)
          draw_help_line(scr_row++,scr_col,help->compass[i]);
    display_tngdat();
    break;
  default:
    break;
  }
}

/*
 * Sets variables for drawing marked area in marking mode.
 */
void mark_check(void)
{
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    if (!mapmode->mark)
      return;
    if (mapmode->markx <= tx)
    {
      mapmode->markl = mapmode->markx;
      mapmode->markr = tx;
    }
    else
    {
      mapmode->markr = mapmode->markx;
      mapmode->markl = tx;
    }
    if (mapmode->marky <= ty)
    {
      mapmode->markt = mapmode->marky;
      mapmode->markb = ty;
    }
    else
    {
      mapmode->markb = mapmode->marky;
      mapmode->markt = ty;
    }
}

char *mode_status(int mode)
{
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    static char buffer[LINEMSG_SIZE];
    switch (mode)
    {
    case MD_CLM:
    case MD_TNG:
      sprintf (buffer, "   Position: %2d,%2d", tx,ty);
      break;
    case MD_SLB:
      sprintf (buffer, "   Position: %2d,%2d", tx,ty);
      if (mapmode->mark)
          strcat (buffer, " (Marking)");
      else if (mapmode->paintmode)
          strcat (buffer, " (Painting)");
      break;
    case MD_HELP:
      sprintf (buffer, "   (Help for %s mode)", longmodenames[help->formode]);
      break;
    case MD_CRTR:
      sprintf (buffer, "   (Selecting kind)");
      break;
    case MD_ITMT:
    case MD_TXTR:
    case MD_SLBL:
      sprintf (buffer, "   (Selecting type)");
      break;
    default:
      strcpy (buffer, "(unknown mode)");
      break;
    case MD_SCRP:
      sprintf (buffer, "   (Text editor)");
      break;
    }
    return buffer;
}

/*
 * Returns color in which the specified tile should be drawn.
 * The color depends on tile owner, but also marking mode.
 */
int get_draw_map_tile_color(struct LEVEL *lvl,int tx,int ty,short special,short darken)
{
    int g;
    if ((tx<0)||(tx>=MAP_SIZE_X)) return PRINT_COLOR_GREY_ON_BLACK;
    if ((ty<0)||(ty>=MAP_SIZE_Y)) return PRINT_COLOR_GREY_ON_BLACK;
    int own=get_tile_owner(lvl,tx,ty);
    short marked=((mapmode->mark) && (tx>=mapmode->markl) && (tx<=mapmode->markr)
                        && (ty>=mapmode->markt) && (ty<=mapmode->markb));
    int col;
    if (special)
    {
      if (marked)
        return PRINT_COLOR_MAGENT_ON_LGREY;
      else
      {
        switch (own)
        {
        case PLAYER0:return PRINT_COLOR_LMAGENT_ON_RED;
        case PLAYER1:return PRINT_COLOR_LMAGENT_ON_BLUE;
        case PLAYER2:return PRINT_COLOR_LMAGENT_ON_GREEN;
        case PLAYER3:return PRINT_COLOR_LMAGENT_ON_BROWN;
        case PLAYER_GOOD:return PRINT_COLOR_LMAGENT_ON_CYAN;
        default:
        case PLAYER_UNSET:
              return PRINT_COLOR_LMAGENT_ON_BLACK;
        }
      }
    } else
    {
      return get_screen_color_owned(own,marked,darken);
    }
}

int get_screen_color_owned(unsigned char owner,short marked,short darken)
{

    if (marked)
    {
        switch (owner)
        {
        case PLAYER0:return PRINT_COLOR_RED_ON_WHITE;
        case PLAYER1:return PRINT_COLOR_BLUE_ON_WHITE;
        case PLAYER2:return PRINT_COLOR_GREEN_ON_WHITE;
        case PLAYER3:return PRINT_COLOR_BROWN_ON_WHITE;
        case PLAYER_GOOD:return PRINT_COLOR_CYAN_ON_WHITE;
        default:
        case PLAYER_UNSET:return PRINT_COLOR_BLACK_ON_LGREY;
        }
    } else
    {
        switch (owner)
        {
        case PLAYER0:
            if (darken)
              return PRINT_COLOR_GREY_ON_RED;
            else
              return PRINT_COLOR_WHITE_ON_RED;
        case PLAYER1:
            if (darken)
              return PRINT_COLOR_GREY_ON_BLUE;
            else
              return PRINT_COLOR_WHITE_ON_BLUE;
        case PLAYER2:
            if (darken)
              return PRINT_COLOR_GREY_ON_GREEN;
            else
              return PRINT_COLOR_WHITE_ON_GREEN;
        case PLAYER3:
            if (darken)
              return PRINT_COLOR_GREY_ON_BROWN;
            else
              return PRINT_COLOR_WHITE_ON_BROWN;
        case PLAYER_GOOD:
            if (darken)
              return PRINT_COLOR_GREY_ON_CYAN;
            else
              return PRINT_COLOR_WHITE_ON_CYAN;
        default:
        case PLAYER_UNSET:
            if (darken)
              return PRINT_COLOR_GREY_ON_BLACK;
            else
              return PRINT_COLOR_LGREY_ON_BLACK;
        }
    }

}

/*
 * Reduces slab to basic type, for displaying map without rooms
 */
unsigned char simplify_map_slab(unsigned char slab)
{
  if (slab==SLAB_TYPE_ROCK) return SLAB_TYPE_ROCK;
  if (slab_is_tall(slab)) return SLAB_TYPE_EARTH;
  return SLAB_TYPE_CLAIMED;
}

/*
 * Returns a character that should de drawn on map in current mode.
 */
int get_draw_map_tile_char(struct LEVEL *lvl,int tx,int ty,short show_ground,short show_rooms,short show_things,short force_at)
{
    char out_ch;
    unsigned char slab;
    if (show_things)
      out_ch=get_thing_char(tx,ty);
    else
      out_ch=' ';
    if ((out_ch==' ')&&((show_ground)||(show_rooms)))
    {
      slab=get_tile_slab(lvl,tx,ty);
      if (!show_rooms) slab=simplify_map_slab(slab);
      if ((force_at)||(slbkey==NULL))
        out_ch='@';
      else
        out_ch=slbkey[slab];
    }
    return out_ch;
}

/*
 * Draws the map area for all modes that has map display.
 * Also clears the right panel.
 */
void draw_map_area(struct LEVEL *lvl,short show_ground,short show_rooms,short show_things)
{
    int i, k;
    for (k=0; k<scrmode->rows; k++)
    {
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      set_cursor_pos(k,0);
      int ty=mapmode->mapy+k;
      if (ty >= MAP_SIZE_Y)
      {
          for (i=0; i<scrmode->cols; i++)
            screen_printchr(' ');
      }
      else
      {
          for (i=0; i<scrmode->cols; i++)
          {
            int tx=mapmode->mapx+i;
            if (tx < MAP_SIZE_X)
            {
              char out_ch;
              int g;
              short has_ccol;
              short darken;
              if (show_rooms)
              {
                  g = graffiti_idx(lvl,tx,ty);
                  has_ccol = slab_has_custom_columns(lvl,tx,ty);
                  unsigned char slab=get_tile_slab(lvl,tx,ty);
                  darken=(slab==SLAB_TYPE_ROCK)||(slab==SLAB_TYPE_LAVA);
              } else
              {
                  g = -1;
                  has_ccol = false;
                  darken=(get_object_tilnums(lvl,tx,ty)==0);
              }
              screen_setcolor(get_draw_map_tile_color(lvl,tx,ty,has_ccol,darken));
              out_ch=get_draw_map_tile_char(lvl,tx,ty,show_ground,show_rooms,show_things,(g>=0));
              screen_printchr(out_ch);
            } else
            {
              screen_printchr(' ');
            }
          }
      }
      screen_setcolor(PRINT_COLOR_YELLOW_ON_BLUE);
      screen_printchr(' ');
      screen_printchr(' ');
      screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
      screen_printf_toeol("");
    }
}

/*
 * Shows cursor on the map screen.
 */
void draw_map_cursor(struct LEVEL *lvl,short show_ground,short show_rooms,short show_things)
{
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    char out_ch;
    int g;
    if (show_rooms)
      g = graffiti_idx(lvl,tx,ty);
    else
      g = -1;
    out_ch=get_draw_map_tile_char(lvl,tx,ty,show_ground,show_rooms,show_things,(g>=0));
    show_cursor(out_ch);
}

void show_cursor(char cur)
{
    set_cursor_pos(mapmode->screeny, mapmode->screenx);
    screen_setcolor(PRINT_COLOR_RED_ON_WHITE);
    screen_printchr(cur);
    set_cursor_pos(get_screen_rows()-1, get_screen_cols()-1);
}

void display_tngdat(void)
{
    int tx, ty;
    tx = mapmode->screenx+mapmode->mapx;
    ty = mapmode->screeny+mapmode->mapy;
    if (scrmode->rows > TNGDAT_ROWS)
    {
      int scr_col=scrmode->cols+3;
      if (dat_view_mode!=0)
      {
        display_dat_subtiles(scrmode->rows-TNGDAT_ROWS,scr_col,ty,tx);
        scr_col+=17;
      }
      display_tng_subtiles(scrmode->rows-TNGDAT_ROWS,scr_col,ty,tx);

    }
}
    
int display_mode_keyhelp(int scr_row, int scr_col,int mode)
{
    if (!init_key_help(mode)) return 0;
    int i;
    for (i=0; i<help->rows; i++)
          draw_help_line(scr_row++,scr_col,help->text[i]);
    return scr_row;
}

/*
 * Changes the work mode. Reinits mode even if the new one is same as old.
 */
int change_mode(int new_mode)
{
  mdend[scrmode->mode%MODES_COUNT]();
  switch (new_mode)
  {
  case MD_SLB:
       start_mdslab(lvl);
       break;
  case MD_TNG:
       start_mdtng(lvl);
       break;
  case MD_CRTR:
  case MD_ITMT:
       start_list(lvl,new_mode);
       break;
  case MD_TXTR:
       start_mdtextr(lvl);
       break;
  case MD_SLBL:
       start_mdslbl(lvl);
       break;
  case MD_HELP:
       start_help(lvl);
       break;
  case MD_CLM:
       start_mdclm(lvl);
       break;
  case MD_SCRP:
       start_scrpt(lvl);
       break;
  }
}

/*
 * Returns if the work mode is "simple". You can't save, load or exit
 * directly from "simple" modes.
 */
short is_simple_mode(int mode)
{
    if ((scrmode->mode==MD_HELP) || (scrmode->mode==MD_CRTR) ||
        (scrmode->mode==MD_ITMT) || (scrmode->mode==MD_TXTR) ||
        (scrmode->mode==MD_CCLM) || (scrmode->mode==MD_CUBE) ||
        (scrmode->mode==MD_SLBL) )
      return true;
    return false;
}

/*
 * Main action function - gets and processes a key stroke.
 */
void proc_key(void)
{
    static char usrinput[READ_BUFSIZE];
    unsigned int g;
    if (automated_commands[0]!=0)
    {
      g=automated_commands[0];
      memmove(automated_commands,automated_commands+1,(READ_BUFSIZE-1)*sizeof(unsigned int));
    } else
    {
      if (!input_enabled)
      {
        finished=true;
        return;
      }
      g = get_key();
    }
    // Decoding "universal keys" - global actions
    // which should work in every screen
    //Performing actions, or sending the keycode elswhere
    switch (g)
    {
    case KEY_F1:
      if (scrmode->mode != MD_HELP)
        start_help(lvl);
      break;
    case KEY_CTRL_Q:
      if (!is_simple_mode(scrmode->mode))
      {
          finished=true;
          break;
      }
      message_info("You can't quit from here.");
//      actions[scrmode->mode%MODES_COUNT](KEY_CTRL_Q); // Grotty but it'll work
      break;
    case KEY_CTRL_L:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't load from here.");
        break;
      }
      if (get_str ("Enter map number/name to load: ", usrinput))
      {
        popup_show("Loading map","Reading map files. Please wait...");
        if (format_map_fname(lvl->fname,usrinput))
        {
          free_map();
          load_map(lvl);
          change_mode(scrmode->mode);
          message_info("Map \"%s\" loaded", lvl->fname);
        } else
          message_error("Map loading cancelled");
      } else
      {
        speaker_beep();
      }
      break;
    case KEY_F7:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't load from here.");
        break;
      }
      if (strlen(lvl->fname)>0)
      {
          popup_show("Reloading map","Reading map files. Please wait...");
          free_map();
          load_map(lvl);
          change_mode(scrmode->mode);
          message_info("Map \"%s\" reloaded", lvl->fname);
      } else
          message_error("Map name is empty, cannot load last loaded.");
      break;
    case KEY_CTRL_S:
      if ((scrmode->mode==MD_HELP) || (scrmode->mode==MD_CRTR) || (scrmode->mode==MD_ITMT))
      {
        message_info("You can't save from here.");
        break;
      }
      if (get_str("Enter map number/name to save: ", usrinput))
      {
        popup_show("Saving map","Writing map files. Please wait...");
        if (format_map_fname(lvl->savfname,usrinput))
        {
          save_map(lvl);
          message_info("Map \"%s\" saved", lvl->savfname);
        } else
          message_error("Map saving cancelled");
      } else
      {
        speaker_beep();
      }
      break;
    case KEY_F5:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't save from here.");
        break;
      }
      if (strlen(lvl->savfname)>0)
      {
        popup_show("Saving map","Writing map files. Please wait...");
        save_map(lvl);
        message_info("Map \"%s\" saved", lvl->savfname);
      } else
      {
          message_error("Map name is empty, please save as.");
      }
      break;
    case KEY_CTRL_N:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't clear map from here.");
        break;
      }
      popup_show("Clearing map","Generating empty map. Please wait...");
      free_map();
      start_new_map(lvl);
      change_mode(scrmode->mode);
      message_info_force("New map started");
      break;
    case KEY_CTRL_R:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't randomize map from here.");
        break;
      }
      popup_show("Randomizing map","Generating random map. Please wait...");
      free_map();
      generate_random_map(lvl);
      change_mode(scrmode->mode);
      message_info_force("Map generation completed");
      break;
    case KEY_CTRL_P:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("Go back to map view first.");
        break;
      }
      if (mapmode->panel_mode!=PV_COMPS)
        mapmode->panel_mode=PV_COMPS;
      else
        mapmode->panel_mode=PV_MODE;
      message_info_force("Compass rose mode %s",(mapmode->panel_mode==PV_COMPS)?"enabled":"disabled");
      break;
    case KEY_CTRL_U:
      datclm_auto_update=!datclm_auto_update;
      if (datclm_auto_update)
        message_info_force("Automatic update of DAT/CLM/WIB enabled");
      else
        message_info_force("Auto DAT/CLM/WIB update disabled - manual with \"u\"");
      break;
    case KEY_CTRL_T:
      if ((scrmode->mode==MD_HELP) || (scrmode->mode==MD_CRTR) || (scrmode->mode==MD_ITMT))
      {
        message_info("You can't view script from here.");
      } else
      {
        change_mode(MD_SCRP);
      }
      break;
    case KEY_CTRL_E:
      if (is_simple_mode(scrmode->mode))
      {
        message_info("You can't change texture from here.");
      } else
      {
        start_mdtextr(lvl);
      }
      break;

    default:
      {
        //Sending the action to a function corresponding to actual screen
        actions[scrmode->mode%MODES_COUNT](g);
      };break;
    }
}

/*
 * Action function - covers cursor actions from non-help screens.
 */
short cursor_actions(int key)
{
    int osx, osy, omx, omy;
    int i, j;
    
    osx=mapmode->screenx;
    osy=mapmode->screeny;
    omx=mapmode->mapx;
    omy=mapmode->mapy;
    
    switch (key)
    {
    case KEY_CTRL_UP: // ctrl+arrow up
      mapmode->screeny-=5;
      break;
    case KEY_CTRL_DOWN: // ctrl+arrow down
      mapmode->screeny+=5;
      break;
    case KEY_CTRL_LEFT: // ctrl+arrow left
      mapmode->screenx-=5;
      break;
    case KEY_CTRL_RIGHT: // ctrl+arrow right
      mapmode->screenx+=5;
      break;
    case KEY_ALT_UP: // alt+arrow up
      mapmode->screeny-=15;
      break;
    case KEY_ALT_DOWN: // alt+arrow down
      mapmode->screeny+=15;
      break;
    case KEY_ALT_LEFT: // alt+arrow left
      mapmode->screenx-=15;
      break;
    case KEY_ALT_RIGHT: // alt+arrow right
      mapmode->screenx+=15;
      break;
    case KEY_CTRL_HOME: // ctrl+home
      mapmode->mapx=0;
      mapmode->screenx=0;
      break;
    case KEY_CTRL_END: // ctrl+end
      mapmode->mapx=MAP_SIZE_X;
      mapmode->screenx=MAP_SIZE_X;
      break;
    case KEY_CTRL_PGUP: // ctrl+page up
      mapmode->mapy=0;
      mapmode->screeny=0;
      break;
    case KEY_CTRL_PGDOWN: // ctrl+page down
      mapmode->mapy=MAP_SIZE_Y;
      mapmode->screeny=MAP_SIZE_Y;
      break;
    case KEY_UP:
      mapmode->screeny--;
      break;
    case KEY_DOWN:
      mapmode->screeny++;
      break;
    case KEY_RIGHT:
      mapmode->screenx++;
      break;
    case KEY_LEFT:
      mapmode->screenx--;
      break;
    case KEY_END:
      mapmode->screenx+=10;
      break;
    case KEY_HOME:
      mapmode->screenx-=10;
      break;
    case KEY_PGUP:
      mapmode->screeny-=10;
      break;
    case KEY_PGDOWN:
      mapmode->screeny+=10;
      break;
      default:
      return false;
    }
    curposcheck();
    int tx=mapmode->screenx+mapmode->mapx;
    int ty=mapmode->screeny+mapmode->mapy;
    if ((omx+osx != tx) || (omy+osy != ty))
      // As we've moved, reset which object we're looking at
    change_visited_tile();
    return true;
}

/*
 * Action function - allows selecting subtiles with digits from numpad.
 */
short subtl_select_actions(int key)
{
      switch (key)
      {
        case '1':
          mapmode->subtl_x=0;
          mapmode->subtl_y=2;
          break;
        case '2':
          mapmode->subtl_x=1;
          mapmode->subtl_y=2;
          break;
        case '3':
          mapmode->subtl_x=2;
          mapmode->subtl_y=2;
          break;
        case '4':
          mapmode->subtl_x=0;
          mapmode->subtl_y=1;
          break;
        case '5':
          mapmode->subtl_x=1;
          mapmode->subtl_y=1;
          break;
        case '6':
          mapmode->subtl_x=2;
          mapmode->subtl_y=1;
          break;
        case '7':
          mapmode->subtl_x=0;
          mapmode->subtl_y=0;
          break;
        case '8':
          mapmode->subtl_x=1;
          mapmode->subtl_y=0;
          break;
        case '9':
          mapmode->subtl_x=2;
          mapmode->subtl_y=0;
          break;
      default:
      return false;
    }
    return true;
}

/*
 * Action subfunction - check cursor position.
 */
void curposcheck(void)
{
    if (mapmode->screenx < 0)
    {
      mapmode->mapx+=mapmode->screenx;
      mapmode->screenx=0;
    }
    if (mapmode->screeny < 0)
    {
      mapmode->mapy+=mapmode->screeny;
      mapmode->screeny=0;
    }
    if (mapmode->screeny >= scrmode->rows)
    {
      mapmode->mapy+=mapmode->screeny-(scrmode->rows-1);
      mapmode->screeny=scrmode->rows-1;
    }
    if (mapmode->screenx >= scrmode->cols)
    {
      mapmode->mapx+=mapmode->screenx-(scrmode->cols-1);
      mapmode->screenx = scrmode->cols-1;
    }
    if (mapmode->mapx+scrmode->cols > MAP_SIZE_X)
      mapmode->mapx = MAP_SIZE_X-scrmode->cols;
    if (mapmode->mapy+scrmode->rows > MAP_SIZE_Y)
      mapmode->mapy = MAP_SIZE_Y-scrmode->rows;
    if (mapmode->mapx < 0)
      mapmode->mapx=0;
    if (mapmode->mapy < 0)
      mapmode->mapy=0;
}

/*
 * Gets index of the item from clipboard which is object.
 * Objects are action points, lights and things.
 * If cannot find, returns NULL.
 */
struct CLIPBOARD *get_clipboard_object(int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        int dtype=scrmode->clipbrd[i].dtype;
        if ((dtype==OBJECT_TYPE_ACTNPT)||(dtype==OBJECT_TYPE_THING)||
            (dtype==OBJECT_TYPE_STLIGHT))
          cidx++;
        if (cidx==idx)
          return &(scrmode->clipbrd[i]);
    }
    return NULL;
}

/*
 * Gets the slab item from clipboard.
 */
unsigned char *get_clipboard_slab(int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_SLAB)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Gets the DAT list item from clipboard.
 */
unsigned char *get_clipboard_datlst(int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_DATLST)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Gets the column item from clipboard.
 */
unsigned char *get_clipboard_column(int idx)
{
    if (idx<0) return NULL;
    int cidx=-1;
    int i;
    for (i=0;i<scrmode->clip_count;i++)
    {
        if (scrmode->clipbrd[i].dtype==OBJECT_TYPE_COLUMN)
          cidx++;
        if (cidx==idx)
          return scrmode->clipbrd[i].data;
    }
    return NULL;
}

/*
 * Adds any object/slab/datlist/column to the clipboard,
 * pasting the given pointer as clipboard item.
 * Returns index of new item.
 */
int add_clipboard_any(char *obj,int obj_type)
{
    if (obj==NULL) return 0;
    int obj_idx=scrmode->clip_count;
    scrmode->clipbrd=realloc(scrmode->clipbrd,(obj_idx+1)*sizeof(struct CLIPBOARD));
    if (scrmode->clipbrd==NULL)
        die("add_clipboard_any: Cannot allocate clipboard memory");
    scrmode->clip_count=obj_idx+1;
    scrmode->clipbrd[obj_idx].data=obj;
    scrmode->clipbrd[obj_idx].dtype=obj_type;
    return obj_idx;
}

/*
 * Replaces clipboard with given object/slab/datlist/column,
 * pasting the given pointer as clipboard item.
 * Returns index of new item, which is zero.
 */
int put_clipboard_any(char *obj,int obj_type)
{
    clear_clipboard();
    return add_clipboard_any(obj,obj_type);
}

/*
 * Clears all object from clipboard (frees the memory).
 */
void clear_clipboard()
{
     int i;
     for (i=scrmode->clip_count-1;i>=0;i--)
         free(scrmode->clipbrd[i].data);
     scrmode->clip_count=0;
     free(scrmode->clipbrd);
     scrmode->clipbrd=NULL;
}

/*
 * Copies thing to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_thing(unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_TNG_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_TNG_REC);
    return put_clipboard_any(obj_cp,OBJECT_TYPE_THING);
}

/*
 * Copies action point to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_actnpt(unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_APT_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_APT_REC);
    return put_clipboard_any(obj_cp,OBJECT_TYPE_ACTNPT);
}

/*
 * Copies static light to clipboard, clearing any previous thing there.
 */
int copy_to_clipboard_stlight(unsigned char *obj)
{
    if (obj==NULL) return 0;
    unsigned char *obj_cp=(unsigned char *)malloc(SIZEOF_DK_LGT_REC);
    memcpy(obj_cp,obj,SIZEOF_DK_LGT_REC);
    return put_clipboard_any(obj_cp,OBJECT_TYPE_STLIGHT);
}
