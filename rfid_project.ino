

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define SS_PIN 10
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

int PIN_RED = 4;
int PIN_GREEN = 3;
int PIN_BLUE = 2;

int buzzerPin = 9;

void turnOnLights(String color = "") {
  if (color == "red") {
    setColor(230, 0, 0);
  } else if (color == "green") {
    setColor(102, 255, 51);
  } else if (color == "blue") {
    setColor(51, 51, 255);
  } else {
    setColor(51, 51, 255);
  }
}

void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  Serial.begin(9600);  // Initiate a serial communication
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  tone(buzzerPin, 1000, 1000);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.display();
  delay(1000);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Set text size, color, and position
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("idling");
  display.display();
  turnOnLights();
}

void loop() {
  display.clearDisplay();
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  display.display();

  int scan = scan_();
  display_text(scan);
  delay(1000);
}

int scan_() {
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  char targetValue[100];  // Character array to hold the substring

  // Convert the substring to a character array
  content.substring(1).toCharArray(targetValue, sizeof(targetValue));

  char* array[] = { "93 E1 F1 4B", "E2 29 CD 1D1", "X1 Y2 Z3" };
  bool found = false;
  Serial.println();
  for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
    if (strcmp(array[i], targetValue) == 0) {
      found = true;
      break;
    }
  }

  if (found) {
    tone(buzzerPin, 3000, 1000);
    turnOnLights("green");
    return 1;
  } else {
    tone(buzzerPin, 3500, 1000);
    turnOnLights("red");
    return 0;
  }
  noTone(buzzerPin);
}

void display_text(int status) {
  String text = F("Access ");
  int textWidth = text.length() * 6;
  int y = (SCREEN_HEIGHT - 2 * 8) / 2;

  text += status ? "Granted" : "Denied";

  display.setCursor(10, y);
  display.println(text);
  display.display();

  delay(1000);
  display.setCursor(10, y);
  Serial.print("idling");
  display.clearDisplay();
  display.println("Idling...");
  turnOnLights();
  display.display();

  display.clearDisplay();
}

void setColor(int R, int G, int B) {
  analogWrite(PIN_RED, R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE, B);
}