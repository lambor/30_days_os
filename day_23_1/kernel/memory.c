#include "memory.h"
#include "io.h"

extern unsigned int _memtest_sub(unsigned int start,unsigned int end);
unsigned int memtest(unsigned int start,unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg,cr0,i;
	
	//check cpu type
	eflg = io_load_eflags();
	eflg != EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if((eflg & EFLAGS_AC_BIT) !=0)
		flg486 = 1;
	eflg &= ~EFLAGS_AC_BIT;
	io_store_eflags(eflg);
	
	//disable cpu cache
	if(flg486)
	{
		cr0 = load_cr0;
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}
	
	//i = memtest_sub(start,end);
	i = _memtest_sub(start,end);
	
	//enable cpu cache
	if(flg486)
	{
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}

	return i;
}

//the compiler will op memtest_sub,make it not work expected
unsigned int memtest_sub(unsigned int start, unsigned int end)
{
	unsigned int i,*p,old,pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	for(i=start;i<=end;i+=0x1000)
	{
		p = (unsigned int *) (i+0xffc);
		old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if(*p != pat1)
		{
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;
		if(*p != pat0)
		{
			goto not_memory;
		}
		*p = old;
	}
	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}

unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i,t = 0;
	for(i=0;i<man->frees;i++)
	{
		t+=man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man,unsigned int size)
{
	unsigned int i,a;
	for(i=0;i<man->frees;i++)
	{
		if(man->free[i].size >= size)
		{
			a = man->free[i].addr;
			man->free[i].addr+=size;
			man->free[i].size-=size;
			if(man->free[i].size == 0)
			{
				man->frees--;
				for(;i<man->frees;i++)
				{
					man->free[i] = man->free[i+1];
				}
			}
			return a;
		}
	}
	return 0;
}

int memman_free(struct MEMMAN *man,unsigned int addr,unsigned int size)
{
	int i,j;
	
	for(i=0;i<man->frees;i++)
	{
		if(man->free[i].addr>addr) break;
	}

	//if prev combinable.
	if(i>0 && (man->free[i-1].addr+man->free[i-1].size == addr))
	{
		//combine with prev
		man->free[i-1].size += size;

		//and if next combinable.
		if(i<man->frees && addr + size == man->free[i].addr)
		{
			//combine next.
			man->free[i-1].size += man->free[i].size;
			for(;i<man->frees;i++)
				man->free[i] = man->free[i+1];
			man->frees--;
		}
		
		return 0;
	}

	//if next combinable
	if(i<man->frees && (addr + size == man->free[i].addr))
	{
		man->free[i].addr = addr;
		man->free[i].size += size;
		return 0;
	}
	
	//insert a new free into man->free[]
	if(man->frees < MEMMAN_FREES)
	{
		for(j=man->frees-1;j>=i;j--)
			man->free[j+1] = man->free[j];
		if(man->maxfrees < man->frees) man->maxfrees = man->frees;
		man->free[i].addr = addr;
		man->free[i].size = size;
		man->frees++;
		return 0;
	}

	//no room for new free
	man->losts++;
	man->lostsize += size;
	return -1;
}

unsigned int memman_alloc_4k(struct MEMMAN *man,unsigned int size)
{
	unsigned int a;
	size = (size+0xfff) & 0xfffff000;
	a = memman_alloc(man,size);
	return a;
}

int memman_free_4k(struct MEMMAN *man,unsigned int addr,unsigned int size)
{
	int i;
	size = (size+0xfff) & 0xfffff000;
	i = memman_free(man,addr,size);
	return i;
}
