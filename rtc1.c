#include<p18f4580.h>
#include "LCD.h"
#define Bit_rate 100000
void I2C_Init(void);
char I2C_Start(char slave_write_address);
void I2C_Ready(void);
char I2C_Write(unsigned char data);
char I2C_Stop(void);
char I2C_Read(char flag);
void I2C_Ack(void);
void I2C_Nack(void);
void delay(int a);
char high_byte(unsigned char bcd);
char low_byte(unsigned char bcd);
void lcd_cmd(unsigned char b);
void lcd_data(unsigned char b);
void Send2Lcd(const char Adr,rom const char *s);
unsigned char sec,min,hour,day,date,month,year;
char I2C_repeated_start(char slave_read_address);
void main()
{

    TRISD=0X00;
    TRISC=0X00;

    lcd_cmd(0x01);
	lcd_cmd(0x38);
	lcd_cmd(0x0E);
	lcd_cmd(0x06);
	lcd_cmd(0x80);
    
        lcd_cmd(0x0c);
 delay(40);
    Send2Lcd(0x80,"Time:");
 delay(40);
    Send2Lcd(0xC0,"Date:");
   delay(40);
	

while(1)
{	

I2C_Init();
delay(5);
I2C_Ready();
delay(5);
I2C_Start(0xD0);
delay(5);
I2C_Write(0X00);
delay(5);
I2C_repeated_start(0XD1);

    sec=I2C_Read(0);
    min=I2C_Read(0);
    hour=I2C_Read(0);
    day=I2C_Read(0);
    date=I2C_Read(0);
    month=I2C_Read(0);
    year=I2C_Read(1);
    I2C_Stop();

    lcd_cmd(0X85);
	lcd_data(high_byte(hour));
	lcd_data(low_byte(hour));
	lcd_data(':');
	lcd_data(high_byte(min));
	lcd_data(low_byte(min));
	lcd_data(':');
	lcd_data(high_byte(sec));
	lcd_data(low_byte(sec));
	
	  lcd_cmd(0XC5);
	lcd_data(high_byte(date));
	lcd_data(low_byte(date));
	lcd_data(':');
	lcd_data(high_byte(month));
	lcd_data(low_byte(month));
	lcd_data(':');
	lcd_data(high_byte(year));
	lcd_data(low_byte(year));
}
}


void I2C_Init(void)
{
    TRISCbits.RC3=1;		/* Set up I2C lines by setting as input */
    TRISCbits.RC4=1;
    SSPSTAT=0x80;		/* Slew rate disabled, other bits are cleared */
    SSPCON1=0x28;	
			
    SSPCON2=0;
    SSPADD=Bit_rate;	/* Clock 100 kHz */  
    PIE1bits.SSPIE=1;		/* Enable SSPIF interrupt */
   PIR1bits.SSPIF=0;
}

char I2C_Start(char slave_write_address)
{   
    SSPCON2bits.SEN=1;		/* Send start pulse */
    while(SSPCON2bits.SEN);	/* Wait for completion of start pulse */
    PIR1bits.SSPIF=0;
    if(!SSPSTATbits.S)		/* Check whether START detected last */
    return 0;			/* Return 0 to indicate start failed */   
    return (I2C_Write(slave_write_address));	/* Write slave device address
						                        with write to communicate */
}


 void I2C_Ready(void)
{
    while(PIR2bits.BCLIF);	/* Wait if bit collision interrupt flag is set*/

    while(SSPSTATbits.BF || (SSPSTATbits.R));
    PIR1bits.SSPIF=0;  		/* Clear SSPIF interrupt flag*/
}



char I2C_Write(unsigned char data)
{
      SSPBUF=data;                	/* Write data to SSPBUF*/
      I2C_Ready();
      if (SSPCON2bits.ACKSTAT)	/* Check for acknowledge bit*/
        return 1;
      else
        return 2;
}

char I2C_Stop(void)
{
    I2C_Ready();
    SSPCON2bits.PEN=1;		/* Stop communication*/
    while(SSPCON2bits.PEN);		/* Wait for end of stop pulse*/
    PIR1bits.SSPIF = 0;
    if (!SSPSTATbits.P);  /* Check whether STOP is detected last */
    return 0;		      /* If not return 0 to indicate start failed*/
}


char I2C_Read(char flag)
{
        int buffer=0;
        SSPCON2bits.RCEN=1;			/* Enable receive */
	/* Wait for buffer full flag which when complete byte received */
        while(!SSPSTATbits.BF);
        buffer=SSPBUF;		/* Copy SSPBUF to buffer */
        SSPSTATbits.BF=0;//clear to avoid bufferfull condition
	/* Send acknowledgment or negative acknowledgment after read to 
	continue or stop reading */
        if(flag==0)
            I2C_Ack();
        else
            I2C_Nack();
        I2C_Ready();
        return(buffer);
}

char I2C_repeated_start(char slave_read_address)
{
	SSPCON2bits.RSEN=1;
	while(SSPCON2bits.RSEN);
	PIR1bits.SSPIF=0;
	if(!SSPSTATbits.S)
		return 0;
	return I2C_Write(slave_read_address);
	if(SSPCON2bits.ACKSTAT)
		return 1;
	else
		return 2;
}

 char high_byte(unsigned char bcd)
{
return((bcd>>4)+48);
}

 char low_byte(unsigned char bcd)
{
return((bcd&0X0F)+48);
}

void I2C_Ack(void)
{
    SSPCON2bits.ACKDT=0;		/* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN=1;		/* Enable ACK to send */
    while(SSPCON2bits.ACKEN);
 }


void I2C_Nack(void)
{
    SSPCON2bits.ACKDT=1;		/* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN=1;		/* Enable ACK to send */
    while(SSPCON2bits.ACKEN);
}


