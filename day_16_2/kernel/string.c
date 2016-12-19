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



