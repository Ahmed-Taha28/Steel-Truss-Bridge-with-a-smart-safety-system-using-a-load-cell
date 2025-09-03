#include <HX711_ADC.h>
#include <EEPROM.h>

float weight = 1;
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin
HX711_ADC LoadCell(HX711_dout, HX711_sck);

int safeWeight = 2.5;
int warningWeight = 5;
int maxWeight = 7.5;
int distance = 0;
long duration = 0;
float calibrationValue = 0;

bool ON = false; 

#define redPin 11 
#define greenPin 10 
#define bluePin 9 
#define buzzerPin 8 
#define trigPin 7 
#define echoPin 6 

int calVal_eepromAdress = 0; 

void setup() { 
  Serial.begin(9600);
  pinMode(distance, OUTPUT);  
  pinMode(redPin, OUTPUT); 
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT); 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
   
  LoadCell.begin(); 
  EEPROM.get(calVal_eepromAdress, calibrationValue);
  LoadCell.setCalFactor(calibrationValue); 
} 

int t = 0; 

void loop() { 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 

  duration = pulseIn(echoPin, HIGH); 
  distance = (duration * 0.034) / 2; 
   
  Serial.println(distance); 
  delay(1000); 
 
  static boolean newDataReady = 0; 
  const int serialPrintInterval = 0;
 
  if (LoadCell.update()) newDataReady = true; 
 
  if (newDataReady) { 
    if (millis() > t + serialPrintInterval) { 
      weight = LoadCell.getData(); 
      Serial.print("Load_cell output val: "); 
      Serial.println(weight); 
      newDataReady = 0; 
      t = millis(); 
    } 
  } 
   
  if(distance < 60 || weight > 0.5) { 
    ON = true; 
  } else { 
    ON = false; 
  } 

  ON = true; 
  Serial.println(distance); 
 
  if(ON) { 
    if(weight < safeWeight) { 
      digitalWrite(bluePin, HIGH); 
      digitalWrite(redPin, LOW); 
      digitalWrite(greenPin, LOW); 
    } else if(weight < warningWeight) { 
      digitalWrite(greenPin, HIGH); 
      digitalWrite(redPin, LOW); 
      digitalWrite(bluePin, LOW); 
    } else if(weight >= maxWeight) { 
      digitalWrite(redPin, HIGH); 
      digitalWrite(greenPin, LOW); 
      digitalWrite(bluePin, LOW); 

      tone(buzzerPin, 1000); 
      delay(500); 
      noTone(buzzerPin); 
      delay(500); 
      tone(buzzerPin, 1000); 
      delay(500); 
      noTone(buzzerPin); 
      delay(3000); 
    } else { 
      digitalWrite(redPin, HIGH); 
      digitalWrite(greenPin, HIGH); 
      digitalWrite(bluePin, HIGH); 
    } 
  } 
}
