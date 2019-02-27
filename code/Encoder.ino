#include <ClickEncoder.h>

ClickEncoder *encoder;
int16_t lastEnc, encVal;

void setupEncoder(){
  encoder = new ClickEncoder(14,15,16,4);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  lastEnc = -1;
  encoder->setAccelerationEnabled(false);
}

void timerIsr() {
  encoder->service();
}

void updateEncoder(){

  switch(menuStatus){
    case 0:
      // Display the home screen, handled elsewhere, encoder does nothing apart from monitor for clicks
      if(isButtonPressed() == 3){
        menuStatus = 13;
      }  else if (isButtonPressed() == 5){
        for (uint8_t j = 0; j < sizeof(seqNotes); j++){
          seqNotes[j] = 255;
          seqVels[j] = 255;
        }
      }
      break;

    case 1:
      // Mode submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        modeOffset += encVal;
        if(modeNumber + modeOffset < 0){
          Serial.println(F("Bottom of modes list selected and scrolling down  "));
          modeOffset = -modeNumber;
        }
        if(modeNumber + modeOffset >= 2){
          Serial.println(F("Top of modes list selected and scrolling up  "));
          modeOffset = 1 - modeNumber;
        }
      }

      if(isButtonPressed() == 3){
        modeNumber += modeOffset;
        menuStatus = 13;
        modeOffset = 0;
        EEPROM.update(8, modeNumber);
      }
      break;
      
    case 2:
      // Scale type submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        scaleMenuOffset += encVal;
        if(scaleTypeIndex + scaleMenuOffset < 0){
          Serial.print(F("Top of scales list selected and scrolling up  "));
          Serial.println(sizeof(scaleTypesString));
          scaleMenuOffset = -scaleTypeIndex;
        }
        if(scaleTypeIndex + scaleMenuOffset >= sizeof(scaleLengths)){
          Serial.print(F("Bottom of scales list selected and scrolling down  "));
          scaleMenuOffset = sizeof(scaleLengths) - scaleTypeIndex -1;
          Serial.println(scaleMenuOffset + scaleTypeIndex);
        }
      }

      if(isButtonPressed() == 3){
        scaleTypeIndex += scaleMenuOffset;
        menuStatus = 13;
        scaleMenuOffset = 0;
        EEPROM.update(11, scaleTypeIndex);
      }
      break;
      
    case 3:
      // Root note submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        rootNoteOffset += encVal;
        if(rootNoteNumber + rootNoteOffset < 0){
          Serial.print(F("Bottom of root notes selected and decreasing  "));
          rootNoteOffset = -rootNoteNumber;
          Serial.println(rootNoteOffset + rootNoteNumber);
        }
        if(rootNoteNumber + rootNoteOffset > 127){
          Serial.print(F("Top of scales list selected and increasing  "));
          rootNoteOffset = 127 - rootNoteNumber;
          Serial.println(rootNoteOffset + rootNoteNumber);
        }
      }

      if(isButtonPressed() == 3){
        rootNoteNumber += rootNoteOffset;
        menuStatus = 13;
        rootNoteOffset = 0;
        EEPROM.update(1, rootNoteNumber);
      }
      break;
      
    case 4:
      // Range submenu
      encVal = 0;
      encVal = encoder->getValue();
      //maxR = (127-rootNoteNumber)/12 + 1; // Stops the MIDI notes from overflowing the 0-127 range
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        rangeOffset += encVal;
        if(octRange + rangeOffset < 1){
          Serial.print(F("Bottom of offset selected and decreasing  "));
          rangeOffset = -octRange + 1;
          Serial.println(octRange + rangeOffset);
        }
        if(octRange + rangeOffset > (127-rootNoteNumber)/12 + 1){
          Serial.print(F("Top of range selected and increasing  "));
          rangeOffset = (127-rootNoteNumber)/12 + 1 - octRange;
          Serial.println(octRange + rangeOffset);
        }
      }

      if(isButtonPressed() == 3){
        octRange += rangeOffset;
        menuStatus = 13;
        rootNoteOffset = 0;
        EEPROM.update(2, octRange);
      }
      
      break;
      
    case 5:
      // Min. velocity submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        minVelOffset += encVal;
        if(minVel + minVelOffset < 0){
          Serial.print(F("Bottom of min vel selected and decreasing  "));
          minVelOffset = -minVel;
          Serial.println(minVel + minVelOffset);
        }
        if(minVel + minVelOffset > maxVel){
          Serial.print(F("Top of min vel selected and increasing  "));
          minVelOffset = maxVel - minVel;
          Serial.println(minVel + minVelOffset);
        }
      }

      if(isButtonPressed() == 3){
        minVel += minVelOffset;
        menuStatus = 13;
        minVelOffset = 0;
        EEPROM.update(3, minVel);
      }
      break;
      
    case 6:
      // Max. velocity submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        maxVelOffset += encVal;
        if(maxVel + maxVelOffset < minVel){
          Serial.print(F("Bottom of max vel selected and decreasing  "));
          maxVelOffset = minVel -maxVel;
          Serial.println(maxVel + maxVelOffset);
        }
        if(maxVel + maxVelOffset > 127){
          Serial.print(F("Top of max vel selected and increasing  "));
          maxVelOffset = 127 - maxVel;
          Serial.println(maxVel + maxVelOffset);
        }
      }

      if(isButtonPressed() == 3){
        maxVel += maxVelOffset;
        menuStatus = 13;
        maxVelOffset = 0;
        EEPROM.update(4, maxVel);
      }
      break;
      
    case 7:
      // Note length submenu
      encoder->setAccelerationEnabled(true);
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        noteLengthOffset += encVal;
        if(noteLength + noteLengthOffset < 1){
          Serial.print(F("Bottom of note length selected and decreasing  "));
          noteLengthOffset = -noteLength;
          Serial.println(noteLength + noteLengthOffset);
        }
        if(noteLength + noteLengthOffset > 15000){
          Serial.print(F("Top of note length selected and increasing  "));
          noteLengthOffset = 15000 - noteLength;
          Serial.println(noteLength + noteLengthOffset);
        }
      }

      if(isButtonPressed() == 3){
        noteLength += noteLengthOffset;
        menuStatus = 13;
        noteLengthOffset = 0;
        encoder->setAccelerationEnabled(false);
        EEPROM.update(9,lowByte(noteLength));
        EEPROM.update(10,highByte(noteLength));
      }
      break;

    case 8:
      // Sequencer length
      // Scale type submenu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        seqLengthOffset += encVal;
        if(seqLengthIndex + seqLengthOffset < 0){
          Serial.println(F("Top of seq length list selected and scrolling up  "));
          seqLengthOffset = -seqLengthIndex;
        }
        if(seqLengthIndex + seqLengthOffset >= sizeof(seqLengths)){
          Serial.println(F("Bottom of seq length list selected and scrolling down  "));
          seqLengthOffset = sizeof(seqLengths) - seqLengthIndex -1;
        }
      }

      if(isButtonPressed() == 3){
        seqLengthIndex += seqLengthOffset;
        menuStatus = 13;
        seqLengthOffset = 0;
        EEPROM.update(5, seqLengthIndex);
      }
      
      break;

    case 9:
      // Sequencer tempo

      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        seqTempoOffset += encVal;
        if(seqTempo + seqTempoOffset < 2){
          Serial.print(F("Bottom of seq tempo selected and decreasing  "));
          seqTempoOffset = -seqTempo;
          Serial.println(seqTempo + seqTempoOffset);
        }
        if(seqTempo + seqTempoOffset > 240){
          Serial.print(F("Top of seq tempo selected and increasing  "));
          seqTempoOffset = 240 - seqTempo;
          Serial.println(seqTempo + seqTempoOffset);
        }
      }

      if(isButtonPressed() == 3){
        seqTempo += seqTempoOffset;
        menuStatus = 13;
        seqTempoOffset = 0;
        EEPROM.update(6, seqTempo);
        int msInterval = 60000/(seqTempo * seqDivs[seqDivIndex]);
        FlexiTimer2::stop();
        FlexiTimer2::set(msInterval, playSequencer);
        FlexiTimer2::start();
      }

      break;

    case 10:
      // seq division
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        seqDivOffset += encVal;
        if(seqDivIndex + seqDivOffset < 0){
          Serial.print(F("Bottom of seq divs selected and decreasing  "));
          seqDivOffset = -seqDivIndex;
          Serial.println(seqDivOffset + seqDivIndex);
        }
        if(seqDivIndex + seqDivOffset >= sizeof(seqDivs)){
          Serial.print(F("Top of seq divs selected and increasing  "));
          seqDivOffset = sizeof(seqDivs) - seqDivIndex -1;
          Serial.println(seqDivOffset + seqDivIndex);
        }
      }

      if(isButtonPressed() == 3){
        seqDivIndex += seqDivOffset;
        menuStatus = 13;
        seqDivOffset = 0;
        EEPROM.update(7, seqDivIndex);
        int msInterval = 60000/(seqTempo * seqDivs[seqDivIndex]);
        FlexiTimer2::stop();
        FlexiTimer2::set(msInterval, playSequencer);
        FlexiTimer2::start();
      }

      break;

    case 11:
      // LED Brightness
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        fadeOffset += encVal;
        if(fadeAmount + fadeOffset < 0){
          //Serial.print(F("Bottom of LED fade selected and decreasing  "));
          fadeOffset = -fadeAmount;
          //Serial.println(fadeAmount + fadeOffset);
        }
        if(fadeAmount + fadeOffset > 100){
          fadeOffset = 100 - fadeAmount;
          
        }
