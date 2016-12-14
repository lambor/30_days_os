#ifndef dsctbl_h
#define dsctbl_h
#include "const.h"
void init_gdtidt(void);
extern void asm_inthandler21();
extern void asm_inthandler2c();
#endif
