#include <io.h>
#include <delay.h>
#include <mega164a.h>
#include <string.h>
#include <stdio.h>

int tip = 0;  // variabila care indica modul (crt/ tens)

void sendText(char *txt){               //functia care trimite un sir de caractere pe seriala
    int i;
    for(i=0;i<strlen(txt);i++){
        while ( !( UCSR0A & (1<<UDRE0)) );     //udre0 flag-ul de finalizare a transmisiei (0 asteapta ;1 terminat)
        UDR0 = txt[i];         // incarca caracterul in registrul de tranmisie
    }
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = '\n';  //registrul in care se pune valoare pe 8 biti de trimis
}

#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))
// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

#define VOLTAGE 0
#define CURRENT 1
float compute(int type){                        //functia care citeste valorile tensiunilor analogice 
    int i;
    float current = 0, voltage = 0;
    
    if(type == VOLTAGE){                           //citire si prelucrare voltmetru
    
        for(i=0;i<9;i++){
                voltage=voltage+read_adc(1);
                delay_us(10);
            }
            voltage=(float)voltage/10;
            voltage=(float)voltage*5/1024;
            return voltage;
    
    }else if(type == CURRENT){                     //citire si prelucrare ampermetru
        float volts=0;
        for(i=0;i<9;i++){
                volts=volts+read_adc(0);
                delay_us(10);
            }
            volts=(float)volts/10;
            volts=(float)volts*5/1024;
            current=(volts-2.5)/0.2;
            return current;
        
    }else return -1;

}

void check_button(){            //functia care citeste butonul de comutare intre modurile multimetrului (volt/amper)
    if(PIND.5 == 0)
    PORTD.6 = PORTD.6 ? 0 : 1;  //led stins pt crt si aprins pt tensiune
    tip = tip ? 0 : 1;          //tipul de masura 0 - curent 1 - tensiune
    delay_ms(200);              //debounce time
}

void main(void) {

    // Crystal Oscillator division factor: 1
    #pragma optsize-
    CLKPR=(1<<CLKPCE);
    CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif 
    
    // Port A initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0); 
    
    // Port D initialization
    // Function: Bit7=In Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRD=(0<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=T Bit6=0 Bit5=P Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
    
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
    
    // ADC initialization
    // ADC Clock frequency: 625.000 kHz
    // ADC Voltage Reference: AREF pin
    // ADC Auto Trigger Source: ADC Stopped
    // Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
    // ADC4: On, ADC5: On, ADC6: On, ADC7: On
    DIDR0=(0<<ADC7D) | (0<<ADC6D) | (0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
    ADMUX=ADC_VREF_TYPE;
    ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
    ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);  
    
    #asm("sei"); // set interrupts (activeaza interrupt-urile/ cli clear interrupts   
    
    while(1){
        char data[20];
        if(tip){                                         //in functie de variabila citeste curentul / tensiunea si o afiseaza pe seriala
            sendText("voltage");
            sprintf(data, "%.2f V", compute(VOLTAGE));
            sendText(data);
        
        }
        else {
            sendText("current");
            sprintf(data, "%.2f A", compute(CURRENT));
            sendText(data);
        }
        check_button();
    }
}