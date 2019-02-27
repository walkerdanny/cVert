/* cVert software by Danny Walker, 2019
 *  
 *  Designed to run on Arduino Pro Micro
 *  
 *  See github.com/walkerdanny/cVert for more info
 *  
 *  CC BY-SA 3.0
 */


/*TODO:
  - User save of settings in 4 (?) preset slots
  - Find a way of testing Serial MIDI properly
  - Find ways of reducing screen flicker - update less frequently?
  - Try and correct for tempo fluctuations of sequencer. Maybe do maths for next note immediately after the previous note is played, and then just send it to MIDI using the interrupt.
  - Remove a lot of the debug serial messages
  - Implement control change messages? Separate mode? Is there enough memory?
  - Add in more scales
  - Find a less bodgey way of restricting the notes to 0-127 while still maintaining the randomness

*/
#include <FlexiTimer2.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <TimerOne.h>
#include <EEPROM.h>

#define LCD_COLS 20
#define LCD_ROWS 4

hd44780_I2Cexp display; // declare object: auto locate & config exapander chip


// MIDI Globals
uint8_t modeNumber = 0;
String baseNotes [] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
uint8_t rootNoteNumber = 57;
String rootNoteStr = "A4";
String scaleTypesString [] = {"None", "Major", "Minor", "Blues", "Chrom", "Dorian", "Har-Min", "Mel-Min"};
uint8_t scales [8][12] = {
  {0, 0, 0, 0, 0, 0, 0,  0,   0,   0,   0,   0},
  {0, 2, 4, 5, 7, 9, 11, 100, 100, 100, 100, 100},
  {0, 2, 3, 5, 7, 8, 10, 100, 100, 100, 100, 100},
  {0, 2, 3, 4, 5, 7, 9,  10,  11,  100, 100, 100},
  {0, 1, 2, 3, 4, 5, 6,  7,   8,   9,   10,  11},
  {0, 2, 3, 5, 7, 9, 10, 100, 100, 100, 100, 100},
  {0, 2, 3, 5, 7, 8, 11, 100, 100, 100, 100, 100},
  {0, 2, 3, 5, 7, 8, 9,  10,  11,  100, 100, 100}
};
byte seqNotes [32] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
byte seqVels [32] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
byte seqDivs [6] = {1, 2, 4, 8, 16, 32};

uint8_t scaleLengths [] = {1, 7, 7, 9, 12, 7, 7, 9};
uint8_t scaleTypeIndex = 1;
uint8_t octRange = 1;
uint8_t minVel = 0;
uint8_t maxVel = 127;
int noteLength  = 1000;
uint8_t seqLengths [] = {4, 8, 16, 32};
uint8_t seqLengthIndex;
uint8_t seqTempo = 120;
uint8_t seqIndex = 0;
uint8_t seqFillIndex = 0;
uint8_t seqDivIndex = 2;
byte fadeAmount = 0;

// Menu globals
byte menuStatus = 0;
int8_t seqLengthOffset = 0;
int8_t seqTempoOffset = 0;
int8_t seqDivOffset = 0;
int8_t modeOffset = 0;
int8_t selectedLine = 0;
int8_t menuRootOffset = 0;
int8_t scaleMenuOffset = 0;
int8_t rootNoteOffset = 0;
int8_t rangeOffset = 0;
int8_t minVelOffset = 0;
int8_t maxVelOffset = 0;
int8_t fadeOffset = 0;
int noteLengthOffset = 0;
boolean displayingAbout = false;

// Home screen globals
String currentNoteString = "";
unsigned int totalCounts = 0;
unsigned int countRate = 0;

int buttonTimeout = 200;
long buttonMillis = -buttonTimeout;
boolean play = false;
boolean displayButtonPress = false;

//byte offAmount = 0;
//byte counter = 0;
//boolean pin5State = false;

void checkForSeqReset() {
  if (digitalRead(6) == LOW && (isButtonPressed() == 3 || isButtonPressed() == 1)) {
    for (uint8_t j = 0; j < sizeof(seqNotes); j++) {
      seqNotes[j] = 255;
      seqVels[j] = 255;
    }
  }
}

String noteNumberToString(int number) {
  return baseNotes[number % 12] + String((number / 12) - 2);
}

int stringToNoteNumber(String noteName) {
  uint8_t ind;
  for (unsigned int i = 0; i < sizeof(baseNotes); i++) {
    if (noteName.substring(0, noteName.length() - 1).equals(baseNotes[i])) {
      ind = i;
    }
  }
  return (noteName.substring(noteName.length() - 1, noteName.length()).toInt() + 2) * 12 + ind;
}

void readLastConfiguration() {
  // Big, big bodge
  rootNoteNumber = EEPROM.read(1);
  rootNoteNumber = constrain(rootNoteNumber, 0, 127);
  octRange       = EEPROM.read(2);
  octRange = constrain(octRange, 0, 11);
  minVel         = EEPROM.read(3);
  minVel = constrain(minVel, 0, 127);
  maxVel         = EEPROM.read(4);
  maxVel = constrain(maxVel, 0, 127);
  seqLengthIndex = EEPROM.read(5);
  seqLengthIndex = constrain(seqLengthIndex, 0, 10);
  seqTempo       = EEPROM.read(6);
  seqTempo = constrain(seqTempo, 0, 200);
  seqDivIndex    = EEPROM.read(7);
  seqDivIndex = constrain(seqDivIndex, 0, 4);
  modeNumber     = EEPROM.read(8);
  modeNumber = constrain(modeNumber, 0, 1);
  byte i1        = EEPROM.read(9);
  byte i2        = EEPROM.read(10);
  scaleTypeIndex = EEPROM.read(11);
  fadeAmount     = EEPROM.read(12);
  fadeAmount = constrain(fadeAmount, 0, 100);
  scaleTypeIndex = constrain(scaleTypeIndex, 0, 7);
  noteLength = word(i2, i1);
  noteLength = constrain(noteLength, 0, 2000);
}


void setup() {
  Serial.begin(9600);
  Serial.println(F("Booted and doing stuff"));
  Serial1.begin(31250);
  readLastConfiguration();
  if (seqTempo != 0) {
    int msInterval = 60000 / (seqTempo * seqDivs[seqDivIndex]);
    FlexiTimer2::set(msInterval, playSequencer);
    FlexiTimer2::start();
  }
  display.begin(LCD_COLS, LCD_ROWS);
  setupEncoder();
  attachInterrupt(digitalPinToInterrupt(7), countDetected, RISING);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  if (seqTempo == 0) seqTempo = 120;
  Serial.println(F("Setup Done"));
}

void loop() {

  analogWrite(5, 2*(fadeAmount+fadeOffset));

  if (menuStatus == 0) {
  displayHomescreen();
  } else {
    displayMenu();
  }

  if (millis() - buttonMillis > buttonTimeout) { // Debounce, kinda
  displayButtonPress = false;
  if (digitalRead(6) == LOW) {
      play = true;
      displayButtonPress = true;
      buttonMillis = millis();
    }
  }

  updateEncoder();
  updateNotes();
}



