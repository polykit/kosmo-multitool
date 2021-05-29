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
LinkedList<byte> arpHoldNotes;

void startArp() {
  arpLast = millis();
  arpMidiPos = 0;
}

void endArp() {
  arpHoldNotes.clear();
  resetCVOut();
  resetGateOut();
}

void loopArp() {
  byte note;
  int totalLength = 60000/arpTempo/arpNoteValue;

  if (millis() - arpLast > totalLength) {
    if (arpHold) {
      for (int i=0; i<MIDI_NOTES_ACTIVE.size(); i++) {
        arpHoldNotes.add(MIDI_NOTES_ACTIVE.remove(i));
      }
      note = arpHoldNotes.get(arpMidiPos);
    } else {
      note = MIDI_NOTES_ACTIVE.get(arpMidiPos);
    }

    if (note > 0) {
      float cv = getCVFromNote(note);
      setCVOut(0, cv, true);
      setGateOutDuration(0, totalLength*arpNoteLength/100);
      arpLast = millis();
      arpMidiPos++;
    }

    if (arpHold) {
      if (arpMidiPos >= arpHoldNotes.size()) {
        arpMidiPos = 0;
      }
    } else {
      if (arpMidiPos >= MIDI_NOTES_ACTIVE.size()) {
        arpMidiPos = 0;
      }
    }
  }
}

result arpClearHold() {
  arpHoldNotes.clear();
  return proceed;
}
