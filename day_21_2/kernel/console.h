#ifndef  console_h
#define console_h

struct CONSOLE
{
	struct SHEET *sht;
	int cur_x,cur_y,cur_c;
};

void cons_putchar(struct CONSOLE *cons,int chr,char move);
void cons_newline(struct CONSOLE *cons);
void cons_runcmd(char *cmdline,struct CONSOLE *cons,int *fat,unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons,unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons,int *fat,char *cmdline);
void cmd_hlt(struct CONSOLE *cons,int *fat);
int cmd_app(struct CONSOLE *cons,int *fat,char *cmdline);

void cons_putstr0(struct CONSOLE *cons,char *s);
void cons_putstr1(struct CONSOLE *cons,char *s,int l);
#endif
