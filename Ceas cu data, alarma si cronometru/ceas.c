#include <mega164a.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <delay.h>

// TWI functions
#include <twi.h>

// DS1307 Real Time Clock functions for TWI
#include <ds1307_twi.h>

// Alphanumeric LCD functions
#include <alcd.h>

// Declare your global variables here
char l1[20],l2[20],l3[20],l0[20];
int8_t menu=0;
//uint8_t crnStart = 2;
int8_t alAct = 0;
int8_t item = 0;
uint8_t prevBtnState[3] = {1,1,1};
uint8_t selected = 0;

typedef struct timp {
    uint8_t zi;
    uint8_t an;
    uint8_t luna;
    int8_t ora;
    int8_t minut;
    int8_t secunda;
    uint8_t ziSapt;
} timp;

//timp cronometru;
timp t;
timp alarma;

// Standard Input/Output functions
#include <stdio.h>

// Voltage Reference: AREF pin
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))

timp clear_time(timp tmp){
    tmp.zi = 0;
    tmp.an = 0;
    tmp.luna = 0;
    tmp.minut = 0;
    tmp.secunda = 0;
    tmp.ziSapt = 0;
    tmp.ora = 0;    
    return tmp;
}

void clear_alarm(){
        alarma = clear_time(alarma);
        alAct = 0;
}

uint8_t read_buttons(){
     if(PIND.4 == 0 && prevBtnState[0] == 1) {
        prevBtnState[0] = 0;
        delay_ms(10); 
        return 1;
     }
     else prevBtnState[0] = PIND.4; 
     if(PIND.5  == 0 && prevBtnState[1] == 1) {
        prevBtnState[1] = 0;
        delay_ms(10); 
        return 2;
     }
     else prevBtnState[1] = PIND.5; 
     if(PIND.3  == 0 && prevBtnState[2] == 1) {
        prevBtnState[2] = 0;
        delay_ms(10); 
        return 3;
     }
     else prevBtnState[2] = PIND.3; 

     return 0;
}
void update_menu(uint8_t btn){
    if(btn) {
        switch(menu){
            case 10:
            switch (btn) //alarma
                   {
                   case 1:
                        if(selected == 0){
                            item++;
                            if(item==4) 
                                item=0;
                        }
                        if(item == 1 && selected == 1){
                            alarma.ora++;
                            if(alarma.ora == 24) 
                                alarma.ora = 0;
                        }
                        if(item == 2 && selected == 1){
                            alarma.minut++;
                            if(alarma.minut == 60) 
                                alarma.minut = 0;
                        }
                        
                   break;

                   case 2:
                        if(item == 3){ 
                            menu = 0; 
                            break;
                        }
                        if(item == 0){
                            if(alAct == 0) 
                                alAct = 1;
                            else if(alAct == 1) 
                                alAct = 0;
                            break;
                        } 
                        if(selected == 0) selected=1;
                        else if(selected == 1) selected=0;
                   break;
                   
                   case 3:
                        if(selected == 0){
                            item--;
                            if(item==-1) 
                                item=3;
                        }
                        if(item == 1 && selected == 1){
                            alarma.ora--;
                            if(alarma.ora == -1) 
                                alarma.ora = 23;
                        }
                        if(item == 2 && selected == 1){
                            alarma.minut--;
                            if(alarma.minut == -1) 
                                alarma.minut = 59;
                        }
                   default:

                   break;
                   }
            break;
            default:
            switch (btn)
                   {
                   case 1:
                        menu++;
                        if(menu >= 3) 
                            menu = 0;
                   break;

                   case 2:
                        if(menu == 1)
                            menu = 10;
                        if(menu == 2) {
                            clear_alarm();    
                        }
                   break;
                   
                   case 3:
                        menu--;
                        if(menu <= -1) 
                            menu = 2;
                   break;
                   }
                
            break;
        }
        lcd_clear();
    }
}  
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    t.secunda++;
    if(t.secunda == 60) { t.minut++; t.secunda = 0;}
    if(t.minut == 60) {t.ora++; t.minut = 0;}
    if(t.ora == 24) {
        rtc_get_time(&t.ora,&t.minut,&t.secunda);
        rtc_get_date(&t.an,&t.luna,&t.zi,&t.ziSapt);
    }
    if( (t.ora == alarma.ora) && (t.minut == alarma.minut) && alAct && (t.secunda==0) ){
        PORTB.1 = 1;          
        alAct = 2;
    }
    if( (alAct == 2) && (t.secunda == 2) ){
        PORTB.1 = 0;                 
        alAct = 0;
    }
  /*  if(crnStart == 0 ){
        cronometru.secunda++;
        if(cronometru.secunda == 60) {cronometru.minut++; cronometru.secunda = 0;}
        if(cronometru.minut == 60) {cronometru.ora++; cronometru.minut = 0;}
    } */
}

