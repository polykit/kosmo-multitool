int quantMode = 0;
float quantOffset = 0;
bool quantTrigger = false;

result handleQuantizer(eventMask event, navNode& nav, prompt &item);

SELECT(quantMode,quantModeMenu,"MODE",doNothing,noEvent,noStyle
  ,VALUE("CHROMATIC",0,doNothing,noEvent)
  ,VALUE("MAJOR",1,doNothing,noEvent)
  ,VALUE("MINOR",2,doNothing,noEvent)
);

TOGGLE(quantTrigger,quantTriggerMenu,"TRIGGER ",doNothing,noEvent,wrapStyle
  ,VALUE("ON",HIGH,doNothing,noEvent)
  ,VALUE("OFF",LOW,doNothing,noEvent)
);

MENU(subMenuQuantizer,"QUANTIZER",handleQuantizer,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  ,SUBMENU(quantModeMenu)
  ,FIELD(quantOffset,"OFFSET","V",-10,10,0.1,0.01,doNothing,noEvent,noStyle)
  ,SUBMENU(quantTriggerMenu)
  ,EXIT("<BACK")
);
