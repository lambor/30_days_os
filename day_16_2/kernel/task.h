#ifndef task_h
#define task_h

struct TSS32
{
	int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
	int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
	int es,cs,ss,ds,fs,gs;
	int ldtr,iomap;
};

struct TASK
{
	int sel,flags;
	struct TSS32 tss;
};

#define MAX_TASKS 1000
#define TASK_GDT0 4

struct TASKCTL
{
	int running;
	int now;
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};

void load_tr(int tr);
void farjmp(int eip,int cs);

void mt_init(void);
void mt_taskswitch(void);

#include "memory.h"
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task);
void task_switch(void);
void task_sleep(struct TASK *task);
#endif
