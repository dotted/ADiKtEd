/*
 * scr_clm.h header file for scr_clm.c
 */

#ifndef ADIKT_SCRCLM_H
#define ADIKT_SCRCLM_H

struct LEVEL;

//Functions - init and free
short init_mdclm(void);
void free_mdclm(void);

//Functions - start and stop
short start_mdclm(struct LEVEL *lvl);
void end_mdclm(void);

//Functions - actions and screen
void actions_mdclm(int key);
void draw_mdclm();
void draw_mdclm_panel();

//Functions - lower level
int display_dat_subtiles(int scr_row, int scr_col,int ty,int tx);
int display_column(unsigned char *clmentry,int clm_idx, int scr_row, int scr_col);

//Functions - internal

#endif // ADIKT_SCRCLM_H