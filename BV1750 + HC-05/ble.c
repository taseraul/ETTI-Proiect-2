/*******************************************************
This program was created by the CodeWizardAVR V3.32a 
Automatic Program Generator
© Copyright 1998-2017 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 8/6/2018
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

// TWI functions
#include <twi.h>

// BH1750TVI Ambient Light Sensor functions for TWI
#include <bh1750_twi.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>

// Declare your global variables here

// Standard Input/Output functions
#include <stdio.h>

void sendText(char *txt){
    int i;
    for(i=0;i<strlen(txt);i++){
        while ( !( UCSR0A & (1<<UDRE0)) );     //udre0 flag-ul de finalizare a transmisiei (0 asteapta ;1 terminat)
        UDR0 = txt[i];
    }
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = '\n';  //registrul in care se pune valoare pe 8 biti de trimis
}


void sendToBLE(float val){
    switch ((int)val/2000)
           {
           case 0:
            sendText("Intuneric");
           break;

           case 1:
            sendText("Slab luminat");
           break;
           
           case 3:
            sendText("Luminat");
           break;
           
           case 4:
           default:
            sendText("Puternic luminat");
           break;
           }
}

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

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

// TWI initialization
// Mode: TWI Master
// Bit Rate: 100 kHz
twi_master_init(100);

// Globally enable interrupts
#asm("sei")

// BH1750TVI Ambient Light Sensor initialization for TWI
// I2C bus address: 23h ADDR Pin=0
// Measurement time register: 69
bh1750_init(BH1750_I2C_ADDR0,69);

while (1)
      {
      // Place your code here
      if(bh1750_start(BH1750_I2C_ADDR0,BH1750_SINGLE_CONV_HIGH_RES2) == BH1750_RES_OK){
        float lx;
        delay_ms(500);
        lx = bh1750_rdlight(BH1750_I2C_ADDR0);
        if(lx >= 0)
            sendToBLE(lx);
        else         
            if(lx == BH1750_COMM_ERROR)
                sendText("Eroare de comunicare");
      }
      else
        sendText("Comm fail");
      delay_ms(2000);
      }
}