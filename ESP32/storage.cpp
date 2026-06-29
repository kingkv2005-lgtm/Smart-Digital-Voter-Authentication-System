#include "storage.h"
#include "crypto.h"
#include "utils.h"

bool sdWriteAtomic(const String &tempPath, const String &finalPath, const uint8_t *data, size_t len) {
  File f = SD.open(tempPath.c_str(), FILE_WRITE);
  if (!f) {
    Serial.print("SD write open fail: "); Serial.println(tempPath);
    return false;
  }
  size_t written = f.write(data, len);
  f.close();
  if (written != len) {
    Serial.println("SD write incomplete");
    if (SD.exists(tempPath.c_str())) SD.remove(tempPath.c_str());
    return false;
  }
  if (SD.exists(finalPath.c_str())) SD.remove(finalPath.c_str());
  bool ok = SD.rename(tempPath.c_str(), finalPath.c_str());
  if (!ok) {
    Serial.println("SD rename failed - fallback");
    File f2 = SD.open(finalPath.c_str(), FILE_WRITE);
    if (!f2) return false;
    f2.write(data, len);
    f2.close();
  }
  return true;
}

String readLastPrevHash() {
  if (!SD.exists("/blocks/last_prev_hash.txt")) {
    return String("0000000000000000000000000000000000000000000000000000000000000000");
  }
  File f = SD.open("/blocks/last_prev_hash.txt", FILE_READ);
  if (!f) return String("0000000000000000000000000000000000000000000000000000000000000000");
  String h = f.readStringUntil('\n');
  f.close(); h.trim();
  if (h.length() != 64) return String("0000000000000000000000000000000000000000000000000000000000000000");
  return h;
}

bool writeLastPrevHash(const String &hexhash) {
  String tmp = "/blocks/last_prev_hash_tmp.txt";
  String finalp = "/blocks/last_prev_hash.txt";
  if (SD.exists(tmp.c_str())) SD.remove(tmp.c_str());
  File f = SD.open(tmp.c_str(), FILE_WRITE);
  if (!f) return false;
  f.println(hexhash);
  f.close();
  if (SD.exists(finalp.c_str())) SD.remove(finalp.c_str());
  return SD.rename(tmp.c_str(), finalp.c_str());
}

uint32_t getNextBlockIndex() {
  uint32_t maxIdx = 0;
  File d = SD.open("/blocks");
  if (!d) return 1;
  File f = d.openNextFile();
  while (f) {
    String name = String(f.name());
    if (name.endsWith(".bin")) {
      int p1 = name.lastIndexOf('/');
      String fn = name.substring(p1 + 1);
      if (fn.startsWith("block_")) {
        int p2 = fn.indexOf('.', 6);
        String num = fn.substring(6, p2);
        uint32_t n = (uint32_t)num.toInt();
        if (n > maxIdx) maxIdx = n;
      }
    }
    f.close();
    f = d.openNextFile();
  }
  d.close();
  return maxIdx + 1;
}

