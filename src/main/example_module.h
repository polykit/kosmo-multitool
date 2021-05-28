result handleExample(eventMask event, navNode& nav, prompt &item);

MENU(subMenuExample,"EXAMPLE",handleExample,(Menu::eventMask)(enterEvent|exitEvent),wrapStyle
  // add meu items here
  ,OP("EXAMPLE ITEM",doNothing,noEvent)
  ,EXIT("<BACK")
);
