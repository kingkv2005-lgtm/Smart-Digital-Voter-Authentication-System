#include "config.h"
#include "utils.h"
#include "fingerprint.h"
#include "voting.h"
#include "storage.h"
#include "crypto.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RESULT_BUTTON, INPUT_PULLUP);
  pinMode(NO_PIN, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(10);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  if (!rtc.begin()) {
    lcd.clear(); lcd.print("RTC ERROR");
    Serial.println("RTC not found");
    while (1) delay(100);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power — setting to compile time");
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  Serial.print("RTC current time: ");
  Serial.println(getTimestampString());
  lcd.clear();
  lcd.print("RTC:");
  lcd.setCursor(0,1);
  lcd.print(getTimestampString().substring(11));
  delay(1200);

  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  delay(100);
  if (!finger.verifyPassword()) {
    lcd.clear(); lcd.print("Sensor ERROR");
    Serial.println("Fingerprint sensor not found");
    while (1) delay(100);
  }

  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed! Formatting...");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("LittleFS still failed!");
      lcd.clear(); lcd.print("FS ERROR");
      while (1) delay(100);
    }
  }

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println("SD mount failed!");
    lcd.clear(); lcd.print("SD ERROR");
    while (1) delay(100);
  }

  if (!SD.exists("/blocks")) SD.mkdir("/blocks");
  if (!LittleFS.exists("/votes")) LittleFS.mkdir("/votes");

  delay(300);
  lcd.clear(); lcd.print("Place Finger");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("winner")) showWinner(true);
    else if (cmd.equalsIgnoreCase("reset")) resetVotes();
    else if (cmd.equalsIgnoreCase("showlogs")) printLogsToSerial();
    else if (cmd.equalsIgnoreCase("time")) printTimeToSerial();
    else if (cmd.equalsIgnoreCase("checkchain")) checkChainIntegrity();
    else if (cmd.startsWith("settime")) {
      int y, m, d, h, mm, s;
      if (sscanf(cmd.c_str(), "settime %d-%d-%d %d:%d:%d", &y, &m, &d, &h, &mm, &s) == 6) {
        rtc.adjust(DateTime(y, m, d, h, mm, s));
        Serial.println("RTC UPDATED SUCCESSFULLY");
        Serial.print("New time: ");
        Serial.println(getTimestampString());
      } else {
        Serial.println("Invalid format! Use:");
        Serial.println("settime YYYY-MM-DD HH:MM:SS");
      }
    }
  }

  if (digitalRead(RESULT_BUTTON) == LOW) {
    delay(80);
    if (digitalRead(RESULT_BUTTON) == LOW) showWinner(false);
  }

  if (!accessGranted) {
    checkFingerprint();
    updateClockLine();
    delay(50);
    return;
  }

  updateClockLine();
  handleVotingButtons();
  delay(20);
}
