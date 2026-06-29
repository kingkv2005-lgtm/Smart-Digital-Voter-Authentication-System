# Smart Digital Voter Authentication System

A secure IoT-based digital voting platform that integrates QR Code verification, Fingerprint Authentication, Firebase Cloud Firestore, and ESP32 to provide secure voter authentication while preventing unauthorized voting.

---

##  Overview

The Smart Digital Voter Authentication System is designed to modernize the traditional voting process by combining embedded systems, web technologies, cloud databases, and secure authentication techniques.

The system verifies each voter through QR code authentication and fingerprint verification before allowing a vote to be cast. Voter information is securely managed using Firebase Cloud Firestore, while AES encryption and SHA-256 hashing are used to improve data security and integrity.

---

##  Features

-  Fingerprint Authentication
-  QR Code Based Voter Verification
-  Firebase Cloud Firestore Integration
-  Dedicated Webpage for Each Registered Voter
-  AES Encryption
-  SHA-256 Hashing
-  ESP32 Based Embedded System
-  Web Interface using HTML, CSS and JavaScript
-  Secure Authentication Workflow

---

## System Architecture

```

User
│
▼
QR Code Scan
│
▼
Web Portal
│
▼
Firebase Cloud Firestore
│
▼
Fingerprint Authentication
│
▼
ESP32
│
▼
Vote Authentication
│
▼
AES Encryption
│
▼
SHA-256 Integrity Verification




## Workflow

1. User scans the QR Code.
2. Dedicated voter webpage opens.
3. Voter information is retrieved from Firebase Cloud Firestore.
4. Fingerprint authentication is performed.
5. Authentication status is verified.
6. Authorized voter casts the vote.
7. Vote data is securely processed.



##  Technologies Used

###  Programming Languages

- JavaScript
- HTML5
- CSS3
- C++ (Arduino)

### Cloud

- Firebase Cloud Firestore

### Hardware

- ESP32
- Fingerprint Sensor
- QR Code Scanner(shoud be use but we have used mobile phone for our convinence)

### Security

- AES Encryption
- SHA-256 Hashing

### Tools

- Arduino IDE
- Git
- GitHub


  Future Enhancements

- Face Recognition Integration
- OTP Verification
- Blockchain-based Vote Storage
- Election Dashboard
- Real-time Analytics
- Multi-language Support

##  Author

**Vishal K**
**Vishwa R**
**Jemmy Angel K**



LinkedIn:
https://linkedin.com/in/vishal-k-a23ba9255
