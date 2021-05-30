int midi2CVOctave = 0;

result handleMidi2CV(eventMask event, navNode& nav, prompt &item);

SELECT(midi2CVOctave,midi2CVOctaveMenu,"OCTAVE",doNothing,noEvent,noStyle
  ,VALUE("-3",-3,doNothing,noEvent)
  ,VALUE("-2",-2,doNothing,noEvent)
  ,VALUE("-1",-1,doNothing,noEvent)
  ,VALUE("0",0,doNothing,noEvent)
  ,VALUE("1",1,doNothing,noEvent)
  ,VALUE("2",2,doNothing,noEvent)
  ,VALUE("3",3,doNothing,noEvent)
);

MENU(subMenuMidi2CV,"MIDI TO CV",handleMidi2CV,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  ,SUBMENU(midi2CVOctaveMenu)
  ,EXIT("<BACK")
);
