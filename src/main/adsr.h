result handleADSR(eventMask event, navNode& nav, prompt &item);

unsigned int adsrAttack = 50;
unsigned int adsrDecay = 50;
float adsrSustain = 7.0f;
unsigned int adsrRelease = 250;
float adsrAmplitude = 8.0f;

MENU(subMenuADSR,"ADSR",handleADSR,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  ,FIELD(adsrAttack,"ATTACK ","MS",0,5000,100,10,doNothing,noEvent,noStyle)
  ,FIELD(adsrDecay,"DECAY  ","MS",0,5000,100,10,doNothing,noEvent,noStyle)
  ,FIELD(adsrSustain,"SUSTAIN","V",0,10,1,0.1,doNothing,noEvent,noStyle)
  ,FIELD(adsrRelease,"RELEASE","MS",0,5000,100,10,doNothing,noEvent,noStyle)
  ,FIELD(adsrAmplitude,"MAX AMP","V",0,10,1,0.1,doNothing,noEvent,noStyle)
  ,EXIT("<BACK")
);
