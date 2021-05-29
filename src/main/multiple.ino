result handleMultiple(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startMultiple();
    activeModule = &loopMultiple;
  }
  if (event == exitEvent) {
    activeModule = {};
    endMultiple();
  }
  return proceed;
}

// called once on module enter
void startMultiple() {
}

// called once on module exit
void endMultiple() {
  resetCVOut();
  resetGateOut();
}

// called constantly from main loop
void loopMultiple() {
  if (multipleMode == 0) {
    float cv = getCVIn(0, true);
    bool gate = getGateIn(0);
    for (int i=0; i<4; i++) {
      setCVOut(0, cv, true);
      setGateOut(0, gate);
    }
  }
  if (multipleMode == 1) {
    float cv0 = getCVIn(0, true);
    float cv1 = getCVIn(2, true);
    setCVOut(0, cv0, true);
    setCVOut(1, cv0, true);
    setCVOut(2, cv1, true);
    setCVOut(3, cv1, true);
    bool gate0 = getGateIn(0);
    bool gate1 = getGateIn(2);
    setGateOut(0, gate0);
    setGateOut(1, gate0);
    setGateOut(2, gate1);
    setGateOut(3, gate1);
  }
}
