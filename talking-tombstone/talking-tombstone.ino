/**
 * Arduino Nano Talking Tombstone Project by Deron Eriksson 
 * 
 * This project uses an Arduino Nano to read whether or not a PIR motion
 * sensor detects motion. If motion is detected, a sound module (like YX5300)
 * plays a random sound file from a micro SD card. An RGB LED on a KY-016
 * module is used to display status information. While the PIR sensor
 * initializes, it flashes yellow. When it's ready to detect motion, it
 * becomes green. If motion is detected, it turns blue. If a problem occurs,
 * it turns red.
 */
#include "SerialMP3Player.h"

// Nano pins
#define RGB_LED_R 2 // KY-016 R
#define RGB_LED_G 3 // KY-016 G
#define RGB_LED_B 4 // KY-016 B
#define SOUND_RX 5 // to TX of sound module
#define SOUND_TX 6 // to RX of sound module
#define PIR_MOTION_DETECTOR 7 // signal from motion detector

const long BAUD_RATE = 9600;
const int RESEND_INTERVAL = 3000; // milliseconds to resend command if answer not returned
const int PIR_INIT_TIME = 60; // seconds to init PIR motion detector

SerialMP3Player soundModule(SOUND_RX, SOUND_TX);
uint8_t smAnswer[10] = {0}; // the answer from sound module command
int fileCount = 0; // number of files on SD card

void setup() {
  pinMode(RGB_LED_R, OUTPUT);
  pinMode(RGB_LED_G, OUTPUT);
  pinMode(RGB_LED_B, OUTPUT);
  pinMode(PIR_MOTION_DETECTOR, INPUT);

  Serial.begin(BAUD_RATE);
  soundModule.showDebug(1); // show debug messages
  soundModule.begin(BAUD_RATE);
  soundModule.sendCommand(CMD_SEL_DEV, 0, 2); // select the SD card
  delay(500);
  
  Serial.println("Talking Tombstone Project");
  Serial.println("Play random mp3 sound files in response to motion detection");
  Serial.println();

  fileCount = getFileCount();
  Serial.println("File count: " + (String) fileCount);

  initPirMotionDetector();

}

void loop() {
//  ledColorTest();
//  pirMotionTest();
  playSoundIfMotion();
}

/**
 * If motion sensor picks up movement, set RGB LED to blue and play random file;
 * otherwise, set RGB LED to green.
 */
void playSoundIfMotion() {
  int motionSignal = digitalRead(PIR_MOTION_DETECTOR);
//  Serial.println("PIR Motion Detector: " + (String) motionSignal);
  if (motionSignal == HIGH) { // Motion detected
    setLedToBlue();
    Serial.println("Motion detected, so requesting to play file");
    playRandomFile();
    delay(3000);
  } else {
    setLedToGreen();
//    Serial.println("No motion detected at " + (String) millis());
  }
  delay(100);  
}

/**
 * Test PIR motion sensor. Set RGB LED to blue if motion detected; otherwise, green.
 */
void pirMotionTest() {
  int motionSignal = digitalRead(PIR_MOTION_DETECTOR);
  Serial.println("PIR Motion Detector: " + (String) motionSignal);
  if (motionSignal == HIGH) { // Motion detected
    setLedToBlue();
  } else {
    setLedToGreen();
  }
  delay(100);
}

/**
 * Test RGB LED by flashing different colors.
 */
void ledColorTest() {
  setLedToRed();
  delay(500);
  setLedToGreen();
  delay(500);
  setLedToBlue();
  delay(500);
  setLedToOff();
  delay(500);
  setLedToWhite();
  delay(500);
  setLedToYellow();
  delay(500);
}

/**
 * Set RGB LED to red.
 */
void setLedToRed() {
  digitalWrite(RGB_LED_R, HIGH);
  digitalWrite(RGB_LED_G, LOW);
  digitalWrite(RGB_LED_B, LOW);
}

/**
 * Set RGB LED to green.
 */
void setLedToGreen() {
  digitalWrite(RGB_LED_R, LOW);
  digitalWrite(RGB_LED_G, HIGH);
  digitalWrite(RGB_LED_B, LOW);
}

/**
 * Set RGB LED to blue.
 */
void setLedToBlue() {
  digitalWrite(RGB_LED_R, LOW);
  digitalWrite(RGB_LED_G, LOW);
  digitalWrite(RGB_LED_B, HIGH);
}

/**
 * Set RGB LED to white.
 */
