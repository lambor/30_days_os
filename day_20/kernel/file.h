#ifndef file.h
#define file.h
struct FILEINFO
{
	char slot[32]; //reserve for vfat format
	unsigned char name[8],ext[3],type;
	char reserve[10];
	unsigned short time,data,clustno;
	unsigned int size;
};
void file_readfat(int *fat,unsigned char *img);
void file_loadfile(int clustno,int size,char *buf,int *fat,char *img);
struct FILEINFO *file_search(char *name,struct FILEINFO *finfo,int max);
#endif
