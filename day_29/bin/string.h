#ifndef string_h
#define string_h
void itoa(int value,char *buf);
void sprintf(char *str,char *format ,...);
void xtoa(unsigned int value,char *buf);
int strcmp(const char *str1,const char *str2);
int strncmp(const char *str1,const char *str2,unsigned int n);
int strlen(const unsigned char * str);
#endif

