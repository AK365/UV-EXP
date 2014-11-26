/*
Platinenbelichter V0.1 (UV Exposure Unit)

Acts as a timer for PCB exposure. The leds are controlled by a FET.
Furthermore a 16x2 Character LCD and three push buttons are connected.

The circuit:
* LCD RS pin to digital pin 12
* LCD Enable pin to digital pin 11
* LCD DO4 pin to digital pin 5
* LCD DO5 pin to digital pin 4
* LCD DO6 pin to digital pin 3
* LCD DO7 pin to digital pin 2
* LCD R/W pin to ground
*Push button to digital pin8 / 9 / 10

Copyright: Alexander Kroth 2014

*/

//include LCD library
#include <LiquidCrystal.h>

//include Debounce library
#include <Bounce2.h>

//LCD initilization
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Button pin definition
const int MinButtonP = 8;
const int SecButtonP = 9;
const int StartButtonP = 10;

//FET pin definition
const int FetP = 13;

//Timer variable
unsigned long current_time;
unsigned long termination_time;

int Min_Set = 0;
int Sec_Set = 0;
int Min_Cur = 0;
int Sec_Cur = 0;

boolean running;

//Initialize button objects
Bounce MinButton = Bounce();
Bounce SecButton = Bounce();
Bounce StartButton = Bounce();

void setup() {
  
  //Pin Setup
  
  pinMode(MinButtonP, INPUT);
  digitalWrite(MinButtonP, HIGH);
  
  pinMode(SecButtonP, INPUT);
  digitalWrite(SecButtonP, HIGH);
  
  pinMode(StartButtonP, INPUT);
  digitalWrite(StartButtonP, HIGH);
  
  pinMode(FetP, OUTPUT);
  
  //Button debouncing using Bounce 2 from thomasfredericks
  
  MinButton.attach(MinButtonP);
  MinButton.interval(100);
  
  
  SecButton.attach(SecButtonP);
  SecButton.interval(100);
  
  
  StartButton.attach(StartButtonP);
  StartButton.interval(100);
  
  
  
  //Row and columns setup of LCD
  lcd.begin(16,2);
  //Startup Screen
  lcd.print("Platinen-");
  lcd.setCursor(0,1);
  lcd.print("Belichter");
  lcd.setCursor(0,0);
  delay(2000);
  
  //Prepare Display
  lcd.print("Dauer           ");
  lcd.setCursor(0,1);
  lcd.print("Min. 00 Sec. 00 ");
  
}
  
