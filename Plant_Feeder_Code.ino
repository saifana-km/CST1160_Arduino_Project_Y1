#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
const int relayPin       = 5;
const int sensorSoilPin  = A0;
const int sensorWaterPin = A1;
const int red_led        = 2;

// Variables
int dampness = 0;
int waterLevel = 0;
int prevSoilState = -1;
int prevPumpState = -1;
int prevWaterPercent = -1;

// Water thresholds
const int DRY_ON_THRESHOLD  = 550;  // pump turns ON above this
const int WET_OFF_THRESHOLD = 420;  // pump turns OFF below this

// Pump state memory
bool pumpOn = false;

void setup() {

  pinMode(relayPin, OUTPUT);
  pinMode(red_led, OUTPUT);

  Serial.begin(9600);

  // LCD intro
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Soil & Water");
  lcd.setCursor(0, 1);
  lcd.print("Monitor Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  dampness = analogRead(sensorSoilPin);
  delay(10);
  waterLevel = analogRead(sensorWaterPin);
  delay(10);

  // Soil states
  bool soilWet = dampness < WET_OFF_THRESHOLD;
  bool soilDry = dampness > DRY_ON_THRESHOLD;

  int soilState = soilWet ? 0 : (soilDry ? 2 : 1);

  // Water level state
  bool tankLow  = waterLevel < 300;

  // Pump
  if (soilDry && !tankLow) {
      pumpOn = true;  
  }

  if (soilWet) {
      pumpOn = false; 
  }

  if (tankLow) {
      pumpOn = false;
  }

  // OK state
  int pumpState = pumpOn ? 1 : 0;

  // Water tank percentage
  int waterPercent = map(waterLevel, 0, 1023, 0, 100);
  waterPercent = constrain(waterPercent, 0, 100);

  // Update LCD 
  if (soilState != prevSoilState || pumpState != prevPumpState || waterPercent != prevWaterPercent) {

    lcd.clear();
    delay(3);

    lcd.setCursor(0, 0);
    lcd.print("Soil:");
    lcd.print(soilState == 0 ? "WET" : (soilState == 2 ? "DRY" : "OK "));
    delay(2);

    lcd.setCursor(0, 1);
    lcd.print("Pump:");
    lcd.print(pumpOn ? "ON " : "OFF");
    lcd.print(" ");
    lcd.print(waterPercent);
    lcd.print("%");
    delay(2);

    prevSoilState = soilState;
    prevPumpState = pumpState;
    prevWaterPercent = waterPercent;
  }

  // Relay + LED control 
  digitalWrite(relayPin, pumpOn ? LOW : HIGH);
  digitalWrite(red_led, tankLow ? HIGH : LOW);

  delay(300);
}