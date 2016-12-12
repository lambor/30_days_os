#ifndef memory_h
#define memory_h

#define EFLAGS_AC_BIT 		0x00040000
#define CR0_CACHE_DISABLE 	0x60000000

unsigned int memtest(unsigned int start, unsigned int end);

#define MEMMAN_FREES 4090

struct FREEINFO
{
	unsigned int addr,size;
};

struct MEMMAN
{
	int frees,maxfrees,lostsize,losts;
	struct FREEINFO free[MEMMAN_FREES];
};

void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN* man, unsigned int size);
int memman_free(struct MEMMAN* man,unsigned int addr,unsigned int size);

#define MEMMAN_ADDR 0x003c0000
#endif
