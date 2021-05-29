result handleExample(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startExample();
    activeModule = &loopExample;
  }
  if (event == exitEvent) {
    activeModule = {};
    endExample();
  }
  return proceed;
}

// called once on module enter
void startExample() {
}

// called once on module exit
void endExample() {
  resetCVOut();
  resetGateOut();
}

// called constantly from main loop
void loopExample() {
}
