result handleQuantizer(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startQuantizer();
    activeModule = &loopQuantizer;
  }
  if (event == exitEvent) {
    activeModule = {};
    endQuantizer();
  }
  return proceed;
}

// called once on module enter
void startQuantizer() {
}

// called once on module exit
void endQuantizer() {
  resetCVOut();
  resetGateOut();
}

float quantLastCV = 0;

// called constantly from main loop
void loopQuantizer() {
  float cv = quantLastCV;

  if (!quantTrigger || getGateIn(0)) {
    cv = getCVIn(0, true);
    cv = round(12 * cv) * (1 / 12.0f) +  quantOffset;
    if (cv < 0 ) cv = 0;
  }

  if (cv != quantLastCV) {
    setCVOut(0, cv, true);
    setGateOutDuration(0, 10);
    quantLastCV = cv;
  }
}