void setLedToWhite() {
  digitalWrite(RGB_LED_R, HIGH);
  digitalWrite(RGB_LED_G, HIGH);
  digitalWrite(RGB_LED_B, HIGH);
}

/**
 * Set RGB LED to yellow.
 */
void setLedToYellow() {
  digitalWrite(RGB_LED_R, HIGH);
  digitalWrite(RGB_LED_G, HIGH);
  digitalWrite(RGB_LED_B, LOW);
}

/**
 * Set RGB LED to off.
 */
void setLedToOff() {
  digitalWrite(RGB_LED_R, LOW);
  digitalWrite(RGB_LED_G, LOW);
  digitalWrite(RGB_LED_B, LOW);
}

/**
 * PIR sensor needs time to initialize.
 */
void initPirMotionDetector() {
  Serial.println("Initializing PIR sensor for " + (String) PIR_INIT_TIME + " seconds");
  for (int i=0; i<PIR_INIT_TIME; i++) {
    Serial.println("Initializing PIR sensor: " + (String) (PIR_INIT_TIME - i));
    if (i % 2 == 0) { // blink yellow on and off while PIR initializes
      setLedToYellow();
    } else {
      setLedToOff();
    }
    delay(1000);
  }
  Serial.println("PIR sensor initialized for " + (String) PIR_INIT_TIME + " seconds");
  setLedToGreen(); // green when PIR done initializing
}

/**
 * Return number of sound files on the SD card.
 */
int getFileCount() {
  Serial.println("Requesting file count");
  soundModule.sendCommand(CMD_QUERY_TOT_TRACKS);
  // delay(100);
  int fc = getFileCountFromSMAnswer();
  unsigned long start = millis();
  while (fc == -1) {
    if ((millis() - start) >= RESEND_INTERVAL) {
      Serial.println("Answer not received, so resending request for file count");
      soundModule.sendCommand(CMD_QUERY_TOT_TRACKS);
      setLedToRed();
      // delay(100);
    }
    fc = getFileCountFromSMAnswer();
    // Serial.println("Is it hanging? " + (String) millis());
    // delay(100);
  }
  setLedToGreen();
  return fc;
}

/**
 * Return file count based on sound module answer.
 */
int getFileCountFromSMAnswer() {
  readSMAnswer();
  if ((smAnswer[0] == 0x7E) && (smAnswer[3] == 0x48) && (smAnswer[9] == 0xEF)) {
    return smAnswer[6];
  } else {
    return -1;
  }
}

/**
 * Read sound module answer into smAnswer.
 */
void readSMAnswer() {
  smAnswer[10] = {0};
  uint8_t i = 0;
  while (soundModule.available() && (i < 10)) {
    uint8_t b = soundModule.read();
    smAnswer[i] = b;
    i++;
  }
  return;
}

/**
 * Return true if the sound module is playing a file; otherwise return false
 */
bool isPlaying() {
  soundModule.sendCommand(CMD_QUERY_STATUS);
  // delay(100);
  int status = isPlayingFromSMAnswer();
  unsigned long start = millis();
  while (status == -1) {
    if ((millis() - start) >= RESEND_INTERVAL) {
      Serial.println("Answer not received, so resending request for status");
      soundModule.sendCommand(CMD_QUERY_STATUS);
      // delay(100);
    }
    status = isPlayingFromSMAnswer();
    // Serial.println("Is it hanging? " + (String) millis());
    // delay(100);
  }
  if (status == 1) {
    return true;
  } else {
    return false;
  }
}

/**
 * Return 1 if sound module playing a file, or other numbers if not playing.
 */
int isPlayingFromSMAnswer() {
  readSMAnswer();
  if ((smAnswer[0] == 0x7E) && (smAnswer[3] == 0x42) && (smAnswer[9] == 0xEF)) {
    if (smAnswer[6] == 0x00) {
      return 0; // stopped
    } else if (smAnswer[6] == 0x01) {
      return 1; // playing
    } else if (smAnswer[6] == 0x02) {
      return 2; // paused
    } else {
      return -2; // unknown
    }
  } else {
    return -1;
  }
}

/**
 * Play a random file.
 */
void playRandomFile() {
  randomSeed(millis());
  int whichFile = random(1, fileCount + 1);
  // if (!isPlaying()) {
    Serial.println("Sending request to play file #" + (String) whichFile);
    soundModule.play(whichFile);
  // } else {
    // Serial.println("Currently playing");
  // }
}
