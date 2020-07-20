#include <LiquidCrystal.h>
#include <stdlib.h>

#define rs 11
#define en 12
#define d4 4
#define d5 5
#define d6 6
#define d7 7
#define IN 3
#define OUT 2
#define LED 13

#define timeOut 5000

unsigned long long checkTime = 0;     //timpul la care s-a facut prima citire   
bool direction = 0;     //directia 0 = out / 1 = in

int peopleInside = 0;       //variabila nr persoane
bool readState[2] = {0,0};      // strea intrerupatorului in loop-ul curent
bool prevReadState[2] = {0,0};      //starea intrerupatorului in loop-ul precedent

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);    // initializare lcd

void setup() {
  // put your setup code here, to run once:
  pinMode(IN,INPUT);
  pinMode(OUT,INPUT);
  pinMode(LED,OUTPUT);
  lcd.begin(16, 2);   
  Serial.begin(9600);  
  updateLCD();
  delay(timeOut);   // delay pt ca atunci cand resetam timpul cu checkTime = millis() - timeOut sa nu se ajunga la overflow si checkTime sa fie f mare
}

void loop() {
  prevReadState[0] = readState[0]; // mut valorile citite anterior 
  prevReadState[1] = readState[1];
  readState[0] = digitalRead(OUT);  //citesc noile valori
  readState[1] = digitalRead(IN);
  if(readState[0] != prevReadState[0] || readState[1] != prevReadState[1]) delay(200);  //daca s-a detectat un front atunci aplic un delay pt debounce
 // Serial.print("OUT: ");
  //Serial.print(readState[0],DEC);
  //Serial.print("IN: ");
  //Serial.println(readState[1],DEC);    //millis() returneaza timpul in milisec de cand uC este pornit
  if((checkTime + timeOut) > millis()){     //checkTime stocheaza timpul la care s-a facut prima citire iar timeOut-ul reseteaza numarul de citiri daca se depaseste o anumita durata
      if(readState[1] == 1 && prevReadState[1] == 0 && direction == 0){  //daca se detecteaza front pe IN din directia OUT -> a iesit o persoana
        peopleInside--;
        checkTime = millis() - timeOut;
        //Serial.print(peopleInside,DEC);
        updateLCD();
        //Serial.println("if1");
        delay(100);
      }
      if(readState[0] == 1 && prevReadState[0] == 0 && direction == 1){ //daca se detecteaza front pe OUT din directia IN -> a intrat o pers
        peopleInside++;
        checkTime = millis() - timeOut;
        //Serial.print(peopleInside,DEC);
        updateLCD();
        //Serial.println("if2");
        //delay(100);
      }
      if(readState[1] == 1 && prevReadState[1] == 1){ // in cazul in care persoana se opreste "in usa" se reinitializeaza timpul la care s-a facut prima citire
        checkTime = millis();
        //Serial.println("if3");
      }
      if(readState[0] == 1 && prevReadState[0] == 1){
        checkTime = millis();
        //Serial.println("if4");
      }
  }
  
  else{ //prima citire 
      if(readState[1] == 1 && prevReadState[1] == 0){   //front pe IN -> setez directia si timpul la care s-a facut citirea
        direction = 1;
        checkTime = millis();
        
        //Serial.println("if5");
        delay(100);
      }
      if(readState[0] == 1&& prevReadState[0] == 0){    //front pe OUT -> setez directia si timpul la care s-a facut citirea
        direction = 0;
        checkTime = millis();
        
        //Serial.println("if6");
        delay(100);
      }
  } 
}

void updateLCD(){
  lcd.clear();                          
  if(peopleInside > 0){
    lcd.setCursor(0,0);               
    lcd.print("Numar persoane:");
    lcd.setCursor(0,1);
    char buff[17];
    sprintf(buff, "%d", peopleInside);
    lcd.print(buff);
    digitalWrite(LED,0);
    Serial.print("Numar persoane: ");
    Serial.println(buff);
  }
  else if(peopleInside == 0){
    lcd.setCursor(0,0);
    lcd.print("Camera goala");
    Serial.println("Camera goala");
    digitalWrite(LED,1);
  }
  else if(peopleInside < 0){
    lcd.setCursor(0,0);
    lcd.print("Oops, mai era");
    lcd.setCursor(0,1);
    lcd.print("cineva inauntru");
    Serial.println("Oops, mai era cineva inauntru");
    peopleInside = 0;
    delay(1500);    
    updateLCD();
  }
}