void loop() {
  
  //Button Updates
  MinButton.update();
  SecButton.update();
  StartButton.update();
  
  running = false;
  
  
  
  //Info Screen
  if(MinButton.read() == false && SecButton.read() == false && StartButton.read() == false) {
    lcd.setCursor(0,0);
    lcd.print("Hackaday.io     ");
    lcd.setCursor(0,1);
    lcd.print("Alexander Kroth ");
    delay(2000);
    
    lcd.setCursor(0,0);
    lcd.print("Dauer           ");
    lcd.setCursor(0,1);
    lcd.print("Min. 00 Sec. 00 ");
    
    Min_Set--;
    Sec_Set--;
    running = true;
    
    if(Min_Set < 10) {
      
      lcd.setCursor(6,1);
      lcd.print(Min_Set);
    }
    else {
      lcd.setCursor(5,1);
      lcd.print(Min_Set);
    }
  
    if(Sec_Set < 10) {
      
      lcd.setCursor(14,1);
      lcd.print(Sec_Set);
    }
    else {
      lcd.setCursor(13,1);
      lcd.print(Sec_Set);
    }
  }
  
  
  //Reset time
  if(MinButton.read() == false && SecButton.read() == false) {

    
    Min_Set = -1;
    Sec_Set = -1;
    lcd.setCursor(0,1);
    lcd.print("Min. 00 Sec. 00 ");

  } 
  
  
  //Increment minutes
  //Check wether exposure is running and rising edge was detected
  if(MinButton.rose() == true) {
    //Increase minutes to expose
    Min_Set++;
    
    //Maximum of 99 minutes, seriously, no one needs more?
    if(Min_Set > 99) {
      Min_Set = 99;
    }
    
    //Move cursor to position the numbers
    if(Min_Set < 10) {
      
      lcd.setCursor(6,1);
      lcd.print(Min_Set);
    }
    else {
      lcd.setCursor(5,1);
      lcd.print(Min_Set);
    }
  }
  
  
  
  
  //Increment seconds
  //Check wether exposure is running and rising edge was detected
  if(SecButton.rose() == true) {
    Sec_Set++;
    
    //Seconds overflow and Cursor correction
    if(Sec_Set > 59) {
      Sec_Set = 0;
      lcd.setCursor(13,1);
      lcd.print("00");
    }
    
    //Move cursor to position the numbers
    if(Sec_Set < 10) {
      
      lcd.setCursor(14,1);
      lcd.print(Sec_Set);
    }
    else {
      lcd.setCursor(13,1);
      lcd.print(Sec_Set);
    }
  }
  
  
  //Start exposure
  if(running == false && StartButton.rose() == true) {
    //Calculate end time of exposure
    termination_time = millis()/1000 + Min_Set * 60 + Sec_Set;
    
    //Inform user of exposure
    lcd.setCursor(0,0);
    lcd.print("Belichtung      ");
    lcd.setCursor(0,1);
    lcd.print("00:00 / 00:00    ");
    if(Min_Set < 10) {
      
      lcd.setCursor(9,1);
      lcd.print(Min_Set);
    }
    else {
      lcd.setCursor(8,1);
      lcd.print(Min_Set);
    }
  
    if(Sec_Set < 10) {
      
      lcd.setCursor(12,1);
      lcd.print(Sec_Set);
    }
    else {
      lcd.setCursor(11,1);
      lcd.print(Sec_Set);
    }
    
    
    
    
    //Switch the leds on
    digitalWrite(FetP, HIGH);
    
    //Wait for PCB to be exposed
    while( millis()/1000 < termination_time) {
      
      //Still read the StartButton to get free
      StartButton.update();
      
      //Abort exposure
      if(StartButton.rose() == true) {
        break;
      }
      
      //Calculate time to display
      Min_Cur = int(long(termination_time-(millis()/1000))/60);
      Sec_Cur = long(termination_time-millis()/1000) - (int(long(termination_time-(millis()/1000))/60)*60);
      
      
      //Update display
      if(Min_Cur < 10) {
        lcd.setCursor(0,1);
        lcd.print(0);
        lcd.setCursor(1,1);
        lcd.print(Min_Cur);
      }
      else {
        lcd.setCursor(0,1);
        lcd.print(Min_Cur);
      }
  
      if(Sec_Cur < 10) {
        lcd.setCursor(3,1);
        lcd.print(0);
        lcd.setCursor(4,1);
        lcd.print(Sec_Cur);
      }
      else {
        lcd.setCursor(3,1);
        lcd.print(Sec_Cur);
      }
    }
    digitalWrite(FetP, LOW);
    
    //Restore former display
    lcd.setCursor(0,0);
    lcd.print("Dauer           ");
    lcd.setCursor(0,1);
    lcd.print("Min. 00 Sec. 00 ");
    
    if(Min_Set < 10) {
      
      lcd.setCursor(6,1);
      lcd.print(Min_Set);
    }
    else {
      lcd.setCursor(5,1);
      lcd.print(Min_Set);
    }
  
    if(Sec_Set < 10) {
      
      lcd.setCursor(14,1);
      lcd.print(Sec_Set);
    }
    else {
      lcd.setCursor(13,1);
      lcd.print(Sec_Set);
    }
  }


  
}
  