//        Serial.print(F("Fade amount: "));
//        Serial.println(fadeAmount);
//        Serial.print(F("Fade offset: "));
//        Serial.println(fadeOffset);
//        Serial.println("");
      }

      if(isButtonPressed() == 3){
        fadeAmount += fadeOffset;
        fadeOffset = 0;
        menuStatus = 13;
        EEPROM.update(12, fadeAmount);
      }

      break;
    
    case 12:
      // About submenu
      if(isButtonPressed() == 3){
        displayingAbout = false;
        menuStatus = 13;
      }
      break;
      
    case 13:
      // Display root of menu
      encVal = 0;
      encVal = encoder->getValue();
      if(lastEnc += encVal != lastEnc){
        lastEnc += encVal;
        selectedLine += encVal;
        if(selectedLine < 0){
          selectedLine = 0;
          if(menuRootOffset>0) menuRootOffset--;
        }
        if(selectedLine>=4){
          selectedLine = 3;
          if(menuRootOffset<9) menuRootOffset++;
        }
      }

      checkForSeqReset();

      if(isButtonPressed() == 3){
        Serial.print(F("Entering menu option: "));
        Serial.println(selectedLine + menuRootOffset);
        // Do some magic with the menu offset and the selected line
        menuStatus = selectedLine + menuRootOffset;
      }
      break;
  }

}


int isButtonPressed(){
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case (ClickEncoder::Pressed):
        return 1;
        break;
      case (ClickEncoder::Released):
        return 2;
        break;
      case (ClickEncoder::Clicked):
        return 3;
        break;
      case (ClickEncoder::DoubleClicked):
        return 4;
        break;
      case (ClickEncoder::Held):
        return 5;
        break;
    }
  } else {
    return 0;
  }
}
