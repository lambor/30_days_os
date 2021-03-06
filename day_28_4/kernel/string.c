#include"string.h"
void itoa(int value,char *buf){
	char tmp_buf[10] = {0};
	char *tbp = tmp_buf;
	if((value >> 31) & 0x1){ // neg num 
		*buf++ = '-';
		value = ~value + 1; 
	}

	do{
		*tbp++ = ('0' + (char)(value % 10));
		value /= 10;
	}while(value);
	while(tmp_buf != tbp--)
		*buf++ = *tbp;
	*buf='\0';
}

void sprintf(char *str,char *format ,...){
	int *var=(int *)(&format)+1; //得到第一个可变参数的地址
	char buffer[10];
	char *buf=buffer;
	while(*format){
		if(*format!='%'){
			*str++=*format++;
			continue;
		}
		else{
			format++;
			switch (*format){
				case 'd':itoa(*var,buf);while(*buf){*str++=*buf++;};break;
				case 'x':xtoa(*var,buf);while(*buf){*str++=*buf++;};break;
				case 's':buf=(char*)(*var);while(*buf){*str++=*buf++;};break;
			}
			buf=buffer;
			var++;
			format++;
		}
	}
	*str='\0';
}

static  inline char fourbtoc(int value){
    if(value >= 10)
        value = value - 10 + 65;
    else
        value = value + 48;
    return value;
}

void xtoa(unsigned int value,char *buf){
    char tmp_buf[30] = {0};
    char *tbp = tmp_buf;

    *buf++='0';
    *buf++='x';
    do{
        // *tbp++ = ('0' + (char)(value % 16));//得到低位数字
		*tbp++=fourbtoc(value&0x0000000f);
        
        //*tbp++ = ((value % 16)>9)?('A' + (char)(value % 16-10)):('0' + (char)(value % 16));//得到低位数字
		value >>= 4;
    }while(value);
    
    while(tmp_buf != tbp){
      tbp--;
      *buf++ = *tbp;
    }
    *buf='\0';
}

int strcmp(const char *str1,const char *str2)
{
	int i = 0;
	char c1,c2;
	while((c1 = str1[i]) && (c2 = str2[i]))
	{
		if(c1==c2) i++;
		else return c1-c2;
	}
	return str1[i] - str2[i];
}

int strncmp(const char *str1,const char *str2,unsigned int n)
{
	if(n == 0) return 0;
	unsigned int i = 0;
	char c1,c2;
	while(i<n-1 && (c1 = str1[i]) && (c2 = str2[i]))
	{
		if(c1==c2) i++;
		else return c1-c2;
	}
	return str1[i] - str2[i];
}
