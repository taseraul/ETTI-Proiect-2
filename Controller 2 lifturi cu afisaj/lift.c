/*******************************************************
This program was created by the CodeWizardAVR V3.32a 
Automatic Program Generator
© Copyright 1998-2017 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 5/30/2018
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
#include <delay.h>

#define L1UP PORTC.3
#define L1DOWN PORTC.4
#define L2UP PORTC.5
#define L2DOWN PORTC.6 

#define HC595_PORT          PORTB
#define HC595_DDR           DDRB
#define HC595_DS_POS        PORTB0      //Data pin (DS) pin location
#define HC595_SH_CP_POS     PORTB1      //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS     PORTB2      //Store Clock (ST_CP) pin location

// Declare your global variables here
uint8_t source = 3;
uint8_t lift1[3]={0,0,0},lift2[3]={0,0,0};
int8_t dirL1 = 0, dirL2 = 0;
int8_t cPos1 = 1,cPos2 = 1;
uint8_t n[10]=
{
  0b11111100, //0
  0b11111001, //1
  0b10100100, //2
  0b10110000, //3
  0b10011001, //4
};
uint8_t prevBtn[10]={1,1,1,1,1,1,1,1,1,1};

void HC595Init()
{
   //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
   HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
}
//Low level macros to change data (DS)lines
#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))
#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))
//Sends a clock pulse on SH_CP line
void HC595Pulse()
{
   //Pulse the Shift Clock
   HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH
   HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW
}

//Sends a clock pulse on ST_CP line
void HC595Latch()
{
   //Pulse the Store Clock
   HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
   delay_us(5);
   HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
   delay_us(5);
}
  
void HC595Write(uint8_t data)
{
    uint8_t i;
    //Order is MSB first
    for(i=0;i<8;i++)
    {
      //Output the data on DS line according to the
      //Value of MSB
      if(data & 0b10000000)
      {
         //MSB is 1 so output high

         HC595DataHigh();
      }
      else
      {
         //MSB is 0 so output high
         HC595DataLow();
      }

      HC595Pulse();  //Pulse the Clock line
      data=data<<1;  //Now bring next bit at MSB position

    }

   //Now all 8 bits have been transferred to shift register
   //Move them to output latch at one
   HC595Latch();
}

void update_pannel(){
    if(dirL2 == 0) HC595Write(n[cPos2] & 0x7F);
    else HC595Write(n[cPos2]);
    if(dirL1 == 0) HC595Write(n[cPos1] & 0x7F);
    else HC595Write(n[cPos1]);
}

void queue(uint8_t etaj){
    uint8_t i=0;
    for(i=0;i<3;i++){
        if(lift1[i] == etaj || lift2[i] == etaj)   //verifica daca este deja un lift la acel etaj
            return;
    }
    switch(source){
        case 1:
            if(dirL1 == 0){
                lift1[0] = etaj;
                break;
            }
            if(lift1[1] == 0){
                lift1[1] = etaj;
                break;
            }
            if(lift1[2] == 0){
                if( (cPos1 - etaj <= -1) && dirL1 == 1 ){
                    lift1[1] = etaj;
                    break;
                }
                if( (cPos1 - etaj >= 1) && dirL1 == -1 ){
                    lift1[1] = etaj;
                    break;
                }
                lift1[2] = etaj;
                break;
            }
        break;
        case 2:
            if(dirL2 == 0){
                lift2[0] = etaj;
                break;
            }
            if(lift2[1] == 0){
                lift2[1] = etaj;
                break;
            }
            if(lift2[2] == 0){
                if( (cPos2 - etaj <= -1) && dirL2 == 1 ){
                    lift2[1] = etaj;
                    break;
                }
                if( (cPos2 - etaj >= 1) && dirL2 == -1 ){
                    lift2[1] = etaj;
                    break;
                }
                lift2[2] = etaj;
                break;
            }
        break;
        case 3:
            if(dirL1 == 0){
                lift1[0] = etaj;
                break;
            }
            if(dirL2 == 0){
                lift2[0] = etaj;
                break;
            }
            if(lift1[1] == 0){
                if( (cPos1 - etaj <= -1) && dirL1 == 1 ){
                    lift1[1] = etaj;
                    break;
                }
                if( (cPos1 - etaj >= 1) && dirL1 == -1 ){
                    lift1[1] = etaj;
                    break;
                }   
            }
            else
                if(lift1[2] == 0){
                    if(lift1[0]<etaj && etaj<lift1[1]){
                        lift1[2]=lift1[1];
                        break;
                    }
                    if(lift1[0]>etaj && etaj>lift1[1]){
                        lift1[2]=lift1[1];
                        break;
                    }
                }
            if(lift2[1] == 0){
                if( (cPos2 - etaj <= -1) && dirL2 == 1 ){
                    lift2[1] = etaj;
                    break;
                }
                if( (cPos2 - etaj >= 1) && dirL2 == -1 ){
                    lift2[1] = etaj;
                    break;
                }   
            }
            else
                if(lift2[2] == 0){
                    if(lift2[0]<etaj && etaj<lift2[1]){
                        lift2[2]=lift2[1];
                        break;
                    }
                    if(lift2[0]>etaj && etaj>lift2[1]){
                        lift2[2]=lift2[1];
                        break;
                    }
                }
            if(lift1[2] == 0) lift1[2] = etaj;
            if(lift2[2] == 0) lift2[2] = etaj;
        break;
    }    
}

void btn(){
    uint8_t delay = 0;
    if(~PIND.2 && prevBtn[0] == 1) {
        queue(1);
        prevBtn[0] = 0;
        delay = 1;        //et1
    }
    else prevBtn[0] = PIND.2;
    if(~PIND.3 && prevBtn[1] == 1){
        queue(2);
        prevBtn[1] = 0;
        delay = 1;        //e2
    }
    else prevBtn[1] = PIND.3;
    if(~PIND.4 && prevBtn[2] == 1){
        queue(3);
        prevBtn[2] = 0;
        delay = 1;        //e3
    }
    else prevBtn[2] = PIND.4;
    if(~PIND.5 && prevBtn[3] == 1){
        queue(4);
        prevBtn[3] = 0;
        delay = 1;        //e4
    }
    else prevBtn[3] = PIND.5;
    if(~PIND.7 && prevBtn[4] == 1){
        source = 1;
        prevBtn[4] = 0;
        delay = 1;        //int1
    }
    else prevBtn[4] = PIND.7;
    if(~PINC.0 && prevBtn[5] == 1){
        source = 2;
        prevBtn[5] = 0;
        delay = 1;        //int2
    }
    else prevBtn[5] = PINC.0;
    if(~PINC.1 && prevBtn[6] == 1){
        source = 3;
        prevBtn[6] = 0; 
        delay = 1;        //ext
    }
    else prevBtn[6] = PINC.1;
    if(~PINC.2 && prevBtn[7] == 1){
        prevBtn[7] = 0;
        if(dirL1 != 0){
            cPos1 = cPos1 + dirL1;
            update_pannel();
        
        }        //detectie etaj L1
        delay = 1;
    }
    else prevBtn[7] = PINC.2;
    if(~PINC.7 && prevBtn[8] == 1){
        prevBtn[8] = 0;
        if(dirL2 != 0){
            cPos2 = cPos2 + dirL2;
            update_pannel();
        }        //detectie etaj L2
        delay = 1;
    }
    else prevBtn[8] = PINC.7;
    if(delay)
        delay_ms(150);
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

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=P Bit2=P Bit1=P Bit0=P 
PORTC=(1<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=P Bit6=T Bit5=P Bit4=P Bit3=P Bit2=P Bit1=T Bit0=T 
PORTD=(1<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (1<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

    HC595Init();
    update_pannel();
    while (1)
    {   
        int8_t check1 = dirL1;
        int8_t check2 = dirL2;
        btn();
        if(cPos1 == lift1[0]){
            lift1[0]=lift1[1];
            lift1[1]=lift1[2];
            lift1[2]=0;
        }
        if(cPos2 == lift2[0]){
            lift2[0]=lift2[1];
            lift2[1]=lift2[2];
            lift2[2]=0;
        }
        if(lift1[0] == 0) dirL1 = 0;
        if(lift2[0] == 0) dirL2 = 0;
        if(lift1[0]>cPos1 && lift1[0]){
            dirL1 = 1;
        }
        if(lift1[0]<cPos1  && lift1[0]){
            dirL1 = -1;
        }
        if(lift2[0]>cPos2 && lift2[0]){
            dirL2 = 1;
        }
        if(lift2[0]<cPos2 && lift2[0]){
            dirL2 = -1;
        }
        if(dirL1 == 1){
            L1UP = 1;
            L1DOWN = 0;
        }
        if(dirL1 == -1){
            L1DOWN = 1;
            L1UP = 0;
        }
        if(dirL2 == 1){
            L2UP = 1;
            L2DOWN = 0;
        }
        if(dirL2 == -1){
            L2DOWN = 1;
            L2UP = 0;
        }
        if(lift1[0] == 0){
            L1DOWN = 0;
            L1UP = 0; 
            dirL1=0;        
        }
        if(lift2[0] == 0){
            L2DOWN = 0;
            L2UP = 0;
            dirL2=0; 
        }
        if(dirL1 != check1 || dirL2 != check2)
            update_pannel();                                     
    }
}
