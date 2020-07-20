#include <io.h>
#include <delay.h>
#include <stdint.h>
// TWI functions
#include <twi.h>
#include <eeprom.h>
#include <string.h>
#include <stdlib.h>

// DS1307 Real Time Clock functions for TWI
#include <ds1307_twi.h>

#define HC595_PORT   PORTB
#define HC595_DDR    DDRB
#define HC595_DS_POS PORTB0      //Data pin (DS) pin location
#define HC595_SH_CP_POS PORTB1      //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS PORTB3      //Store Clock (ST_CP) pin location

uint8_t sym[11]={ 0b11111100, //0
                  0b11111001, //1
                  0b10100100, //2
                  0b10110000, //3
                  0b10011001, //4
                  0b10110110, //5
                  0b10111110, //6
                  0b11100000, //7
                  0b11111110, //8
                  0b11110110, //9
                  0b11111111, //blank
};
uint16_t tCounter=0;
uint16_t tAlarma=0;
uint8_t timpAlarma = 1;
uint8_t i,alGeneral=0,armGeneral=0;
uint8_t almON = 0;
//char cmd[20]="";


typedef struct camera
{
   uint8_t stareAlarma;
   uint8_t stareArmare;
}camera;
camera c[3];

void initc(){
    int i=0;
    for(i=0;i<3;i++)
    {
        c[i].stareAlarma = 0;
        c[i].stareArmare = 0;
    }
}

uint8_t indexAfisat = 0;
void updateSeg();
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

void updateLeds(){
    alGeneral=0;
    armGeneral=0;
    for(i=0;i<3;i++){
        alGeneral |= c[i].stareAlarma;
    } 
    if(alGeneral){ 
        PORTD.6 = 1;
        while(c[indexAfisat].stareAlarma==0){
            updateSeg();
        }
    }
    else 
        PORTD.6 = 0;
    for(i=0;i<3;i++){
        armGeneral += c[i].stareArmare;
    }
    if(armGeneral==0) 
        PORTC.3 = ~PORTC.3;
    else if(c[indexAfisat].stareArmare == 0) 
        PORTC.3 = 1;
        else
        PORTC.3 = 0;
    if(armGeneral==3) { 
        PORTC.2 = 1;
        PORTD.7 = 0;
    }
    else { 
        if(c[indexAfisat].stareArmare) 
            PORTD.7 = 1;
        else
            PORTD.7 = 0;
        PORTC.2 = 0;
    }
}

interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    uint8_t i=0;
    // Reinitialize Timer1 value
    TCNT1H=0xD9DB >> 8;
    TCNT1L=0xD9DB & 0xff;
    // Place your code here
    tCounter++; 
    if(tCounter%10 == 0) updateSeg();
    updateLeds();
    
    
    if(almON) tAlarma++;
    if(tAlarma == 2*timpAlarma && almON){
        for(i=0;i<3;i++){
            c[i].stareAlarma = 0;
            c[i].stareArmare = 1;
        }
        almON = 0;
        PORTC.5 = 0;
        tAlarma = 0;
    } 
}
void writeEEPROM(uint8_t zona){
    int i = 0;
    uint8_t h,min,sec;
    while( eeprom_read_byte(i) != 0xFF){
        i++;
    }
    rtc_get_time(&h,&min,&sec);
    eeprom_write_byte ( i, h);
    i++;
    eeprom_write_byte ( i, min);
    i++;
    eeprom_write_byte ( i, sec);
    i++;
    eeprom_write_byte ( i, zona);
    i++;
    if (i==512) i = 0;
    eeprom_write_byte ( i,0xFF);
    i++;
    eeprom_write_byte ( i,0xFF);
    i++;
    eeprom_write_byte ( i,0xFF);
    i++;
    eeprom_write_byte ( i,0xFF);   
}/*
void send(char *txt)
{
     int i;
     for(i=0;i<strlen(txt);i++){
        while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Do nothing until UDR is ready for more data to be written to it
        UDR0 = txt[i];                          // Load data to be transferred
      }
} 
void exec(){
    int i = 0;
    char c[40];
    char* tmp;
    if(~strcmp(cmd,"status")){
        while( eeprom_read_byte(i) != 0xFF){
            i++;
        }
        if(i == 0) i = 508;
        itoa(eeprom_read_byte(i),tmp);
        strcpy(c,tmp);
        strcat(c,":");
        itoa(eeprom_read_byte(++i),tmp);
        strcpy(c,tmp);
        strcat(c," zona");
        i+=2;
        itoa(eeprom_read_byte(i),tmp);
        strcpy(c,tmp);
        strcat(c,tmp);
        send(c);    
    }
    if(strcmp(cmd,"set")>=3){
    
    }
}  */
// External Interrupt 0 service routine
interrupt [EXT_INT0] void interrupt_0(void)
{
    if(c[0].stareArmare == 1){  
        c[0].stareAlarma = 1;
        tAlarma=0;
        PORTC.5 = 1;
        almON = 1;
        writeEEPROM(1);
        updateLeds();
        HC595Write(sym[10]);
        HC595Write(sym[1]);
    }
    delay_ms(10);
}

