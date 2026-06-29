#ifndef UTILS_H
#define UTILS_H

#include "config.h"

String getTimestampString();
void updateClockLine();
void printNameSafely(int id);
int currentUserSafeIndex(int id);
void showWinner(bool showOnLCD);
void printLogsToSerial();
void printTimeToSerial();
void resetVotes();

#endif
