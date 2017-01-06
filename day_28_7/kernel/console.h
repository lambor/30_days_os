#ifndef  console_h
#define console_h

struct CONSOLE
{
	struct SHEET *sht;
	int cur_x,cur_y,cur_c;
	struct TIMER *timer;
};

void cons_putchar(struct CONSOLE *cons,int chr,char move);
void cons_newline(struct CONSOLE *cons);
void cons_runcmd(char *cmdline,struct CONSOLE *cons,int *fat,unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons,unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons,int *fat,char *cmdline);
int cmd_app(struct CONSOLE *cons,int *fat,char *cmdline);
void cmd_exit(struct CONSOLE *cons,int *fat);
void cmd_start(struct CONSOLE *cons,char *cmdline,int memtotal);
void cmd_ncst(struct CONSOLE *cons,char *cmdline,int memtotal);
void cmd_langmode(struct CONSOLE *cons,char *cmdline);

void cons_putstr0(struct CONSOLE *cons,char *s);
void cons_putstr1(struct CONSOLE *cons,char *s,int l);

void start_app(int eip,int cs,int esp,int ds,int *tss_esp0);

struct SHEET *open_console(struct SHTCTL *shtctl,unsigned int memtotal);
struct TASK *open_constask(struct SHEET *sht,unsigned int memtotal);
void close_console(struct SHEET *sht);
#endif
