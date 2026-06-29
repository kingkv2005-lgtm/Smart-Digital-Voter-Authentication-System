#include "voting.h"
#include "storage.h"
#include "utils.h"

const int buttonA = 14;
const int buttonB = 27;

void lockVoting() {
  accessGranted = false;
  currentUser = -1;
  lcd.clear(); lcd.print("Place Finger");
}

void handleVotingButtons() {
  if (currentUser <= 0 || currentUser >= MAX_USERS) return;

  if (digitalRead(buttonA) == LOW) {
    delay(60);
    if (digitalRead(buttonA) == LOW) {
      String ts = getTimestampString();
      bool ok = storeVote(1, currentUser, ts);
      if (ok) {
        votesA++;
        lcd.clear(); lcd.print("Voted: Party A");
        lcd.setCursor(0,1); lcd.print(ts.substring(11));
        Serial.printf("VOTE A by %s (ID %d) @ %s\n", names[currentUserSafeIndex(currentUser)], currentUser, ts.c_str());
        delay(1200);
        lockVoting();
        return;
      } else {
        lcd.clear(); lcd.print("Error! Try");
        lcd.setCursor(0,1); lcd.print("Again");
        delay(1200);
        lockVoting();
        return;
      }
    }
  }

  if (digitalRead(buttonB) == LOW) {
    delay(60);
    if (digitalRead(buttonB) == LOW) {
      String ts = getTimestampString();
      bool ok = storeVote(2, currentUser, ts);
      if (ok) {
        votesB++;
        lcd.clear(); lcd.print("Voted: Party B");
        lcd.setCursor(0,1); lcd.print(ts.substring(11));
        Serial.printf("VOTE B by %s (ID %d) @ %s\n", names[currentUserSafeIndex(currentUser)], currentUser, ts.c_str());
        delay(1200);
        lockVoting();
        return;
      } else {
        lcd.clear(); lcd.print("Error! Try");
        lcd.setCursor(0,1); lcd.print("Again");
        delay(1200);
        lockVoting();
        return;
      }
    }
  }
}
