result handleMidi2CV(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startMidi2CV();
    activeModule = &loopMidi2CV;
  }
  if (event == exitEvent) {
    activeModule = {};
    endMidi2CV();
  }
  return proceed;
}

// called once on module enter
void startMidi2CV() {
}

// called once on module exit
void endMidi2CV() {
  resetCVOut();
  resetGateOut();
}

// called constantly from main loop
void loopMidi2CV() {
  byte note = MIDI_NOTES_ACTIVE.get(MIDI_NOTES_ACTIVE.size()-1);
  if (note > 0) {
    float cv = getCVFromNote(note+12*midi2CVOctave);
    setCVOut(0, cv, true);
    setGateOutDuration(0, 10);
  }
}