// External Interrupt 1 service routine
interrupt [EXT_INT1] void interrupt_1(void)
{
// Place your code here
    if(c[1].stareArmare == 1){ 
        c[1].stareAlarma = 1;
        tAlarma=0;
        PORTC.5 = 1;
        almON = 1;
        writeEEPROM(2);
        updateLeds();
        HC595Write(sym[10]);
        HC595Write(sym[2]);
    }
    delay_ms(10);
}

// External Interrupt 2 service routine
interrupt [EXT_INT2] void interrupt_2(void)
{
// Place your code here
    if(c[2].stareArmare == 1){ 
        c[2].stareAlarma = 1;
        tAlarma=0;
        PORTC.5 = 1;
        almON = 1;
        writeEEPROM(3);
        updateLeds();
        HC595Write(sym[10]);
        HC595Write(sym[3]);
    }
    delay_ms(10);
}

void updateSeg(){
    indexAfisat++;                 
    if(indexAfisat == 3) indexAfisat=0;
    HC595Write(sym[10]);
    HC595Write(sym[indexAfisat+1]);
}

void main()
{
    uint8_t prevState[2] = {1,1};
    
    // Port C initialization
    // Function: Bit7=In Bit6=In Bit5=Out Bit4=In Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRC=(0<<DDC7) | (0<<DDC6) | (1<<DDC5) | (0<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRD=(1<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=T Bit6=P Bit5=P Bit4=P Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

    
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 19.531 kHz
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer Period: 0.49997 s
    // Timer1 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
    TCNT1H=0xD9;
    TCNT1L=0xDB;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;
    TIMSK1=0x01;

    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Rising Edge
    // INT1: On
    // INT1 Mode: Rising Edge
    // INT2: On
    // INT2 Mode: Rising Edge
    // Interrupt on any change on pins PCINT0-7: Off
    // Interrupt on any change on pins PCINT8-15: Off
    // Interrupt on any change on pins PCINT16-23: Off
    // Interrupt on any change on pins PCINT24-31: Off
    EICRA=(1<<ISC21) | (1<<ISC20) | (1<<ISC11) | (1<<ISC10) | (1<<ISC01) | (1<<ISC00);
    EIMSK=(1<<INT2) | (1<<INT1) | (1<<INT0);
    EIFR=(1<<INTF2) | (1<<INTF1) | (1<<INTF0);
    PCICR=(0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);
    
    // USART0 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART0 Receiver: On
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    // USART0 Baud Rate: 9600
    UCSR0A=(1<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
    UCSR0B=(1<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
    UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
    UBRR0H=0x00;
    UBRR0L=0x81;
    
    // TWI initialization
    // Mode: TWI Master
    // Bit Rate: 100 kHz
    twi_master_init(100);

    // DS1307 Real Time Clock initialization for TWI
    // Square wave output on pin SQW/OUT: Off
    // SQW/OUT pin state: 0
    rtc_init(0,0,0);
    //Send each 8 bits serially
    
    // Globally enable interrupts
    #asm("sei")
    //Initialize HC595 system
    HC595Init();
    initc();
    updateLeds();
    HC595Write(sym[10]);
    HC595Write(sym[indexAfisat+1]);
    while(1)
    {
        if(PIND.4 == 0 && prevState[0]==1) {
            updateSeg();
            updateLeds();
            prevState[0]=0;
            delay_ms(10);  
        }
        else {
            prevState[0]=PIND.4;                            
            delay_ms(10); 
        }
        if(PIND.5 == 0 && prevState[1]==1) {
            if(alGeneral){
                initc();
                almON = 0;
                tAlarma = 0;
            }
            else {
                if(c[indexAfisat].stareArmare == 0) c[indexAfisat].stareArmare =1;
                else if(c[indexAfisat].stareArmare == 1) c[indexAfisat].stareArmare =0;
            }
            prevState[1]=0;
            updateLeds();
            delay_ms(10);   
        }
        else {
            prevState[1]=PIND.5;
            delay_ms(10);
        }                           
    }
}