/*******************************************************
This program was created by the CodeWizardAVR V3.32a 
Automatic Program Generator
© Copyright 1998-2017 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 7/10/2018
Author  : 
Company : 
Comments: 


Chip type               : ATmega164A
Program type            : Application
AVR Core Clock frequency: 20.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega164a.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <delay.h>

#include <i2c.h>

#define BMP280_ADDR	0x76

// Declare your global variables here
static union bmp280_cal_union {
	uint8_t bytes[24];
	struct dig{
		uint16_t dig_t1;
		int16_t  dig_t2;
		int16_t  dig_t3;
		uint16_t dig_p1;
		int16_t  dig_p2;
		int16_t  dig_p3;
		int16_t  dig_p4;
		int16_t  dig_p5;
		int16_t  dig_p6;
		int16_t  dig_p7;
		int16_t  dig_p8;
		int16_t  dig_p9;
	};
} bmp280_cal;
int32_t bmp280_temp;
uint32_t bmp280_pres;

void sendText(char *tmp){
    int i;
    for(i = 0;i<strlen(tmp);i++){
        while ( !( UCSR0A & (1<<UDRE0)) );
        UDR0 = tmp[i];
    }
}

void sendValue(int32_t val){
    char* tmp;
    int i;    
    itoa(val,tmp);
    for(i = 0;i<strlen(tmp);i++){
        while ( !( UCSR0A & (1<<UDRE0)) );
        UDR0 = tmp[i];
    }
}


void bmp280_writeByte(uint8_t reg, uint8_t value)
{
	i2c_start();
    i2c_write( (BMP280_ADDR<<1) | 0x00 );
	i2c_write(reg);
	i2c_write(value);
	i2c_stop();
}

void bmp280_readBytes(uint8_t reg, uint8_t buff[], uint8_t bytes)
{
	uint8_t i =0;
	i2c_start();
    i2c_write( (BMP280_ADDR << 1) | 0x00 );
	i2c_write(reg);
	i2c_start();
    i2c_write( (BMP280_ADDR << 1) | 0x01 );

	for(i=0; i<bytes; i++) {
		if(i==bytes-1)
			buff[i] = i2c_read(0);
		else
			buff[i] = i2c_read(1);
	}
	i2c_stop();
}

int bmp280_getCal()      
{
    uint8_t reg = 0x88;
    uint8_t bytes = 24;
	uint8_t i =0;
    int ok = 0;
	ok = i2c_start();
    i2c_write( (BMP280_ADDR << 1) | 0x00 );
	i2c_write(reg);
	i2c_start();
    i2c_write( (BMP280_ADDR << 1) | 0x01 );

	for(i=0; i<bytes; i++) {
		if(i==bytes-1)
			bmp280_cal.bytes[i] = i2c_read(0);
		else
			bmp280_cal.bytes[i] = i2c_read(1);
	}
	i2c_stop();
    return ok;
}

int bmp280_init(){
    int8_t ok = bmp280_getCal();
    i2c_start();
    bmp280_writeByte(0xF4,0b01001011);    // X2 OVERSAMPLE TEMP ; X2 OVERSAMPLE PRES ; NORMAL MODE
    bmp280_writeByte(0xF5,0b00010000);    // 0 STANDBY ; 4 IIR FILTER ; NO 3-WIRE SPI
    i2c_stop();
    return ok;
}

void sendPres(uint32_t pres){
    char* tmp;
    uint8_t digit = pres%10;
    pres = pres/10;
    if(pres) sendPres(pres);
    itoa(digit,tmp);
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = tmp[0];    
}

void bmp280_measure(void)
{
	uint8_t data[6];
	int32_t temp_raw, pres_raw,
		var1, var2, t_fine;
	
	bmp280_readBytes(0xF7, data, 6);
	pres_raw = ((int32_t)data[0]<<12 | (int32_t)data[1]<<4 | (int32_t)data[2]>>4);
	temp_raw = ((int32_t)data[3]<<12 | (int32_t)data[4]<<4 | (int32_t)data[5]>>4);

	// The following code is based on a 32-bit integer code
	// from the BMP280 datasheet

	// compute the temperature
	var1 = ((((temp_raw >> 3) - ((int32_t)bmp280_cal.dig_t1 << 1)))
		* ((int32_t)bmp280_cal.dig_t2)) >> 11;
	var2 = (((((temp_raw >> 4) - ((int32_t)bmp280_cal.dig_t1))
		* ((temp_raw >> 4) - ((int32_t)bmp280_cal.dig_t1))) >> 12)
		* ((int32_t)bmp280_cal.dig_t3)) >> 14;
	t_fine = var1 + var2;
	bmp280_temp = (t_fine * 5 + 128) >> 8;

	// compute the pressure
	var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)bmp280_cal.dig_p6);
	var2 = var2 + ((var1 * ((int32_t)bmp280_cal.dig_p5)) << 1);
	var2 = (var2 >> 2) + (((int32_t)bmp280_cal.dig_p4) << 16);
	var1 = (((bmp280_cal.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
		+ ((((int32_t)bmp280_cal.dig_p2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((int32_t)bmp280_cal.dig_p1)) >> 15);

	if (var1 == 0) {
		bmp280_pres = 0;
	} else {
		bmp280_pres = (((uint32_t)(((int32_t)1048576)-pres_raw)
			- (var2 >> 12))) * 3125;
		if (bmp280_pres < 0x80000000) {
			bmp280_pres = (bmp280_pres << 1) / ((uint32_t)var1);
		} else {
			bmp280_pres = (bmp280_pres / (uint32_t)var1) * 2;
		}
		var1 = (((int32_t)bmp280_cal.dig_p9) * ((int32_t)(((bmp280_pres>>3) * (bmp280_pres >> 3)) >> 13))) >> 12;
		var2 = (((int32_t)(bmp280_pres >> 2)) * ((int32_t)bmp280_cal.dig_p8)) >> 13;
		bmp280_pres = (uint32_t)((int32_t)bmp280_pres + ((var1 + var2 + bmp280_cal.dig_p7) >> 4));
	}
}
void main(void)
{
// Port D initialization
// Function: Bit7=In Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=1 Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (1<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Declare your local variables here
// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: Off
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud Rate: 9600
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x81;
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
sendText("Start\n\r");
i2c_init();
sendValue(bmp280_init());
sendText("\n\rInit Ready\n\r");
// Globally enable interrupts
#asm("sei")

while (1)
{
    bmp280_measure();
    sendText("Measure Completed\n\r");
    sendText("Temperatura : ");
    sendValue(bmp280_temp/100);
    sendText(".");
    sendValue(bmp280_temp%100);
    sendText("*C | Presiune : ");
    sendPres(bmp280_pres); 
    sendText("Pa\n\r"); 
    delay_ms(3000);
    PORTD.6 = ~PORTD.6;
}
}
