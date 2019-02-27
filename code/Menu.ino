const String homeScreen [] = {"SCL : ", "NOTE: ", "TOT : "};
const String menuRoot [] = {"HOME", "Mode", "Scale Type", "Root Note", "Range", "Min Velocity", "Max Velocity","Note Length","Seq. Steps", "Seq. BPM", "Seq. Divider" , "Brightness", "About"};
const String modesString [] = {"Single Note", "Sequencer"};

int screenRefreshRate = 50;
long lastScreenRefresh = -screenRefreshRate;

/* Menu Statuses
 *  0: Home screen
 *  1: Mode Menu (Note, Sequencer, Control?)
 *  2: Scale Type Submenu
 *  3: Root Note Submenu
 *  4: Range Submenu
 *  5: Min. Velocity Submenu
 *  6: Max. Velocity Submenu
 *  7: Note Length Submenu
 *  8: Sequencer Length
 *  9: Sequencer Tempo
 *  10: Sequencer divider
 *  11: LED Brightness
 *  12: About
 *  13: Menu root
 */

void displayHomescreen(){
  if(millis() - lastScreenRefresh > screenRefreshRate){
    if(modeNumber == 0){
      display.clear();
      display.setCursor(0,0);
      display.print(F("SCL : "));
      display.setCursor(0,1);
      display.print(F("NOTE: "));
      display.setCursor(0,2);
      display.print(F("TOT : "));
      // Line 1
      display.setCursor(6,0);
      rootNoteStr = noteNumberToString(rootNoteNumber); // Move this to only update when the root note is changed from the menu
      display.print(rootNoteStr);
      display.setCursor(10,0);
      display.print(scaleTypesString[scaleTypeIndex]);
      display.setCursor(18,0);
      display.print(octRange);
  
      // Line 2
      display.setCursor(6,1);
      display.print(currentNoteString);
      display.setCursor(10,1);
      display.print(currentVel);
      display.setCursor(15,1);
      display.print(noteLength);
      
      // Line 3
      display.setCursor(6,2);
      display.print(totalCounts);

      // Line 4
      display.setCursor(0,3);
      display.print(F("MODE:"));
      display.setCursor(6,3);
      display.print(modesString[modeNumber]);
      if(displayButtonPress){
        display.setCursor(19,3);
        display.print(("X"));
      }
      
      
      lastScreenRefresh = millis();


      
    } else if (modeNumber == 1){
      display.clear();
      display.print(F("SCL : "));
      display.setCursor(0,1);
      display.print(F("NOTE: "));
      display.setCursor(0,2);
      display.print(F("TOT : "));
      // Line 1
      display.setCursor(6,0);
      rootNoteStr = noteNumberToString(rootNoteNumber); // Move this to only update when the root note is changed from the menu
      display.print(rootNoteStr);
      display.setCursor(10,0);
      display.print(scaleTypesString[scaleTypeIndex]);
      display.setCursor(19,0);
      display.print(octRange);
  
      // Line 2
      
      display.setCursor(6,1);
      display.print(noteNumberToString(lastNoteNumber));
      display.setCursor(10,1);
      display.print(lastVel);
      display.setCursor(15,1);
      display.print(seqTempo);
      
      // Line 3
      display.setCursor(6,2);
      display.print(totalCounts);
      
      // Line 4
      display.setCursor(0,3);
      display.print(F("MODE:"));
      display.setCursor(6,3);
      display.print(modesString[modeNumber]);
      if(displayButtonPress){
        display.setCursor(19,3);
        display.print(("X"));
      }
      lastScreenRefresh = millis();
      //checkForSeqReset();
    }
  }
}


