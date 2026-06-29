#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"

bool sdWriteAtomic(const String &tempPath, const String &finalPath, const uint8_t *data, size_t len);
String readLastPrevHash();
bool writeLastPrevHash(const String &hexhash);
uint32_t getNextBlockIndex();
bool storeVote(int party, int userId, const String &ts);
void checkChainIntegrity();

#endif
