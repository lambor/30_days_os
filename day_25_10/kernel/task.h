#ifndef task_h
#define task_h

struct TSS32
{
	int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
	int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
	int es,cs,ss,ds,fs,gs;
	int ldtr,iomap;
};

#include "fifo.h"

struct TASK
{
	int sel,flags;
	int level,priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
	struct CONSOLE *cons;
	int ds_base;
};

#define MAX_TASKS_LV 100	//100 tasks per group
#define MAX_TASKLEVELS 10	//10 task group level
#define MAX_TASKS 1000 //100 * 10
#define TASK_GDT0 4

struct TASKLEVEL
{
	int running;
	int now;
	struct TASK *tasks[MAX_TASKS_LV];
};

struct TASKCTL
{
	int now_lv;
	char lv_change;
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

void load_tr(int tr);
void farjmp(int eip,int cs);

void mt_init(void);
void mt_taskswitch(void);

#include "memory.h"
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task,int level,int priority);
void task_switch(void);
void task_sleep(struct TASK *task);

struct TASK *task_now(void);
void task_add(struct TASK *task);
void task_remove(struct TASK *task);
void task_switchsub(void);
#endif
