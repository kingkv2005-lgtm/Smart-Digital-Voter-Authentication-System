# Smart Voting System — ESP32

## Overview

An ESP32-based biometric voting machine using fingerprint authentication, AES-GCM encrypted vote storage, an SD-card blockchain ledger, RTC timestamping, and an LCD user interface.

## Folder Structure

```
Smart_Voting_System/
├── Smart_Voting_System.ino   Main sketch: setup() and loop() only
├── config.h                  Pin definitions, constants, extern declarations, shared structs
├── fingerprint.h / .cpp      Fingerprint sensor initialization and identity verification
├── voting.h / .cpp           Button handling, vote casting, voting state management
├── crypto.h / .cpp           AES-GCM encryption, SHA-256 hashing, random byte generation
├── storage.h / .cpp          SD card block chain, LittleFS vote storage, chain integrity check
├── utils.h / .cpp            Timestamp, LCD helpers, serial logging, winner display, reset
└── README.md                 This file
```

## Module Responsibilities

### `Smart_Voting_System.ino`
Entry point. Contains `setup()` and `loop()`. Handles serial admin commands and delegates to other modules.

### `config.h`
Central configuration header. Defines all hardware pins, constants, `extern` declarations for shared global variables, the `VoteLogEntry` struct, and includes all required libraries.

### `fingerprint.h / .cpp`
Manages the Adafruit fingerprint sensor over UART2. Implements `checkFingerprint()` which scans, matches, checks eligibility, and grants voting access.

### `voting.h / .cpp`
Implements `handleVotingButtons()` and `lockVoting()`. Reads physical button state for Party A and Party B and calls `storeVote()` on a valid press.

### `crypto.h / .cpp`
Implements cryptographic primitives:
- `toHexLower()` — binary to hex string
- `sha256Hex()` — SHA-256 digest via mbedtls
- `randomBytes()` — secure random via `esp_random()`
- `aesGcmEncrypt()` — AES-256-GCM authenticated encryption via mbedtls

Also defines the `aes_key` constant.

### `storage.h / .cpp`
Implements all persistent storage:
- `sdWriteAtomic()` — atomic write with temp-then-rename on SD
- `readLastPrevHash()` / `writeLastPrevHash()` — blockchain chain pointer
- `getNextBlockIndex()` — scans SD for the next block number
- `storeVote()` — full vote storage pipeline (hash → block → encrypt → LittleFS)
- `checkChainIntegrity()` — validates the entire SD block chain

### `utils.h / .cpp`
Shared utilities and global object definitions:
- `getTimestampString()` — formats RTC time as `YYYY-MM-DD HH:MM:SS`
- `updateClockLine()` — writes current time to LCD row 1
- `printNameSafely()` — prints voter name to LCD by ID
- `currentUserSafeIndex()` — bounds-checked name array lookup
- `showWinner()` — displays and prints election result
- `printLogsToSerial()` — dumps vote log to serial
- `printTimeToSerial()` — prints RTC time to serial
- `resetVotes()` — clears all vote state

Also defines all shared globals: `lcd`, `rtc`, `names[]`, `votesA`, `votesB`, `accessGranted`, `currentUser`, `hasVoted[]`, `voteLog[]`, `voteLogCount`.

## Required Libraries

| Library | Purpose |
|---|---|
| Adafruit Fingerprint Sensor Library | Fingerprint sensor |
| LiquidCrystal I2C | LCD display |
| RTClib | DS3231 RTC |
| SD (built-in) | SD card |
| LittleFS (built-in ESP32) | Internal flash FS |
| mbedtls (built-in ESP32) | AES-GCM, SHA-256 |

## Admin Serial Commands

| Command | Action |
|---|---|
| `winner` | Show election result on LCD and serial |
| `reset` | Clear all votes and state |
| `showlogs` | Print vote log to serial |
| `time` | Print current RTC time |
| `checkchain` | Validate SD blockchain integrity |
| `settime YYYY-MM-DD HH:MM:SS` | Set RTC to specified date/time |

## Hardware Connections

| Component | ESP32 Pins |
|---|---|
| LCD (I2C) | SDA=21, SCL=22 |
| Fingerprint (UART2) | RX=16, TX=17 |
| SD Card (VSPI) | SCK=18, MISO=19, MOSI=23, CS=5 |
| LED | GPIO 2 |
| NO Pin | GPIO 4 |
| Result Button | GPIO 15 |
| Button A | GPIO 14 |
| Button B | GPIO 27 |
