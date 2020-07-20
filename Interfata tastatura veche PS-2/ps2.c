/*******************************************************
This program was created by the CodeWizardAVR V3.32a 
Automatic Program Generator
© Copyright 1998-2017 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 6/13/2018
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
#include <delay.h>

// Alphanumeric LCD functions
#include <alcd.h>

// Declare your global variables here
unsigned char data,bitcount=11;
unsigned char is_up=0, shift = 0;
char cursorPos=-1;
char unshifted[67][2] = {
//0x0d,9,
0x0e,'`',0x15,'q',0x16,'1',0x1a,'z',0x1b,'s',0x1c,'a',0x1d,'w',0x1e,'2',0x21,'c',0x22,'x',0x23,'d',0x24,'e',
0x25,'4',0x26,'3',0x29,' ',0x2a,'v',0x2b,'f',0x2c,'t',0x2d,'r',0x2e,'5',0x31,'n',0x32,'b',0x33,'h',0x34,'g',
0x35,'y',0x36,'6',0x39,',',0x3a,'m',0x3b,'j',0x3c,'u',0x3d,'7',0x3e,'8',0x41,',',0x42,'k',0x43,'i',0x44,'o',
0x45,'0',0x46,'9',0x49,'.',0x4a,'/',0x4b,'l',0x4c,';',0x4d,'p',0x4e,'-',0x52,'`',0x54,'[',0x55,'=',0x5a,13,
0x5b,']',0x5d,'/',0x61,'<',0x66,8,  0x69,'1',0x6b,'4',0x6c,'7',0x70,'0',0x71,',',0x72,'2',0x73,'5',0x74,'6',
0x75,'8',0x79,'+',0x7a,'3',0x7b,'-',0x7c,'*',0x7d,'9',0,0 };
char shifted[67][2] = {
//0x0d,9,
0x0e,'`',0x15,'Q',0x16,'!',0x1a,'Z',0x1b,'S',0x1c,'A',0x1d,'W',0x1e,'@',0x21,'C',0x22,'X',0x23,'D',0x24,'E',
0x25,'$',0x26,'#',0x29,' ',0x2a,'V',0x2b,'F',0x2c,'T',0x2d,'R',0x2e,'%',0x31,'N',0x32,'B',0x33,'H',0x34,'G',
0x35,'Y',0x36,'^',0x39,'L',0x3a,'M',0x3b,'J',0x3c,'U',0x3d,'&',0x3e,'*',0x41,'<',0x42,'K',0x43,'I',0x44,'O',
0x45,')',0x46,'(',0x49,'>',0x4a,'?',0x4b,'L',0x4c,':',0x4d,'P',0x4e,'_',0x52,'"',0x54,'{',0x55,'+',0x5a,13,
0x5b,'}',0x5d,'|',0x61,'>',0x66,8,  0x69,'1',0x6b,'4',0x6c,'7',0x70,'0',0x71,',',0x72,'2',0x73,'5',0x74,'6',
0x75,'8',0x79,'+',0x7a,'3',0x7b,'-',0x7c,'*',0x7d,'9',0,0 };

void decode(unsigned char sc) {
   unsigned char i;
   char output;
   if (!is_up) {
      switch (sc) {
         case 0xF0 : //The up-key identifier
            is_up = 1;
            break;
         case 0x12 : //Left SHIFT
            shift = 1;
            break;
         case 0x59 : //Right SHIFT
            shift = 1;
            break;
         default:           
            if(!shift) {
              for(i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
                 if (unshifted[i][0] == sc) {
                       output=unshifted[i][1];
                 }                                       
            }
            else {// If shift pressed
              for(i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
                 if (shifted[i][0] == sc) {
                       output=shifted[i][1];
                 }
            }
            cursorPos++;
            if(cursorPos == 16){
                lcd_clear();
                cursorPos = 0;
                lcd_gotoxy(0,0);
                lcd_puts("Press any key");
                lcd_gotoxy(0,1);
            }
            switch(output){
                case 8:
                    cursorPos--;
                    lcd_gotoxy(cursorPos,1);
                    lcd_putchar(' ');
                    if(cursorPos>=-1)
                    cursorPos--;
                break;
                case 13:
                    lcd_clear();
                    cursorPos = 0;
                    lcd_gotoxy(0,0);
                    lcd_puts("Press any key");
                    lcd_gotoxy(0,1);   
                break;
                default:            
                    lcd_gotoxy(cursorPos,1);
                    lcd_putchar(output);
                break;
            }                       
            break;
      }
   }
   else {
      is_up = 0;
      switch (sc) {
         case 0x12 :// Left SHIFT
            shift = 0;
            break;
         case 0x59 :// Right SHIFT
            shift = 0;
            break;
    
      }
   } 
}

interrupt [EXT_INT2] void ext_int2_isr(void)
{
// Place your code here
   if((bitcount < 11) && (bitcount > 2)) {
      data = (data >> 1);       //shiftare la dreapta
      if (PIND.2==1)            
         data = data | 0x80;    // daca pe pinul de date se gaseste valoarea 1 se pune in variabila data
      else
         data = data & 0x7f;   // daca este 0
   } 
   bitcount--;     
   if(bitcount == 0) {           
           decode(data);     
         bitcount = 11;         
    }       
}

void main(void) //Initializari uC
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
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2A output: Disconnected
// OC2B output: Disconnected
ASSR=(0<<EXCLK) | (0<<AS2);
TCCR2A=(0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (0<<WGM21) | (0<<WGM20);
TCCR2B=(0<<WGM22) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (0<<TOIE0);

// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);

// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (0<<TOIE2);


// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: On
// INT2 Mode: Falling Edge
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-15: Off
// Interrupt on any change on pins PCINT16-23: Off
// Interrupt on any change on pins PCINT24-31: Off
EICRA=(1<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EIMSK=(1<<INT2) | (0<<INT1) | (0<<INT0);
EIFR=(1<<INTF2) | (0<<INTF1) | (0<<INTF0);
PCICR=(0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

// USART0 initialization
// USART0 disabled
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (0<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);

// USART1 initialization
// USART1 disabled
UCSR1B=(0<<RXCIE1) | (0<<TXCIE1) | (0<<UDRIE1) | (0<<RXEN1) | (0<<TXEN1) | (0<<UCSZ12) | (0<<RXB81) | (0<<TXB81);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
ADCSRB=(0<<ACME);
// Digital input buffer on AIN0: On
// Digital input buffer on AIN1: On
DIDR1=(0<<AIN0D) | (0<<AIN1D);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 8
lcd_init(16);
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Press any key");
lcd_gotoxy(0,1);
#asm("sei")  //pornire interrupt-uri
while (1)
      {
      }
}
