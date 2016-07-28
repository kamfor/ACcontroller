#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <DS18B20.h>
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
int16_t last, value;
ClickEncoder::Button button;
byte address[8] = {0x28, 0xB1, 0x6D, 0xA1, 0x3, 0x0, 0x0, 0x11}; //termometr adress
OneWire onewire(5); //tempSensorPin
DS18B20 sensors(&onewire);
LiquidCrystal_I2C lcd(0x27,20,4);


int flameSensorPin = A0; 
int powerRelayPin = 2; 
int resetRelayPin = 3; 
int gasRelayPin = 4;  


int flameSensorValue = 0; 
float tempSensorValue = 0;

void timerIsr() {
  encoder->service();
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW); //GND supplicant

  sensors.begin();
  sensors.request(address);
  
  encoder = new ClickEncoder(6, 7, 8); //A,B, Button

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  
  last = -1;

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");
}

void loop() {

  value += encoder->getValue();//readnig encoder
  
  if (value != last) {
    last = value;
    Serial.print("Encoder Value: ");
    Serial.println(value);
  }
  button = encoder->getButton(); //readnig button state

  flameSensorValue = analogRead(flameSensorPin); //reading flameSensor

  if (sensors.available()) //reading temperature
  {
    tempSensorValue = sensors.readTemperature(address);
    Serial.print(tempSensorValue);
    Serial.println(F(" 'C"));
    sensors.request(address);
  }

  if (button != ClickEncoder::Open) {
    switch (button) {
      case ClickEncoder::Released:
        Serial.print("Clicked");
      break; 
      case ClickEncoder::DoubleClicked:
        Serial.print("Double clicked");
      break;
    }
  }
}



