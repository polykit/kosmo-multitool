int arpTempo = 120;
int arpNoteValue = 2;
int arpNoteLength = 50;
bool arpChords = false;
bool arpHold = false;

result handleArp(eventMask event, navNode& nav, prompt &item);
result arpClearHold();

SELECT(arpNoteValue,arpNoteValueMenu,"NOTE#:",doNothing,noEvent,noStyle
  ,VALUE("1/4",1,doNothing,noEvent)
  ,VALUE("1/8",2,doNothing,noEvent)
  ,VALUE("1/16",4,doNothing,noEvent)
  ,VALUE("1/32",8,doNothing,noEvent)
);

TOGGLE(arpChords,arpChordsMenu,"CHORDS:",doNothing,noEvent,wrapStyle
  ,VALUE("ON",HIGH,doNothing,noEvent)
  ,VALUE("OFF",LOW,doNothing,noEvent)
);

TOGGLE(arpHold,arpHoldMenu,"HOLD:",doNothing,noEvent,wrapStyle
  ,VALUE("ON",HIGH,doNothing,noEvent)
  ,VALUE("OFF",LOW,arpClearHold,enterEvent)
);

MENU(subMenuArp,"ARPEGGIATOR",handleArp,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  ,SUBMENU(arpHoldMenu)
  ,FIELD(arpTempo,"TEMPO:","",0,300,5,1,doNothing,noEvent,noStyle)
  ,SUBMENU(arpNoteValueMenu)
  ,FIELD(arpNoteLength,"LENGTH:","%",1,100,10,1,doNothing,noEvent,noStyle)
  //,SUBMENU(arpChordsMenu)
  ,EXIT("<BACK")
);
