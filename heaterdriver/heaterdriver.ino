#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <OneWire.h>


/*
Sterownik podgrzewacza gazu

Mapa pinów 
a4 nieb SDA
a5 ziel SCL
gnd szar
vcc fiol

rel 1 gaz 4
rel 2 reset 3
rel 3 zasilanie 2

czujnik płomienia 
vcc czerwony 
gnd niebieski 
signal zółty A0

czujnik temperatury 
vcc czerwony 
gnd żółty 
data niebieski 5

enkoder 
2x switch 
3x ab channel 
*/

ClickEncoder *encoder;
int16_t last, control;
ClickEncoder::Button button;
OneWire ourWire(10);
DallasTemperature sensors(&ourWire);
LiquidCrystal_I2C lcd(0x38,16,2);

int menu = 0; 
int flameSensorPin = A0; 
int powerRelayPin = 4; 
int resetRelayPin = 3; 
int gasRelayPin = 2;  
int state=0; //0 - waiting 1 - heating 2 - error
bool displayed = false; 
bool inMenu = false; 

int flameSensorValue = 0; 
float tempSensorValue = 0;
float tempMin = 20.0; 
float tempMax = 35.0; 

void timerIsr() {
  encoder->service();
}

void setup() {
  pinMode(powerRelayPin, OUTPUT);
  pinMode(resetRelayPin, OUTPUT);
  pinMode(gasRelayPin, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(powerRelayPin, HIGH); 
  digitalWrite(resetRelayPin, HIGH);
  digitalWrite(gasRelayPin, HIGH);
  digitalWrite(9, LOW); //GND supplicant

  sensors.begin();
  encoder = new ClickEncoder(6, 7, 8); //A,B, Button

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  
  last = -1;

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Sterownik");
  lcd.setCursor(5,1);
  lcd.print("Boilera");
  displayed = true; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  if(!inMenu)control += encoder->getValue();//readnig encoder
  menu = control/5; 
  button = encoder->getButton();

  if (control != last) {
    last = control;
    if(control<0)control = 24; 
    if(control>24)control = 0; 
    lcd.setCursor(0,0);
    lcd.print(menu);
    displayed = false;
  }
  
  switch (menu){
    case 0: //home screen (waiting)//////////////////////////////////////////////////////////////////////////////////////////////////////////
      if (button == ClickEncoder::Clicked) {
            lcd.setCursor(5,1);
            lcd.print("OK");
      } 
      if(!displayed){
        lcd.clear(); 
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(4,0);
        lcd.print("Sterownik");
        lcd.setCursor(5,1);
        lcd.print("Boilera");
        displayed = true; 
      }    
      digitalWrite(powerRelayPin, HIGH); 
      digitalWrite(resetRelayPin, HIGH);
      digitalWrite(gasRelayPin, HIGH);

    break;

    case 1: //work (start,stop); //////////////////////////////////////////////////////////////////////////////////////////////////
      flameSensorValue = analogRead(flameSensorPin); //reading flameSensor must be less than 150;
       
      if (button == ClickEncoder::Clicked) {
        displayed = false;
        if(inMenu){
          inMenu = false; 
          delay(100); 
        }
        else{
          inMenu = true;
          lcd.setCursor(15,1);
          lcd.print("S");
          delay(100);
        }
      }

      if(inMenu){
        if(!displayed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(3,0);
        lcd.print("Grzanie");
        lcd.setCursor(0,1);
        lcd.print("Temp:");
        digitalWrite(powerRelayPin, LOW); //power module on
        delay(1000); 
        digitalWrite(resetRelayPin, LOW);//reset 
        delay(200); 
        digitalWrite(resetRelayPin, HIGH);
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("8");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("7");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("6");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("5");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("4");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("3");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("2");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("1");
        delay(1000);
        lcd.setCursor(15,0);
        lcd.print("0");
        digitalWrite(gasRelayPin, LOW);// power on gas
        delay(1000); 
        delay(1000); 
        delay(1000); 
        delay(1000); 
        delay(1000); 
        delay(1000); 
        delay(1000); 
        delay(1000);
        delay(1000); 
        delay(1000);
        delay(1000); 
        flameSensorValue = analogRead(flameSensorPin);//read flame state
        delay(100);
        flameSensorValue += analogRead(flameSensorPin);//read flame state
        delay(100);
        flameSensorValue += analogRead(flameSensorPin);//read flame state
        if(flameSensorValue>3000){ //disable all relays
          control = 0;  
        }
        state = 1; 
        inMenu = false;
        displayed = true;
        }
      }
      else{
        if(!displayed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(2,0);
        lcd.print("Start");
        lcd.setCursor(0,1);
        lcd.print("Temp:");
        displayed = true;
      }
        
      }
      if(state){
        flameSensorValue = analogRead(flameSensorPin);//read flame state
        delay(50);
        flameSensorValue += analogRead(flameSensorPin);//read flame state
        delay(50);
        flameSensorValue += analogRead(flameSensorPin);//read flame state
        if(flameSensorValue>3000){ //disable all relays
          menu = 0;
          state  = 0;   
        }
      }
      
      sensors.requestTemperatures();
      tempSensorValue = sensors.getTempCByIndex(0); 
      if(tempSensorValue > tempMax)control = 0; 
      lcd.setCursor(6,1);
      lcd.print(tempSensorValue);
    break; 

    case 2: //temp min////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (button == ClickEncoder::Clicked) {
         if(inMenu){
          inMenu = false; 
          menu = 1; 
          delay(100); 
         }
         else{
          inMenu = true;
          lcd.setCursor(15,1);
          lcd.print("S");
          delay(100);
         }
      }
               
      if(inMenu)tempMin += encoder->getValue();//readnig encoder
      if(tempMin<10)tempMin = 80; 
      if(tempMin>80)tempMin = 10; 
      if(!displayed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(3,0);
        lcd.print("Temp. min:");
        displayed = true;
      }
      lcd.setCursor(4,1);
      lcd.print(tempMin);

    break; 

    case 3: //temp max////////////////////////////////////////////////////////////////////////////////////////////////////////
      if (button == ClickEncoder::Clicked) {
         if(inMenu){
          inMenu = false; 
          menu = 1; 
          delay(100); 
         }
         else{
          inMenu = true;
          lcd.setCursor(15,1);
          lcd.print("S");
          delay(100);
         }
      }
               
      if(inMenu)tempMax += encoder->getValue();//readnig encoder
      if(tempMax<20)tempMax = 85; 
      if(tempMax>85)tempMax = 20; 
      if(!displayed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(3,0);
        lcd.print("Temp. max:");
        displayed = true;
      }
      lcd.setCursor(4,1);
      lcd.print(tempMax);
    break; 

    case 4: //flame sensor//////////////////////////////////////////////////////////////////////////////////////////////////////////
    flameSensorValue = analogRead(flameSensorPin);
    if(!displayed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(menu);
        lcd.setCursor(3,0);
        lcd.print("Plomien:");
        displayed = true;
      }
    lcd.setCursor(6,1);
    delay(100); 
    lcd.print(flameSensorValue);
    break; 
  }
}



