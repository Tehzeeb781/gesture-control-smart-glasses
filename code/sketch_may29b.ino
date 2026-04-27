#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Sound Sensor
#define MIC_PIN 14         // KY-037 AO to GPIO14
#define BUZZER_PIN 13      // Buzzer + to GPIO13, - to GND
const int threshold = 3000;
unsigned long lastClapTime = 0;

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
TwoWire myI2C(0);  // I2C bus on custom pins
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &myI2C, -1);

// State
bool buzzerOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(MIC_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  myI2C.begin(15, 4);  // SDA = GPIO 15, SCL = GPIO 4

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  delay(1000);
  display.clearDisplay();
  Serial.println("Say 'ON'/'OFF' or clap.");
}

void loop() {
  handleClapDetection();
  handleSerialCommand();
}

void handleClapDetection() {
  int micValue = analogRead(MIC_PIN);
  Serial.println(micValue);

  if (micValue > threshold && millis() - lastClapTime > 500) {
    lastClapTime = millis();
    toggleBuzzer("Clap");
  }
}

void handleSerialCommand() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    cmd.toUpperCase();
    Serial.println("Received: [" + cmd + "]");

    if (cmd == "ON" && !buzzerOn) {
      toggleBuzzer("Voice ON");
    } else if (cmd == "OFF" && buzzerOn) {
      toggleBuzzer("Voice OFF");
    }
  }
}

void toggleBuzzer(const String& source) {
  buzzerOn = !buzzerOn;
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(buzzerOn ? "Buzzer ON" : "Buzzer OFF");
  display.println("Source: " + source);
  display.display();

  Serial.println(buzzerOn ? "Buzzer ON" : "Buzzer OFF");
  Serial.println("Triggered by: " + source);
}