void displayMenu(){
  //display.clear();
  switch(menuStatus){
    case 0:
      // Display the home screen, handled elsewhere
      break;

    case 1:
      // Display Mode Menu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Mode:"));
        display.setCursor(0,1);
        Serial.println(modesString[modeNumber + modeOffset]);
        display.print(modesString[modeNumber + modeOffset]);
        lastScreenRefresh = millis();
      }
      break;
    case 2:
      // Scale type submenu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Scale Type:"));
        display.setCursor(0,1);
        Serial.print(F("Scale index: "));
        Serial.println(scaleTypeIndex + scaleMenuOffset);
        display.print(scaleTypesString[scaleTypeIndex + scaleMenuOffset]);
        lastScreenRefresh = millis();
      }
      break;
      
    case 3:
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Root Note:"));
        display.setCursor(0,1);
        Serial.print(F("Root index: "));
        Serial.println(rootNoteNumber + rootNoteOffset);
        display.print(noteNumberToString(rootNoteNumber + rootNoteOffset));
        lastScreenRefresh = millis();
      }
      break;
      
    case 4:
      // Range submenu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Octave Range:"));
        display.setCursor(0,1);
        Serial.print(F("Octave: "));
        Serial.println(octRange + rangeOffset);
        display.print(octRange + rangeOffset);
        lastScreenRefresh = millis();
      }
      break;
    case 5:
      // Min. velocity submenu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Min Velocity:"));
        display.setCursor(0,1);
        Serial.print(F("Min Velocity: "));
        Serial.println(minVel + minVelOffset);
        display.print(minVel + minVelOffset);
        lastScreenRefresh = millis();
      }
      break;
    case 6:
      // Max. velocity submenu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Max Velocity:"));
        display.setCursor(0,1);
        Serial.print(F("Max Velocity: "));
        Serial.println(maxVel + maxVelOffset);
        display.print(maxVel + maxVelOffset);
        lastScreenRefresh = millis();
      }
      break;
    case 7:
      // Note length submenu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Note length (ms):"));
        display.setCursor(0,1);
        Serial.print(F("Note Length: "));
        Serial.println(noteLength + noteLengthOffset);
        display.print(noteLength + noteLengthOffset);
        lastScreenRefresh = millis();
      }
      break;

    case 8:
      // Sequencer length menu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Sequencer Steps:"));
        display.setCursor(0,1);
        Serial.print(F("Arp Length: "));
        Serial.println(seqLengths[seqLengthIndex + seqLengthOffset]);
        display.print(seqLengths[seqLengthIndex + seqLengthOffset]);
        lastScreenRefresh = millis();
      }
      break;

    case 9:
      // Sequencer tempo menu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Sequencer BPM:"));
        display.setCursor(0,1);
        Serial.print(F("Seq BPM: "));
        Serial.println(seqTempo + seqTempoOffset);
        display.print(seqTempo + seqTempoOffset);
        lastScreenRefresh = millis();
      }
      break;
    
    case 10:
      // Sequencer divider menu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("Sequencer Divider:"));
        display.setCursor(0,1);
        Serial.print(F("Seq Div: "));
        Serial.println(seqDivs[seqDivIndex + seqDivOffset]);
        display.print(seqDivs[seqDivIndex + seqDivOffset]);
        lastScreenRefresh = millis();
      }
      break;
    case 11:
      // LED fade
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        display.setCursor(0,0);
        display.print(F("LED Brightness:"));
        display.setCursor(0,1);
        Serial.print(F("LED Fade: "));
        Serial.println(fadeAmount + fadeOffset);
        display.print(fadeAmount + fadeOffset);
        lastScreenRefresh = millis();
      }
      break;
        
    case 12:
      // About submenu
      if(!displayingAbout){
        display.clear();
        display.setCursor(5,0);
        display.print(F("cVert v0.91"));
        display.setCursor(2,1);
        display.print(F("by: Danny Walker"));
        display.setCursor(8,2);
        display.print(F("2018"));
        display.setCursor(0,3);
        display.print(F("Twitter @walkerdanny"));
        displayingAbout = true;
      }
      break;
    case 13:
      // Root of menu
      if(millis() - lastScreenRefresh > screenRefreshRate){
        display.clear();
        // Display root of menu
        for(int i = 0; i < LCD_ROWS; i++){
          display.setCursor(0, i);
          display.print(menuRoot[i + menuRootOffset]); // need to check the menu status in the encoder service and increase the offset to scroll
        }
        display.setCursor(19, selectedLine);
        display.print(">");
        lastScreenRefresh = millis();
      }
      break;
  }
}
