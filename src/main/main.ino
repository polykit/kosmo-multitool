#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4728.h>
#include <MIDI.h>
#include <menu.h>
#include <menuIO/U8x8Out.h>
#include <U8x8lib.h>
#include <RotaryEncoder.h>
#include <Bounce2.h>
#include <LinkedList.h>

//#include "example_module.h"
#include "arp.h"
#include "multiple.h"
#include "midi2cv.h"

#define DEBUG 1
#define CALIBRATE_CV_IN 0
#define CALIBRATE_CV_OUT 0

float CV_IN_CORRECTION[4] = {1.0000f, 1.0000f, 1.0000f, 1.0000f};
unsigned int CV_OUT_LUT[4][21];
int LED_PINS[2] = {13, A3};
int GATE_IN_PINS[4] = {6, 7, 8, 9};
int GATE_OUT_PINS[4] = {2, 3, 4, 5};
unsigned long GATE_OUT_DURATION[4];
int SWITCH_PINS[4] = {A0, A1, A2, 12};
Bounce2::Button SWITCHES[4];
int MIDI_CHANNEL = 1;
int OCTAVE_OFFSET = 3;
LinkedList<byte> MIDI_NOTES_ACTIVE;
void (*activeModule)(void);

Adafruit_ADS1115 ads;
Adafruit_MCP4728 mcp;
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
RotaryEncoder encoder(10, 11, RotaryEncoder::LatchMode::TWO03);

// menu
MENU(mainMenu,"KOSMO MULTITOOL",doNothing,noEvent,wrapStyle
  //,SUBMENU(subMenuExample)
  ,SUBMENU(subMenuArp)
  ,SUBMENU(subMenuMidi2CV)
  ,SUBMENU(subMenuMultiple)
);
#define MAX_DEPTH 3
noInput in;
MENU_OUTPUTS(out,MAX_DEPTH,U8X8_OUT(u8x8,{0,0,17,8}),NONE);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup(void) {
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Polykit Multitool. Initializing...");
  }
  
  // initialize adc
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();

  // initialize dac
  mcp.begin();

  // init midi
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL);

  // init display
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_u);
  
  for (int i=0; i<2; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }

  for (int i=0; i<4; i++) {
    pinMode(GATE_IN_PINS[i], INPUT);
    pinMode(GATE_OUT_PINS[i], OUTPUT);
    GATE_OUT_DURATION[i] = 0;
  }

  for (int i=0; i<4; i++) {
    SWITCHES[i] = Bounce2::Button();
    SWITCHES[i].attach(SWITCH_PINS[i], INPUT_PULLUP);
    SWITCHES[i].interval(10); 
    SWITCHES[i].setPressedState(LOW); 
  } 

  if (CALIBRATE_CV_IN) {
    u8x8.drawString(0,0,"CALIBRATING...");
    calibrateCVIn();
    u8x8.drawString(0,1,"DONE");
    while (1);
  } 

  if (CALIBRATE_CV_OUT) {
    u8x8.drawString(0,0,"CALIBRATING...");
    calibrateCVOut();
    u8x8.drawString(0,1,"DONE");
    while (1);
  } else {
    u8x8.drawString(0,0,"LOADING...");
    int addr = 0;
    for (int c=0; c<4; c++) {
      CV_OUT_LUT[c][0] = 0;
      for (int i=1; i<=20; i++) {
        EEPROM.get(addr, CV_OUT_LUT[c][i]);
        addr += sizeof(unsigned int);
      }
    }
  }

  for (int i=0; i<4; i++) {
    setLEDOn(0);
    setLEDOff(1);
    delay(100);
    setLEDOn(1);
    setLEDOff(0);
    delay(100);
  }
  setLEDOff(1);
}

void loop(void) {
  MIDI.read();
  encoder.tick();
  nav.poll();
  handleMenu();
  handleGateOutDurations();
  handleSwitches();
  if (activeModule) {
    activeModule();
  }
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  if (channel != MIDI_CHANNEL) return;
  MIDI_NOTES_ACTIVE.add(note);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel != MIDI_CHANNEL) return;
  for (int i=0; i<MIDI_NOTES_ACTIVE.size(); i++) {
    if (MIDI_NOTES_ACTIVE.get(i) == note) {
      MIDI_NOTES_ACTIVE.remove(i);
      return;
    }
  }
}

float getCVFromNote(byte note) {
  return (note-12*OCTAVE_OFFSET)/12.0f;
}

void handleMenu() {
  int dir = getEncoderDir();
  if (dir == (int)RotaryEncoder::Direction::COUNTERCLOCKWISE) {
    nav.doNav(navCmd(downCmd));
  } else if(dir == (int)RotaryEncoder::Direction::CLOCKWISE) {
    nav.doNav(navCmd(upCmd));    
  }
  if (isSwitchOn(3)) {
    nav.doNav(navCmd(enterCmd));
  }
}

