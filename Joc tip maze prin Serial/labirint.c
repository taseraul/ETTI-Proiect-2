#include <io.h>
#include <stdio.h>
#include <mega164a.h>
#include <delay.h>

//initializare matrice labirint
char map[21][21] = {
    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'},
    {' ',' ',' ',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ',' ',' ','#'},
    {'#',' ','#','#','#',' ','#',' ','#',' ','#',' ','#',' ','#','#','#','#','#',' ','#'},
    {'#',' ','#',' ','#',' ','#',' ','#',' ','#',' ','#',' ',' ',' ',' ',' ',' ',' ','#'},
    {'#',' ','#',' ','#',' ','#',' ','#',' ','#',' ','#','#','#','#','#',' ','#','#','#'},
    {'#',' ','#',' ','#',' ','#',' ','#',' ',' ',' ','#',' ','#',' ',' ',' ','#',' ','#'},
    {'#',' ','#',' ','#',' ','#',' ','#','#','#','#','#',' ','#',' ','#',' ','#',' ','#'},
    {'#',' ','#',' ','#',' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ','#',' ','#',' ','#'},
    {'#',' ','#',' ','#','#','#',' ','#',' ','#','#','#','#','#','#','#',' ','#',' ','#'},
    {'#',' ','#',' ','#',' ',' ',' ','#',' ',' ',' ',' ',' ','#',' ',' ',' ','#',' ','#'},
    {'#',' ','#',' ','#',' ','#','#','#','#','#','#','#',' ','#',' ','#','#','#',' ','#'},
    {'#',' ',' ',' ','#',' ','#',' ',' ',' ',' ',' ','#',' ','#',' ',' ',' ',' ',' ','#'},
    {'#','#','#','#','#',' ','#',' ','#','#','#',' ','#',' ','#','#','#','#','#','#','#'},
    {'#',' ',' ',' ',' ',' ','#',' ','#',' ','#',' ','#',' ','#',' ',' ',' ','#',' ','#'},
    {'#',' ','#','#','#','#','#',' ','#',' ','#',' ','#',' ','#',' ','#',' ','#',' ','#'},
    {'#',' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ','#',' ','#',' ','#',' ',' ',' ','#'},
    {'#','#','#',' ','#',' ','#','#','#',' ','#','#','#',' ','#',' ','#','#','#',' ','#'},
    {'#',' ','#',' ',' ',' ','#',' ',' ',' ',' ',' ','#',' ','#',' ',' ',' ','#',' ','#'},
    {'#',' ','#','#','#','#','#',' ','#','#','#','#','#',' ','#','#','#',' ','#',' ','#'},
    {'#',' ',' ',' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',' ',' ',' ',' ','#',' ',' '},
    {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}
};
//screen final    
char go[3][21] = {
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ','G','a','m','e',' ','o','v','e','r',' ','!',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}
};
// pozitia de start
int curent_pos[2]={1,0};

//definirea functiilor folosite
void print_map();
void update_map();

void main(void)
{

//initializare pini de input/output
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (1<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// initializare uart tx only 9600 baud asyncronous
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x81;

    //afisare labirint
    print_map();
    while (1)
    {   
        // verificare status butoane
        if( !(PIND.5 && PIND.4 && PIND.3 && PIND.2)){
            update_map();
        //updatare harta daca s-a apasat vreun buton   
        }
        delay_ms(100);//debounce timer
    }                   
}

void print_map(){
    int i=0;
    int j=0;
    //astept golirea bufferului de transmisie
    while( !(UCSR0A & (1<<UDRE0)) );
    UDR0 = 0x0A;// incarc newline in buffer
    for(i=0;i<=20;i++){
        for(j=0;j<=20;j++){
            //astept golirea bufferului de transmisie
            while( !(UCSR0A & (1<<UDRE0)) );
            if(i == curent_pos[0] && j == curent_pos[1]){
                UDR0 = 0x4F; //incarc litera "O" pe pe pozitia jucatorului
            }
            else{    
                UDR0 = map[i][j];//restul hartii
            }
        }
        while( !(UCSR0A & (1<<UDRE0)) ); //astept golirea bufferului
        UDR0 = 0x0A;//newline
    }
}

void update_map(){
    //in functie de statusul butoanelor si de posibilitatea miscarii updatez harta
    if(!PIND.4){
        if( (curent_pos[1]-1) >= 0){
            if(map[curent_pos[0]][curent_pos[1]-1] == ' '){
                curent_pos[1] -= 1;
            }
        }
    }
    if(!PIND.5){
        if( (curent_pos[1]+1) <= 40){
            if(map[curent_pos[0]][curent_pos[1]+1] == ' '){
                curent_pos[1] += 1;
            }
        }
    }
    if(!PIND.3){
        if( (curent_pos[0]-1) >= 0){
            if(map[curent_pos[0]-1][curent_pos[1]] == ' '){
                curent_pos[0] -= 1;
            }
        }
    }
    if(!PIND.2){
        if( (curent_pos[0]+1) <= 40){
            if(map[curent_pos[0]+1][curent_pos[1]] == ' '){
                curent_pos[0] += 1;
            }
        }
    }
    //incarc game over screen peste labirint daca jucatorul a ajuns la sfarsit
    if(curent_pos[0] == 19 && curent_pos[1] == 20){
        int i=0;
        int j=0;
        for(i=0;i<3;i++){
            for (j=0;j<21;j++)
            {
                map[i+9][j] = go[i][j];
            }         
        }
    }          
    print_map();// afisez noul labirint
}