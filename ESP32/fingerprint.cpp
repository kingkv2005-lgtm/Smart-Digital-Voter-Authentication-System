#include "fingerprint.h"
#include "utils.h"

HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void checkFingerprint() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    lcd.clear(); lcd.print("Not Eligible");
    digitalWrite(NO_PIN, HIGH);
    delay(1200); digitalWrite(NO_PIN, LOW);
    lcd.clear(); lcd.print("Place Finger");
    return;
  }
  int id = finger.fingerID;
  if (id <= 0 || id >= MAX_USERS) {
    lcd.clear(); lcd.print("Invalid ID");
    delay(900);
    lcd.clear(); lcd.print("Place Finger");
    return;
  }
  if (hasVoted[id]) {
    lcd.clear(); lcd.print("Already Voted");
    lcd.setCursor(0,1); printNameSafely(id);
    digitalWrite(NO_PIN, HIGH);
    delay(1200); digitalWrite(NO_PIN, LOW);
    lcd.clear(); lcd.print("Place Finger");
    return;
  }
  currentUser = id;
  accessGranted = true;
  lcd.clear(); lcd.print("Eligible");
  digitalWrite(LED_PIN, HIGH);
  lcd.setCursor(0,1); printNameSafely(id);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  lcd.clear(); lcd.print("Vote Now");
}
