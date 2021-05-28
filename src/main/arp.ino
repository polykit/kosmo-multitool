result handleArp(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startArp();
    activeModule = &loopArp;
  }
  if (event == exitEvent) {
    activeModule = {};
    endArp();
  }
  return proceed;
}

unsigned long arpLast;
int arpMidiPos;

void startArp() {
  arpLast = millis();
  arpMidiPos = 0;
}

void endArp() {
}

void loopArp() {
  int totalLength = 60000/arpTempo/arpNoteValue;
  if (millis() - arpLast > totalLength) {
    byte note = MIDI_NOTES_ACTIVE.get(arpMidiPos);
    if (note > 0) {
      float cv = getCVFromNote(note);
      setCVOut(0, cv, true);
      setGateOutDuration(0, totalLength*arpNoteLength/100);
      arpLast = millis();
      arpMidiPos++;
    }
    if (arpMidiPos > MIDI_NOTES_ACTIVE.size()-1) {
      arpMidiPos = 0;
    }
  }
}
