#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int redLED = 3;
const int yellowLED = 4;
const int greenLED = 5;
const int servoPin = 6;
const int buzzer = 7;

byte authorizedUID[] = {0x56, 0x86, 0x10, 0x05}; 

void actualizarPantalla(String msg1, String msg2);
bool checkUID(byte *readUID, byte size);
void accesoConcedido();
void accesoDenegado();

void setup() {
  SPI.begin();
  rfid.PCD_Init();
  
  lcd.init();
  lcd.backlight();
  actualizarPantalla("SISTEMA LISTO:", "ACERQUE TARJETA");

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  myServo.attach(servoPin);
  myServo.write(0); 
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  digitalWrite(yellowLED, HIGH);
  
  if (checkUID(rfid.uid.uidByte, rfid.uid.size)) {
    accesoConcedido();
  } else {
    accesoDenegado();
  }

  digitalWrite(yellowLED, LOW);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  actualizarPantalla("SISTEMA LISTO:", "ACERQUE TARJETA");
}

bool checkUID(byte *readUID, byte size) {
  if (size != 4) return false;
  for (byte i = 0; i < 4; i++) {
    if (readUID[i] != authorizedUID[i]) return false;
  }
  return true;
}

void accesoConcedido() {
  actualizarPantalla("ACCESO VALIDO", "BIENVENIDO");
  digitalWrite(greenLED, HIGH);
  tone(buzzer, 1500, 200);
  myServo.write(90);
  delay(3000);
  myServo.write(0);
  digitalWrite(greenLED, LOW);
}

void accesoDenegado() {
  actualizarPantalla("ACCESO DENEGADO", "ID INVALIDO");
  digitalWrite(redLED, HIGH);
  tone(buzzer, 300, 1000);
  delay(2000);
  digitalWrite(redLED, LOW);
}

void actualizarPantalla(String msg1, String msg2) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);
  lcd.setCursor(0,1);
  lcd.print(msg2);
} 