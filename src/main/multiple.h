int multipleMode = 0;

result handleMultiple(eventMask event, navNode& nav, prompt &item);

SELECT(multipleMode,multipleModeMenu,"MODE",doNothing,noEvent,noStyle
  ,VALUE("1>14",0,doNothing,noEvent)
  ,VALUE("1,3>12,34",1,doNothing,noEvent)
);

MENU(subMenuMultiple,"MULTIPLE",handleMultiple,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  ,SUBMENU(multipleModeMenu)
  ,EXIT("<BACK")
);
