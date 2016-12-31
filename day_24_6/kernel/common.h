#ifndef common_h
#define common_h

#include "map_sheet.h"
int keywin_off(struct SHEET *key_win,struct SHEET *sht_win,int cur_c,int cur_x);
int keywin_on(struct SHEET *key_win,struct SHEET *sht_win,int cur_c);
void change_wtitle8(struct SHEET *sht,char act);

#endif
