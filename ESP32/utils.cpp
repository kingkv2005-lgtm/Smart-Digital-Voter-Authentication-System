#include "utils.h"
#include "storage.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

const char* names[] = {
  "",
  "Vishwa R",
  "Vishal K",
  "Jemmy Angel K",
  "Sivasakthi",
  "Jhalam"
};

volatile int votesA = 0;
volatile int votesB = 0;

bool accessGranted = false;
int currentUser = -1;
bool hasVoted[MAX_USERS] = { false };

VoteLogEntry voteLog[400];
int voteLogCount = 0;

String getTimestampString() {
  DateTime now = rtc.now();
  char buf[25];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());
  return String(buf);
}

void updateClockLine() {
  DateTime now = rtc.now();
  char tbuf[9];
  sprintf(tbuf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 1);
  lcd.print(tbuf);
  for (int i = 8; i < 16; i++) lcd.print(' ');
}

void printNameSafely(int id) {
  int namesCount = (int)(sizeof(names) / sizeof(names[0]));
  if (id > 0 && id < namesCount) lcd.print(names[id]);
  else {
    lcd.print("ID:"); lcd.print(id);
  }
}

int currentUserSafeIndex(int id) {
  int namesCount = (int)(sizeof(names) / sizeof(names[0]));
  if (id > 0 && id < namesCount) return id;
  return 0;
}

void showWinner(bool showOnLCD) {
  String result;
  if (votesA > votesB) result = "WINNER = PARTY A";
  else if (votesB > votesA) result = "WINNER = PARTY B";
  else result = "RESULT = TIE";

  Serial.println(result);
  Serial.print("Total A = "); Serial.println(votesA);
  Serial.print("Total B = "); Serial.println(votesB);
  printLogsToSerial();

  if (showOnLCD) {
    lcd.clear(); lcd.print("RESULT:");
    lcd.setCursor(0,1);
    if (votesA > votesB) lcd.print("Party A ");
    else if (votesB > votesA) lcd.print("Party B ");
    else lcd.print("TIE");
    delay(3000);
    lcd.clear(); lcd.print("Place Finger");
  }
}

void printLogsToSerial() {
  if (voteLogCount == 0) {
    Serial.println("No votes logged yet.");
    return;
  }
  for (int i = 0; i < voteLogCount; i++) {
    Serial.print(i + 1); Serial.print(": Party ");
    Serial.print(voteLog[i].party == 1 ? "A" : "B");
    Serial.print(" | NAME ");
    int uid = voteLog[i].userId;
    int namesCount = (int)(sizeof(names) / sizeof(names[0]));
    if (uid > 0 && uid < namesCount) Serial.print(names[uid]);
    else Serial.print(uid);
    Serial.print(" | ");
    Serial.println(voteLog[i].ts);
  }
}

void printTimeToSerial() {
  DateTime now = rtc.now();
  Serial.printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                now.year(), now.month(), now.day(),
                now.hour(), now.minute(), now.second());
}

void resetVotes() {
  votesA = 0;
  votesB = 0;
  voteLogCount = 0;
  for (int i = 0; i < MAX_USERS; i++) hasVoted[i] = false;

  Serial.println("Votes and records reset.");
  lcd.clear(); lcd.print("Reset Done");
  delay(1000);
  lcd.clear(); lcd.print("Place Finger");
}
