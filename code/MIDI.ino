#include "MIDIUSB.h"

byte currentNoteNumber = rootNoteNumber;
byte currentVel = 0;
byte index =0;
byte octIndex = 0;
byte velCounter = 0;
int timeout = 1000;
long prevMillis = -noteLength;
byte lastNoteNumber = 0;
byte lastVel = 0;
boolean noteTimedOut = true;
boolean playNextNote = false;

void playSequencer(){
  playNextNote = true;
}

void updateNotes() {
  index++;
  octIndex++;
  velCounter++;

  if (index >= scaleLengths[scaleTypeIndex]){
    index = 0;
  }

  if (octIndex >= octRange){
    octIndex = 0;
  }

  if (velCounter > maxVel || velCounter < minVel){
    velCounter = minVel;
  }  
  if(modeNumber == 0){
    // Single note mode
    if(play){
      currentNoteNumber = rootNoteNumber + (12* octIndex) + scales[scaleTypeIndex][index]; // Check that 0 <= note number <= 127
      currentNoteNumber = constrain(currentNoteNumber, 0, 127); // BODGE!
      currentVel = velCounter;
      currentNoteString = noteNumberToString(currentNoteNumber);
      noteOff(0, lastNoteNumber, lastVel); // intensity from 0-127
      noteOn(0, currentNoteNumber, currentVel);
      lastNoteNumber = currentNoteNumber;
      lastVel = currentVel;
      prevMillis = millis();
      play = false;
      noteTimedOut = false;
    }
  
    if (millis() - prevMillis > noteLength && !noteTimedOut){
      noteOff(0, lastNoteNumber, lastVel);
      noteTimedOut = true;
    }
    
    MidiUSB.flush();

    
  } else if (modeNumber == 1){
    // Sequencer Mode
    if(play){
      seqNotes[seqFillIndex] = rootNoteNumber + (12* octIndex) + scales[scaleTypeIndex][index];
      seqVels[seqFillIndex] = velCounter;
      seqFillIndex++;
      if(seqFillIndex >= seqLengths[seqLengthIndex]){
        seqFillIndex = 0;
      }
      play = false;
    }

    
    //int seqNoteLength = 60000/(seqTempo * seqDivs[seqDivIndex]); // ms between seq notes
    if (playNextNote){
      // It'd be nice to just turn off the current note and then turn on the next one
      // Unfortunately, this doesn't allow for modifying the note buffer while playing
      // For example, if the current note gets overwritten after the note on command has been sent,
      // it will never get turned off, as the note number will have changed.
      // Therefore, we still need to save the last note and velocity, and turn that off instead.
      noteOff(0, lastNoteNumber, lastVel);
      MidiUSB.flush();
      seqIndex++;
      
      
      if(seqIndex >= seqLengths[seqLengthIndex]){
        seqIndex = 0;
      }
      
      if(seqNotes[seqIndex] != 255){
        noteOn(0, seqNotes[seqIndex], seqVels[seqIndex]);
        Serial.print("Playing note: ");
        Serial.println(seqNotes[seqIndex]);
        Serial.println(millis());
        MidiUSB.flush();
        lastNoteNumber = seqNotes[seqIndex];
        lastVel = seqVels[seqIndex];
      }
      prevMillis = millis();
      playNextNote = false;
    }
    
  }
  

}

void countDetected(){
  play = true;
  totalCounts++;
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


void noteOn(byte channel, byte pitch, byte velocity) {
  digitalWrite(9, HIGH);
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  
  Serial1.write(0x90);
  Serial1.write(pitch);
  Serial1.write(velocity);
  
//  Serial.print(F("Note on: "));
//  Serial.print(pitch);
//  Serial.print(" ");
//  Serial.println(millis());
}

void noteOff(byte channel, byte pitch, byte velocity) {
  digitalWrite(9, LOW);
  
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  
  Serial1.write(0x80);
  Serial1.write(pitch);
  Serial1.write(velocity);
  
//  Serial.print(F("Note off: "));
//  Serial.print(pitch);
//  Serial.print(" ");
//  Serial.println(millis());
}

