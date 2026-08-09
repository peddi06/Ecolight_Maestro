#include<lpc21xx.h>
void lcd_data(unsigned char data)
{
//Higher nibble
unsigned int temp;
IOCLR1=0xFE<<16;
temp=(data&0xF0)<<16;
IOSET1=temp;
IOSET1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;

//Lower nibble
IOCLR1=0XFE<<16;
temp=(data & 0x0f)<<20;
IOSET1=temp;
IOSET1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;
}
void lcd_cmd(unsigned char cmd)
{
//Higher nibble
unsigned int temp;
IOCLR1=0xFE<<16;
temp=(cmd&0xf0)<<16;
IOSET1=temp;
IOCLR1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;

//Lower nibble
IOCLR1=0xFE<<16;
temp=(cmd&0x0f)<<20;
IOSET1=temp;
IOCLR1=1<<17;
IOCLR1=1<<18;
IOSET1=1<<19;
delay_ms(2);
IOCLR1=1<<19;
}
void lcd_init()
{
IODIR1=0XFE<<16;
lcd_cmd(0x02);
lcd_cmd(0x28);
lcd_cmd(0x0c);
//lcd_cmd(0x0e);
lcd_cmd(0x01);
}

void lcd_string(char *str)
{
	while(*str!=0)
	{
		lcd_data(*str);
		str++;
	}
}