int getEncoderDir() {
  return (int)encoder.getDirection();
}

int getEncoderPos() {
  return encoder.getPosition();
}

float getCVIn(int n, bool correct) {
  if (n>3) return -1;
  
  int16_t adc = ads.readADC_SingleEnded(n);
  float cv = ads.computeVolts(adc)*2;
  
  if (correct) {
    cv = cv*CV_IN_CORRECTION[n];
  }
  
  return cv;
}

void setCVOut(int n, float v, bool correct) {
  if (n>3) return;
  if (v>10) return;
  
  MCP4728_channel_t ch;
  if (n==0) ch = MCP4728_CHANNEL_A;
  if (n==1) ch = MCP4728_CHANNEL_B;
  if (n==2) ch = MCP4728_CHANNEL_C;
  if (n==3) ch = MCP4728_CHANNEL_D;
  int nv = map(v*1000, 0, 10000, 0, 4095);

  if (correct) {
    int pos = round(2*v);
    nv = nv*(CV_OUT_LUT[n][pos]/1000.0f);
  }

  mcp.setChannelValue(ch, nv, MCP4728_VREF_INTERNAL, MCP4728_GAIN_2X);
}

void calibrateCVIn() {
  if (DEBUG) Serial.println("ADC calibration started.");
  int numSamples = 20;
  for (int n=0; n<4; n++) {
    if (DEBUG) {
      Serial.print("CV");
      Serial.print(n);
      Serial.print("CV");
    }
    float cv = 0;
    for (int i=0; i<numSamples; i++) {
      cv += getCVIn(n, false);
    }
    if (DEBUG) Serial.println((cv/numSamples), 4);
  }
  if (DEBUG) Serial.println("ADC calibration ended.");
}

void calibrateCVOut() {
  if (DEBUG) Serial.println("DAC calibration started.");
  int numSamples = 20;
  float cv;
  int addr = 0;
  for (int c=0; c<4; c++) {
    if (DEBUG) {
      Serial.print("Channel: ");
      Serial.println(c);
    }
    CV_OUT_LUT[c][0] = 0;
    for (int i=1; i<=20; i++) {
      setCVOut(c, i*0.5f, false);
      delay(100);
      cv = 0;
      for (int j=0; j<numSamples; j++) {
        cv += getCVIn(c, true);
      }
      cv = cv/numSamples;
      CV_OUT_LUT[c][i] = round(1000*i*0.5f/cv);
      if (DEBUG) {
        Serial.print("Wanted: ");
        Serial.println(i*0.5f, 4);
        Serial.print("Real: ");
        Serial.println(cv, 4);
        Serial.print("LUT: ");
        Serial.println(CV_OUT_LUT[c][i]/1000.0f, 4);
      }
      EEPROM.put(addr, CV_OUT_LUT[c][i]);
      addr += sizeof(unsigned int);
    }
  }
  if (DEBUG) Serial.println("DAC calibration ended.");
}

void setGateOutDuration(int n, unsigned long duration_ms) {
  if (n>3) return;
  GATE_OUT_DURATION[n] = millis()+duration_ms;
  setGateOutHigh(n);
}

void handleGateOutDurations() {
  for (int n=0; n<4; n++) {
    if (GATE_OUT_DURATION[n] > 0) {
      long d = GATE_OUT_DURATION[n]-millis();
      if (d < 0) {
        setGateOutLow(n);
        GATE_OUT_DURATION[n] = 0;
      }
    }
  }
}

void setGateOutHigh(int n) {
  if (n>3) return;
  digitalWrite(GATE_OUT_PINS[n], HIGH);
}

void setGateOutLow(int n) {
  if (n>3) return;
  digitalWrite(GATE_OUT_PINS[n], LOW);
}

void setGateOut(int n, bool b) {
  if (n>3) return;
  digitalWrite(GATE_OUT_PINS[n], b);
}

bool getGateIn(int n) {
  if (n>3) return -1;
  return digitalRead(GATE_IN_PINS[n]);
}

void handleSwitches() {
  for (int n=0; n<4; n++) {
    SWITCHES[n].update();
  }
}

bool isSwitchOn(int n) {
  if (n>3) return 0;
  bool pressed = SWITCHES[n].pressed();
  return pressed;
}

void setLEDOn(int n) {
  if (n>1) return;
  digitalWrite(LED_PINS[n], HIGH);
}

void setLEDOff(int n) {
  if (n>1) return;
  digitalWrite(LED_PINS[n], LOW);
}

void resetCVOut() {
  for(int i=0; i<4; i++) {
    setCVOut(i, 0, false);
  }
}

void resetGateOut() {
  for(int i=0; i<4; i++) {
    setGateOutLow(i);
  }
}
