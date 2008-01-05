/*
 * scr_thing.h header file for scr_thing.c
 */

#ifndef ADIKT_SCRTHING_H
#define ADIKT_SCRTHING_H

struct LEVEL;

// Variables

typedef struct {
    int vistng[3][3]; // Which number are we looking at on each subtile
  } MDTNG_DATA;

extern MDTNG_DATA *mdtng;

//Functions - init and free
short init_mdtng(void);
void free_mdtng(void);

//Functions - start and stop
short start_mdtng(struct LEVEL *lvl);
void end_mdtng(void);

//Functions - actions and screen
void actions_mdtng(int key);
void draw_mdtng();
void draw_mdtng_panel();

//Functions - lower level
void change_visited_tile(void);
int display_thing(unsigned char *thing, int x, int y);
int display_action_point(unsigned char *actnpt, int x, int y);
int display_static_light(unsigned char *stlight, int x, int y);
int display_obj_stats(int scr_row, int scr_col);
int display_tng_subtiles(int scr_row, int scr_col,int ty,int tx);

//Functions - internal
char get_thing_char(int x, int y);
int get_tng_display_color(short obj_type,unsigned char obj_owner,short marked);
void tng_makeitem(int sx,int sy,unsigned char stype_idx);
void tng_change_height(struct LEVEL *lvl, unsigned int sx, unsigned int sy,unsigned int z,int delta_height);

#endif // ADIKT_SCRTHING_H