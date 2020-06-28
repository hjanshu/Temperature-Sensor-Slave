#include <Arduino.h>
#include <Wire.h>
#include <I2C_Anything.h>
#include <LiquidCrystal.h>
#include <BigNumbers.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
BigNumbers bigNum(&lcd);

const byte MY_ADDRESS = 42;
volatile boolean haveData = false;
volatile float temperature;
volatile float maxTemperature;
volatile int tempUnit;
volatile int buzzerTime;
volatile float currTemp;
int tempA;
int tempB;
int buzzerPin = A0;
int ledPin = 11;

void receiveEvent(int);

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);
  TCCR1B = TCCR1B & 0b11111000 | 0x01;
  Wire.begin(MY_ADDRESS);
  lcd.begin(16, 2);
  bigNum.begin();
  Serial.begin(115200);
  Wire.onReceive(receiveEvent);
  lcd.setCursor(0, 0);
  lcd.print("  CAMSOL ENGG  ");
  lcd.setCursor(0, 1);
  lcd.print("   SOLUTIONS   ");
  delay(1000);
  lcd.clear();
}

void loop()
{
  if (haveData)
  {
    lcd.clear();

    digitalWrite(buzzerPin, LOW);
    tempA = floor(currTemp);
    tempB = (currTemp - tempA) * pow(10, 2);
    bigNum.displayLargeInt(tempA, 0, 3, false);
    lcd.write(".");
    lcd.print(tempB);
    lcd.write(" ");
    lcd.print((char)223);
    if (tempUnit == 0)
      lcd.print("C");
    else if (tempUnit == 1)
      lcd.print("F");
    delay(200);
    digitalWrite(buzzerPin, HIGH);
    if (temperature > maxTemperature)
    {
      Wire.write('x');
      Wire.write(buzzerTime);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   HIGH TEMP   ");
      lcd.setCursor(0, 1);
      lcd.print("    DETECTED   ");
      delay(buzzerTime * 1000);
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, LOW);
    }

    delay(2000);
    haveData = false;
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("  CAMSOL ENGG  ");
    lcd.setCursor(0, 1);
    lcd.print("   SOLUTIONS   ");
  }

} // end of loop

// called by interrupt service routine when incoming data arrives
void receiveEvent(int howMany)
{
  if (howMany >= (sizeof temperature) + (sizeof maxTemperature) + (sizeof tempUnit) + (sizeof buzzerTime) + (sizeof currTemp))
  {
    I2C_readAnything(temperature);
    I2C_readAnything(currTemp);
    I2C_readAnything(tempUnit);
    I2C_readAnything(maxTemperature);
    I2C_readAnything(buzzerTime);
    haveData = true;
  } // end if have enough data
} // end of receiveEvent