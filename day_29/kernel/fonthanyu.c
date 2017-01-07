#include "fonthanyu.h"
#include "memory.h"
#include "const.h"
#include "file.h"
unsigned char *hanyu;
void load_hanyu()
{
	int *fat;
	struct FILEINFO *finfo;
	extern char hankaku[4096];

	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	hanyu = (unsigned char *)memman_alloc_4k(memman,16*256+32*94*55);
	fat = (int *)memman_alloc_4k(memman,4*2880);
	file_readfat(fat,(unsigned char *)(ADR_DISKIMG + 0x000200));
	finfo = file_search("hz.fnt",(struct FILEINFO *)(ADR_DISKIMG + 0x002600),224);
	for(int i=0;i<16*256;i++)
		hanyu[i] = hankaku[i];
	if(finfo != 0)
		file_loadfile(finfo->clustno,finfo->size,hanyu+16*256,fat,(char *)(ADR_DISKIMG + 0x003e00));
	else
	{
		for(int i=16*256;i<16*256+32*94*55;i++)
		{
			hanyu[i] = 0xff;
		}
	}
	*((int *)0x0fe8) = (int) hanyu;
	memman_free_4k(memman,(int) fat,4*2880);
}
