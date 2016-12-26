#ifndef dsctbl_h
#define dsctbl_h
#include "const.h"
void init_gdtidt(void);
extern void asm_inthandler20();
extern void asm_inthandler21();
extern void asm_inthandler2c();
extern void asm_sys_api();
extern void asm_inthandler0d();

struct SEGMENT_DESCRIPTOR
{
    short limit_low,base_low;
    char base_mid,access_right;
    char limit_high,base_high;
};

struct GATE_DESCRIPTOR
{
    short offset_low,selector;
    char dw_count,access_right;
    short offset_high;
};

#endif
