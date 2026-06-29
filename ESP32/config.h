#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <LittleFS.h>
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"
#include <esp_system.h>
#include <vector>
#include <algorithm>

#define NO_PIN 4
#define LED_PIN 2
#define RESULT_BUTTON 15

#define SDA_PIN 21
#define SCL_PIN 22

#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS   5

#define MAX_USERS 200

extern LiquidCrystal_I2C lcd;
extern RTC_DS3231 rtc;
extern HardwareSerial mySerial;
extern Adafruit_Fingerprint finger;

extern const int buttonA;
extern const int buttonB;

extern volatile int votesA;
extern volatile int votesB;

extern bool accessGranted;
extern int currentUser;
extern bool hasVoted[MAX_USERS];

extern const char* names[];

extern const uint8_t aes_key[32];

struct VoteLogEntry {
  int party;
  int userId;
  String ts;
};

extern VoteLogEntry voteLog[400];
extern int voteLogCount;

#endif