void main(void)
{
// Declare your local variables here
char* tmp;
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
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (1<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
    
// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=P Bit4=P Bit3=P Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Rising Edge
// INT1: Off
// INT2: Off
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-15: Off
// Interrupt on any change on pins PCINT16-23: Off
// Interrupt on any change on pins PCINT24-31: Off
EICRA=(0<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
EIMSK=(0<<INT2) | (0<<INT1) | (1<<INT0);
EIFR=(0<<INTF2) | (0<<INTF1) | (1<<INTF0);
PCICR=(0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

// TWI initialization
// Mode: TWI Master
// Bit Rate: 100 kHz
twi_master_init(100);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// Characters/line: 20
lcd_init(20);

// Globally enable interrupts
#asm("sei")

// DS1307 Real Time Clock initialization for TWI
// Square wave output on pin SQW/OUT: On
// Square wave frequency: 1 Hz
rtc_init(0,1,0);
//rtc_set_time(15,47,00);
//rtc_set_date(2,5,6,18);
rtc_get_time(&t.ora,&t.minut,&t.secunda);
rtc_get_date(&t.ziSapt,&t.luna,&t.zi,&t.an);
while (1)
      {     
            lcd_gotoxy(0,0);
            lcd_puts(l0);
            lcd_gotoxy(0,1);
            lcd_puts(l1);
            lcd_gotoxy(0,2);
            lcd_puts(l2);
            lcd_gotoxy(0,3);
            lcd_puts(l3);
            update_menu(read_buttons());
            switch (menu)
           {    /*
           case 1:
                strcpy(l0,"Cronometru;\0");
                if(crnStart == 2){
                    strcpy(l1,"Oprit\0");
                    strcpy(l2,"        Start\0");
                }
                if(crnStart == 0){
                    itoa(cronometru.ora,tmp);
                    strcpy(l1,tmp);
                    strcat(l1,":");
                    itoa(cronometru.minut,tmp);
                    strcpy(l1,tmp);
                    strcat(l1,":");
                    itoa(cronometru.secunda,tmp);
                    strcpy(l1,tmp);
                    strcpy(l2,"         Stop\0");
                }
                else { 
                    itoa(cronometru.ora,tmp);
                    strcpy(l1,tmp);
                    strcat(l1,":");
                    itoa(cronometru.minut,tmp);
                    strcpy(l1,tmp);
                    strcat(l1,":");
                    itoa(cronometru.secunda,tmp);
                    strcpy(l1,tmp);
                    strcpy(l2,"      Restart\0");
                }
                strcpy(l3,'\0'); 
           break;  */
           case 1:
                strcpy(l0,"Alarma             ");
                strcpy(l1,"                   ");
                strcpy(l2,"           Select  ");
                strcpy(l3,"                   "); 
                
           break;
           case 10:    
                strcpy(l0,"");
                strcpy(l1,"");
                strcpy(l2,"");
                strcpy(l3,"");
                if(item==0 && selected==0) 
                    strcat(l0,"->>");
                strcat(l0,"Alarma");
                if(alAct == 1)
                    strcat(l0," activa");
                if(alAct == 0) 
                    strcat(l0," inactiva");
                if(item==1 && selected==0) 
                    strcat(l1,"->>");
                strcat(l1,"Ora :");
                itoa(alarma.ora,tmp);
                strcat(l1,tmp);
                if(item==1 && selected==1) 
                    strcat(l1,"<<-");             
                if(item==2 && selected==0) 
                    strcat(l2,"->>");
                strcat(l2,"Minut :");                
                itoa(alarma.minut,tmp);
                strcat(l2,tmp);
                if(item==2 && selected==1) 
                    strcat(l2,"<<-");
                if(item==3) 
                    strcat(l3,"->>");
                strcat(l3,"         Inapoi");     
           break;
           case 2:
                strcpy(l0,"Resetare alarme\0");
                strcpy(l1,"                 ");
                strcpy(l2,"           Select");
                strcpy(l3,"                 "); 
           break;    
           default:
                strcpy(l0,"Data: ");
                itoa(t.zi,tmp);
                strcat(l0,tmp);
                strcat(l0,".");
                itoa(t.luna,tmp);
                strcat(l0,tmp);
                strcat(l0,".");
                itoa(2000+t.an,tmp);
                strcat(l0,tmp);
                strcat(l0,"     ");
                switch(t.ziSapt){
                    case 1:
                          strcpy(tmp,"Luni\0");
                    break;
                    case 2:
                          strcpy(tmp,"Marti\0");
                    break;
                    case 3:
                          strcpy(tmp,"Miercuri\0");
                    break;
                    case 4:
                          strcpy(tmp,"Joi\0");
                    break;
                    case 5:
                          strcpy(tmp,"Vineri\0");
                    break;
                    case 6:
                          strcpy(tmp,"Sambata\0");
                    break;
                    case 7:
                          strcpy(tmp,"Duminica\0");
                    break;
                    default:
                          strcpy(tmp," ");
                    break;
                }
                strcpy(l1,tmp);
                strcat(l1,"     ");
                strcpy(l2,"Ora: ");
                itoa(t.ora,tmp);
                strcat(l2,tmp);
                strcat(l2,":");
                itoa(t.minut,tmp);
                strcat(l2,tmp);
                strcat(l2,":");
                itoa(t.secunda,tmp);
                strcat(l2,tmp);
                strcat(l2,"     ");
                strcpy(l3,"               ");
           break;
           }
      }
}
