
void delay(int a)
{
	int i,j;
	for(i=0;i<a;i++)
	for(j=0;j<100;j++);
}

void lcd_cmd(unsigned char b)
{
	PORTD=b;
	PORTCbits.RC0=0;
    PORTCbits.RC1=1;
	delay(40);
	PORTCbits.RC1=0;
}
void lcd_data(unsigned char b)
{
	PORTD=b;
	PORTCbits.RC0=1;
    PORTCbits.RC1=1;
	delay(40);
    PORTCbits.RC1=0;
}
void Send2Lcd(const char Adr,rom const char *s)
{
  lcd_cmd(Adr);
while(*s!='\0')
  {
   lcd_data(*s);
   s++;
  }
}