bool storeVote(int party, int userId, const String &ts) {
  String nameStr = "";
  int namesCount = (int)(sizeof(names) / sizeof(names[0]));
  if (userId > 0 && userId < namesCount) nameStr = String(names[userId]);
  String meta = String(userId) + "|" + nameStr + "|" + ts;
  String voter_hash = sha256Hex(meta);

  String prev_hash = readLastPrevHash();
  uint32_t index = getNextBlockIndex();

  String block = "{\"index\":" + String(index) +
                 ",\"ts\":\"" + ts +
                 "\",\"voter_hash\":\"" + voter_hash +
                 "\",\"prev_hash\":\"" + prev_hash + "\"}";

  String block_hash = sha256Hex(block);

  String tmpPath = "/blocks/block_tmp_" + String(index) + ".bin";
  String finalPath = "/blocks/block_" + String(index) + ".bin";
  if (!sdWriteAtomic(tmpPath, finalPath, (const uint8_t*)block.c_str(), block.length())) {
    Serial.println("Failed to write block to SD");
    return false;
  }
  if (!writeLastPrevHash(block_hash)) {
    Serial.println("Failed to update last_prev_hash");
    return false;
  }

  String payloadStr = "EVM-01|" + String(userId) + "|" + String(party) + "|" + ts;
  const uint8_t *pldata = (const uint8_t*)payloadStr.c_str();
  size_t plen = payloadStr.length();

  uint8_t iv[12];
  randomBytes(iv, 12);
  uint8_t ciphertext[512];
  uint8_t tag[16];

  bool encok = aesGcmEncrypt(aes_key, pldata, plen, NULL, 0, iv, 12, ciphertext, tag, 16);
  if (!encok) {
    Serial.println("Encryption failed");
    return false;
  }

  uint32_t cipherlen = (uint32_t)plen;
  size_t totalLen = 12 + 16 + 4 + cipherlen;
  uint8_t *buf = (uint8_t*)malloc(totalLen);
  if (!buf) {
    Serial.println("malloc fail");
    return false;
  }
  size_t pos = 0;
  memcpy(buf + pos, iv, 12); pos += 12;
  memcpy(buf + pos, tag, 16); pos += 16;
  buf[pos++] = (uint8_t)((cipherlen >> 24) & 0xFF);
  buf[pos++] = (uint8_t)((cipherlen >> 16) & 0xFF);
  buf[pos++] = (uint8_t)((cipherlen >> 8) & 0xFF);
  buf[pos++] = (uint8_t)(cipherlen & 0xFF);
  memcpy(buf + pos, ciphertext, cipherlen); pos += cipherlen;

  String votePath = "/votes/vote_" + String(userId) + ".bin";
  String tmpVote = "/votes/vote_tmp_" + String(userId) + ".bin";
  if (LittleFS.exists(tmpVote.c_str())) LittleFS.remove(tmpVote.c_str());
  File vf = LittleFS.open(tmpVote.c_str(), "w");
  if (!vf) {
    Serial.println("LittleFS open fail");
    free(buf);
    return false;
  }
  size_t w = vf.write(buf, totalLen);
  vf.close();
  if (w != totalLen) {
    Serial.println("LittleFS write incomplete");
    if (LittleFS.exists(tmpVote.c_str())) LittleFS.remove(tmpVote.c_str());
    free(buf);
    return false;
  }
  if (LittleFS.exists(votePath.c_str())) LittleFS.remove(votePath.c_str());
  bool ren = LittleFS.rename(tmpVote.c_str(), votePath.c_str());
  if (!ren) {
    Serial.println("LittleFS rename failed - fallback write");
    File vf2 = LittleFS.open(votePath.c_str(), "w");
    if (!vf2) { free(buf); return false; }
    vf2.write(buf, totalLen);
    vf2.close();
  }
  free(buf);

  if (voteLogCount < (int)(sizeof(voteLog) / sizeof(voteLog[0]))) {
    voteLog[voteLogCount].party = party;
    voteLog[voteLogCount].userId = userId;
    voteLog[voteLogCount].ts = ts;
    voteLogCount++;
  }

  hasVoted[userId] = true;
  Serial.printf("Stored block %u and vote for user %d\n", index, userId);
  return true;
}

void checkChainIntegrity() {
  Serial.println("Checking chain...");
  File d = SD.open("/blocks");
  if (!d) { Serial.println("blocks dir missing"); return; }
  std::vector<uint32_t> idxs;
  File f = d.openNextFile();
  while (f) {
    String name = String(f.name());
    if (name.endsWith(".bin")) {
      int p1 = name.lastIndexOf('/');
      String fn = name.substring(p1 + 1);
      if (fn.startsWith("block_")) {
        int p2 = fn.indexOf('.', 6);
        String num = fn.substring(6, p2);
        uint32_t n = (uint32_t)num.toInt();
        idxs.push_back(n);
      }
    }
    f.close();
    f = d.openNextFile();
  }
  d.close();
  if (idxs.size() == 0) { Serial.println("No blocks found"); return; }
  std::sort(idxs.begin(), idxs.end());
  String prev = "0000000000000000000000000000000000000000000000000000000000000000";
  for (size_t i = 0; i < idxs.size(); ++i) {
    uint32_t n = idxs[i];
    String path = "/blocks/block_" + String(n) + ".bin";
    File bf = SD.open(path.c_str(), FILE_READ);
    if (!bf) { Serial.print("fail open "); Serial.println(path); return; }
    String blockBytes = "";
    while (bf.available()) blockBytes += (char)bf.read();
    bf.close();
    int p = blockBytes.indexOf("\"prev_hash\":\"");
    if (p >= 0) {
      int start = p + strlen("\"prev_hash\":\"");
      if (start + 64 <= (int)blockBytes.length()) {
        String ph = blockBytes.substring(start, start + 64);
        if (ph != prev) {
          Serial.printf("Chain break at block %u: expected %s got %s\n", n, prev.c_str(), ph.c_str());
          return;
        }
      } else { Serial.printf("Block %u malformed\n", n); return; }
    } else { Serial.printf("Block %u missing prev_hash\n", n); return; }
    String thisHash = sha256Hex(blockBytes);
    prev = thisHash;
  }
  Serial.println("Chain OK");
}
