result handleADSR(eventMask event, navNode& nav, prompt &item) {
  if (event == enterEvent) {
    startADSR();
    activeModule = &loopADSR;
  }
  if (event == exitEvent) {
    activeModule = {};
    endADSR();
  }
  return proceed;
}

// called once on module enter
void startADSR() {
}

// called once on module exit
void endADSR() {
  resetCVOut();
  resetGateOut();
}

unsigned long adsrFrame = 0;
bool adsrActive = false;
bool adsrGateCycle = false;
float adsrCV;

// called constantly from main loop
void loopADSR() {
  bool gate = getGateIn(0);

  if (gate) {
    if (adsrGateCycle) {
      if (adsrActive) {
        adsrFrame = adsrCV / (adsrAmplitude / adsrAttack);
      } else {
        adsrFrame = 0;
      }
      adsrGateCycle = false;
    }
    adsrActive = true;
  } else {
    adsrGateCycle = true;
  }

  if (adsrActive) {
    adsrCV = ADSR(adsrAmplitude, adsrAttack, adsrDecay, adsrSustain, adsrRelease, adsrFrame, gate);
    for (int i=0; i<4; i++) {
      setCVOut(i, adsrCV, true);
    }
    if (adsrFrame <= (adsrAttack+adsrDecay) || !gate ) adsrFrame++;
    if (adsrFrame > (adsrAttack+adsrDecay+adsrRelease)) adsrActive = false;
  }
}

float ADSR(float maxamp, unsigned int attack, unsigned int decay, float sustain, unsigned int release, unsigned long frame, bool gate) {
	int duration = attack + decay + release;
	float amp = 0.0f;

	if (frame < duration) {
		if (frame <= attack) {
			// attack
			amp = frame * (maxamp / attack);
		} else if (frame <= (attack + decay)) {
			// decay
			amp = ((sustain - maxamp) / decay) * (frame - attack) + maxamp;
		} else if (gate) {
			// sustain
			amp = sustain;
		} else {
			// release
			amp = -(sustain / release) * (frame - (duration - release)) + sustain;
		}
	}

	return amp;
}
